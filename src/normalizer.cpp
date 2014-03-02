#include <unicode/normalizer.hpp>
#include <algorithm>


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


}
