#include <unicode/binarysearch.hpp>
#include <unicode/normalizer.hpp>
#include <algorithm>
#include <cstddef>
#include <utility>


namespace Unicode {


	QuickCheck Normalizer::is_impl (const CodePoint * begin, const CodePoint * end, QuickCheck (CodePointInfo::* qc)) const noexcept {
	
		QuickCheck retr=QuickCheck::Yes;
		const CodePointInfo * prev=nullptr;
		for (;begin!=end;++begin) {
		
			auto cpi=begin->GetInfo(locale);
			
			if (cpi!=nullptr) {
			
				//	Check quick check
				switch (cpi->*qc) {
				
					//	No is definite, end at once
					case QuickCheck::No:
						return QuickCheck::No;
					//	Maybe overwrites yes
					case QuickCheck::Maybe:
						retr=QuickCheck::Maybe;
					//	Yes is ignored (it is the default)
					default:
						break;
				
				}
				
				//	Check that combining marks are ordered
				if (
					(cpi->CanonicalCombiningClass!=0) &&
					(prev!=nullptr) &&
					(prev->CanonicalCombiningClass!=0) &&
					(cpi->CanonicalCombiningClass<prev->CanonicalCombiningClass)
				) return QuickCheck::No;
			
			}
			
			prev=cpi;
		
		}
		
		return retr;
	
	}
	
	
	void Normalizer::decompose (std::vector<CodePoint> & vec, CodePoint cp) const {
	
		auto cpi=cp.GetInfo(locale);
		if (
			(cpi==nullptr) ||
			(cpi->DecompositionMapping.size()==0)
		) {
		
			vec.push_back(cp);
			
			return;
		
		}
		
		for (auto cp : cpi->DecompositionMapping) decompose(vec,cp);
	
	}
	
	
	std::vector<CodePoint> Normalizer::decompose (const CodePoint * begin, const CodePoint * end) const {
	
		std::vector<CodePoint> retr;
		for (;begin!=end;++begin) decompose(retr,*begin);
		
		return retr;
	
	}
	
	
	void Normalizer::sort_impl (std::vector<CodePoint>::iterator begin, std::vector<CodePoint>::iterator end) const {
	
		std::stable_sort(
			begin,
			end,
			[&] (CodePoint a, CodePoint b) noexcept {
			
				return a.GetInfo(locale)->CanonicalCombiningClass<b.GetInfo(locale)->CanonicalCombiningClass;
			
			}
		);
	
	}
	
	
	void Normalizer::sort (std::vector<CodePoint>::iterator begin, std::vector<CodePoint>::iterator end) const {
	
		auto b=begin;
		for (;begin!=end;++begin) {
		
			auto cpi=begin->GetInfo(locale);
			if (!((cpi==nullptr) || (cpi->CanonicalCombiningClass==0))) continue;
			
			if (b!=begin) sort_impl(b,begin);
			
			b=begin+1;
		
		}
		
		if (b!=end) sort_impl(b,end);
	
	}
	
	
	static std::optional<CodePoint> compose_impl (CodePoint starter, CodePoint joiner, const Composition * begin, const Composition * end) noexcept {
	
		auto lower=std::lower_bound(
			begin,
			end,
			starter,
			[] (const Composition & a, CodePoint b) noexcept {
			
				return a.CodePoints[0]<b;
			
			}
		);
		if (
			(lower==end) ||
			(lower->CodePoints[0]!=starter)
		) return std::optional<CodePoint>{};
		
		auto upper=std::upper_bound(
			begin,
			end,
			starter,
			[] (CodePoint a, const Composition & b) noexcept {
			
				return a<b.CodePoints[0];
			
			}
		);
		
		auto iter=BinarySearch(
			lower,
			upper,
			joiner,
			[] (const Composition & a, CodePoint b) noexcept {
			
				return a.CodePoints[1]<b;
			
			},
			[] (const Composition & a, CodePoint b) noexcept {
			
				return a.CodePoints[1]==b;
			
			}
		);
		
		if (iter==upper) return std::optional<CodePoint>{};
		return std::optional<CodePoint>(iter->Result);
	
	}
	
	
	std::optional<CodePoint> Normalizer::compose (CodePoint starter, CodePoint joiner) const noexcept {
	
		return compose_impl(
			starter,
			joiner,
			locale.Compositions.begin(),
			locale.Compositions.end()
		);
	
	}
	
	
	std::vector<CodePoint> Normalizer::compose (std::vector<CodePoint> vec) const {
	
		//	In many (most?) instances the decomposes string will
		//	also be the composed string, in this case we don't
		//	need to maintain a second vector, therefore this is
		//	kept disengaged until it's actually needed
		std::optional<std::vector<CodePoint>> retr;
		
		//	This is the location of the last starter within vec
		std::optional<std::size_t> vec_starter;
		
		//	This is the location of the last starter within retr
		//	(if applicable)
		std::optional<std::size_t> retr_starter;
		
		//	This is the canonical combining class of the last
		//	code point encountered, used to determine if the
		//	current code point is blocked
		std::size_t ccc=0;
		
		//	This is the position within retr
		std::size_t pos=0;
		
		//	The begin iterator of vec
		auto vb=vec.begin();
		
		//	Iterate over the input vector
		for (auto begin=vb,end=vec.end();begin!=end;++begin) {
		
			//	If this code point is a starter, do maintenance
			//	and continue to next iteration
			auto cpi=begin->GetInfo(locale);
			if ((cpi==nullptr) || (cpi->CanonicalCombiningClass==0)) {
			
				vec_starter=begin-vb;
				retr_starter=pos;
				
				goto maintenance;
			
			}
			
			//	If there's no starter to combine with, or if this
			//	code point is blocked, do maintenance and continue
			//	to next iteration
			if (!vec_starter || (ccc==cpi->CanonicalCombiningClass)) goto maintenance;

			{
			
				//	See if this code point composes with the last
				//	starter, if it doesn't do maintenance and continue
				//	to next iteration			
				auto repl=compose(vec[*vec_starter],*begin);
				if (!repl) {
				
					//	If this code point combines with the
					//	starter, it will disappear, therefore we
					//	conditionally record its canonical combining
					//	class, because if we unconditionally record it,
					//	further code points could be blocked by
					//	a code point which no longer exists
					ccc=cpi->CanonicalCombiningClass;
					goto maintenance;
					
				}
				
				//	Copy input array if necessary
				if (!retr) {
				
					retr.emplace(vec.begin(),begin);
					retr_starter=vec_starter;
					
				}
				
				(*retr)[*retr_starter]=*repl;
				
				continue;
				
			}
			
			{
			
				maintenance:
				
				++pos;
				if (retr) retr->push_back(*begin);
			
			}
		
		}
		
		return retr ? std::move(*retr) : vec;
	
	}
	
	
	bool Normalizer::IsNFD (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return is_impl(
			begin,
			end,
			&CodePointInfo::NFDQuickCheck
		)==QuickCheck::Yes;
	
	}
	
	
	bool Normalizer::IsNFC (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return is_impl(
			begin,
			end,
			&CodePointInfo::NFCQuickCheck
		)==QuickCheck::Yes;
	
	}
	
	
	std::vector<CodePoint> Normalizer::ToNFD (const CodePoint * begin, const CodePoint * end) const {
	
		auto retr=decompose(begin,end);
		
		sort(retr.begin(),retr.end());
		
		return retr;
	
	}
	
	
	std::vector<CodePoint> Normalizer::ToNFC (const CodePoint * begin, const CodePoint * end) const {
	
		return compose(ToNFD(begin,end));
	
	}


}
