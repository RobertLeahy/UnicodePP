#include <unicode/caseconverter.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/vector.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>


namespace Unicode {


	bool CaseConverter::matches (
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		const Array<Condition> & conds
	) const noexcept {
	
		for (auto & cond : conds) if (!cond(loc,begin,end,locale)) return false;
	
		return true;
	
	}
	
	
	static Array<CodePoint> to_array (const void * ptr, std::size_t size) noexcept {
	
		return Array<CodePoint>(
			reinterpret_cast<const CodePoint *>(ptr),
			size
		);
	
	}


	std::optional<Array<CodePoint>> CaseConverter::get_best (
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		const Array<CaseMapping> & arr
	) const noexcept {
	
		//	We try and find not just an appropriate
		//	case mapping, but the BEST case mapping,
		//	defined as the case mapping which matches
		//	the MOST conditions.  In the case of a tie
		//	we arbitrarily choose to go with the earliest
		//	such mapping
		
		std::optional<Array<CodePoint>> best;
		std::size_t count=0;
		for (auto & mapping : arr) {
		
			if (
				matches(loc,begin,end,mapping.Conditions) &&
				(
					!best ||
					(mapping.Conditions.size()>count)
				)
			) {
			
				best=to_array(
					mapping.Mapping.begin(),
					mapping.Mapping.Size
				);
				count=mapping.Conditions.size();
			
			}
		
		}
		
		return best;
	
	}
	
	
	Array<CodePoint> CaseConverter::get (
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		std::optional<CodePoint::Type> (CodePointInfo::* simple),
		Array<CaseMapping> (CodePointInfo::* full)
	) const noexcept {
	
		auto cpi=loc->GetInfo(locale);
		if (cpi==nullptr) return Array<CodePoint>(loc,1);
		
		if (Full) {
		
			auto mapping=get_best(loc,begin,end,cpi->*full);
			if (mapping) return *mapping;
		
		}
		
		auto & mapping=cpi->*simple;
		return mapping ? to_array(&(*mapping),1) : Array<CodePoint>(loc,1);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::impl (
		const CodePoint * begin,
		const CodePoint * end,
		std::optional<CodePoint::Type> (CodePointInfo::* simple),
		Array<CaseMapping> (CodePointInfo::* full)
	) const {
	
		std::vector<CodePoint> retr;
		for (auto loc=begin;loc!=end;++loc) {
		
			for (auto cp : get(loc,begin,end,simple,full)) retr.push_back(cp);
		
		}
		
		return retr;
	
	}
	
	
	bool CaseConverter::folding_normalization_check (CodePoint cp) const noexcept {
	
		if (cp==0x345) return true;
	
		auto cpi=locale.GetInfo(cp);
		if (cpi==nullptr) return false;
		
		for (auto cp : cpi->DecompositionMapping) if (folding_normalization_check(cp)) return true;
		
		return false;
	
	}
	
	
	bool CaseConverter::UppercasingRequiresNormalization (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return std::find_if(begin,end,[&] (CodePoint cp) noexcept { return cp==0x345;	})!=end;
	
	}
	
	
	bool CaseConverter::CaseFoldingRequiresNormalization (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return std::find_if(begin,end,[&] (CodePoint cp) noexcept {	return folding_normalization_check(cp);	})!=end;
	
	}
	
	
	Array<CodePoint> CaseConverter::ToLower (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return get(
			loc,
			begin,
			end,
			&CodePointInfo::SimpleLowercaseMapping,
			&CodePointInfo::LowercaseMappings
		);
		
	}
	
	
	Array<CodePoint> CaseConverter::ToTitle (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return get(
			loc,
			begin,
			end,
			&CodePointInfo::SimpleTitlecaseMapping,
			&CodePointInfo::TitlecaseMappings
		);
		
	}
	
	
	Array<CodePoint> CaseConverter::ToUpper (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return get(
			loc,
			begin,
			end,
			&CodePointInfo::SimpleUppercaseMapping,
			&CodePointInfo::UppercaseMappings
		);
		
	}
	
	
	Array<CodePoint> CaseConverter::Fold (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept {
	
		return get(
			loc,
			begin,
			end,
			&CodePointInfo::SimpleCaseFolding,
			&CodePointInfo::CaseFoldings
		);
		
	}
	
	
	std::vector<CodePoint> CaseConverter::ToLower (const CodePoint * begin, const CodePoint * end) const {
	
		return impl(
			begin,
			end,
			&CodePointInfo::SimpleLowercaseMapping,
			&CodePointInfo::LowercaseMappings
		);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::ToUpper (const CodePoint * begin, const CodePoint * end) const {
	
		//	In certain cases normalization is required before
		//	uppercasing, check if this is one of those cases
		std::optional<std::vector<CodePoint>> normalized;
		if (UppercasingRequiresNormalization(begin,end)) {
		
			Normalizer n(locale);
			normalized=n.ToNFC(begin,end);
			auto & vec=*normalized;
			begin=Begin(vec);
			end=End(vec);
		
		}
	
		return impl(
			begin,
			end,
			&CodePointInfo::SimpleUppercaseMapping,
			&CodePointInfo::UppercaseMappings
		);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::Fold (const CodePoint * begin, const CodePoint * end) const {
	
		//	In certain cases normalization is required
		//	before case folding, check if this is one
		//	of those cases
		std::optional<std::vector<CodePoint>> normalized;
		if (CaseFoldingRequiresNormalization(begin,end)) {
			
			Normalizer n(locale);
			normalized=n.ToNFD(begin,end);
			auto & vec=*normalized;
			begin=Begin(vec);
			end=End(vec);
		
		}
		
		return impl(
			begin,
			end,
			&CodePointInfo::SimpleCaseFolding,
			&CodePointInfo::CaseFoldings
		);
	
	}
	

}
