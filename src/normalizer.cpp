#include <unicode/normalizer.hpp>
#include <algorithm>
#include <utility>


namespace Unicode {


	//	Sorts combining marks
	static void sort (CodePoint * begin, CodePoint * end, const Locale & locale) {
	
		std::stable_sort(
			++begin,
			end,
			[&] (CodePoint a, CodePoint b) noexcept -> bool {
			
				auto a_i=a.GetInfo(locale);
				if (a_i==nullptr) return false;
				auto b_i=b.GetInfo(locale);
				if (b_i==nullptr) return false;
				
				return a_i->CombiningClass<b_i->CombiningClass;
		
			}
		);
	
	}
	
	
	static const CodePoint * get_joiners (const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
	
		while ((++begin)!=end) {
		
			auto cpi=begin->GetInfo(locale);
			
			if (
				(cpi==nullptr) ||
				(cpi->CombiningClass==0)
			) break;
		
		}
		
		return begin;
	
	}
	
	
	static void decompose (std::vector<CodePoint> & vec, CodePoint cp, const Locale & locale) {
	
		auto cpi=cp.GetInfo(locale);
		
		if (cpi->DecompositionLength==0) vec.push_back(cp);
		else for (std::size_t i=0;i<cpi->DecompositionLength;++i) decompose(
			vec,
			cpi->Decomposition[i],
			locale
		);
	
	}
	
	
	std::vector<CodePoint> Normalizer::nfd (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		std::vector<CodePoint> retr;
		
		while (begin!=end) {
		
			auto size=retr.size();
		
			decompose(retr,*begin,locale);
			
			auto e=get_joiners(begin,end,locale);
			while ((++begin)!=e) retr.push_back(*begin);
			
			sort(
				&retr[0]+size,
				&retr[0]+retr.size(),
				locale
			);
		
		}
		
		return retr;
	
	}
	
	
	bool Normalizer::is_nfd (const CodePoint * begin, const CodePoint * end, const Locale & locale) const noexcept {
	
		for (
			const CodePointInfo * prev=nullptr;
			begin!=end;
			++begin
		) {
		
			auto cpi=begin->GetInfo(locale);
			
			if (
				!(
					(cpi==nullptr) ||
					(cpi->CombiningClass==0)
				) &&
				(
					(cpi->DecompositionLength!=0) ||
					(
						!(
							(prev==nullptr) ||
							(prev->CombiningClass==0)
						) &&
						(prev->CombiningClass>cpi->CombiningClass)
					)
				)
			) return false;
			
			prev=cpi;
		
		}
		
		return true;
	
	}
	
	
	std::vector<CodePoint> Normalizer::nfc (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		return std::vector<CodePoint>{};
	
	}
	
	
	std::vector<CodePoint> Normalizer::Normalize (const CodePoint * begin, const CodePoint * end) const {
	
		auto & locale=GetLocale();
	
		switch (nf) {
		
			case NormalForm::C:
			default:
				return nfc(begin,end,locale);
		
			case NormalForm::D:
				return nfd(begin,end,locale);
		
		}
	
	}
	
	
	bool Normalizer::IsNormalized (const CodePoint * begin, const CodePoint * end) const {
	
		auto & locale=GetLocale();
		
		switch (nf) {
		
			case NormalForm::C:
			default:
				return false;
				
			case NormalForm::D:
				return is_nfd(begin,end,locale);
		
		}
	
	}


}
