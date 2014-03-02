#include <unicode/caseconverter.hpp>
#include <unicode/normalizer.hpp>
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


	const Array<CodePoint::Type> * CaseConverter::get_best (
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
		
		const Array<CodePoint::Type> * best=nullptr;
		std::size_t count=0;
		for (auto & mapping : arr) {
		
			if (
				matches(loc,begin,end,mapping.Conditions) &&
				(
					(best==nullptr) ||
					(mapping.Conditions.size()>count)
				)
			) {
			
				best=&mapping.Mapping;
				count=mapping.Conditions.size();
			
			}
		
		}
		
		return best;
	
	}


	bool CaseConverter::get_full (
		std::vector<CodePoint> & vec,
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		const Array<CaseMapping> & arr
	) const {
	
		auto mapping=get_best(loc,begin,end,arr);
		if (mapping==nullptr) return false;
		
		for (auto cp : *mapping) vec.push_back(cp);
		
		return true;
	
	}


	void CaseConverter::impl (
		std::vector<CodePoint> & vec,
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		Array<CaseMapping> (CodePointInfo::* full),
		std::optional<CodePoint::Type> (CodePointInfo::* simple)
	) const {
	
		//	Look up this code point
		auto cpi=locale.GetInfo(*loc);
		//	If there's no entry, just copy this
		//	code point untouched
		if (cpi==nullptr) {
		
			vec.push_back(*loc);
			
			return;
		
		}
		
		//	If full case mappings/foldings are being
		//	performed, attempt
		if (
			Full &&
			get_full(
				vec,
				loc,
				begin,
				end,
				cpi->*full
			)
		) return;
		
		//	Fall back to simple case mappings/foldings
		
		auto & mapping=cpi->*simple;
		if (mapping) vec.push_back(*mapping);
		else vec.push_back(*loc);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::impl (
		const CodePoint * begin,
		const CodePoint * end,
		Array<CaseMapping> (CodePointInfo::* full),
		std::optional<CodePoint::Type> (CodePointInfo::* simple)
	) const {
	
		std::vector<CodePoint> retr;
		for (auto loc=begin;loc!=end;++loc) impl(
			retr,
			loc,
			begin,
			end,
			full,
			simple
		);
		
		return retr;
	
	}
	
	
	bool CaseConverter::normalization_check (CodePoint cp) const noexcept {
	
		if (cp==0x345) return true;
	
		auto cpi=locale.GetInfo(cp);
		if (cpi==nullptr) return false;
		
		for (auto cp : cpi->DecompositionMapping) if (normalization_check(cp)) return true;
		
		return false;
	
	}
	
	
	bool CaseConverter::requires_normalization (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		for (;begin!=end;++begin) if (normalization_check(*begin)) return true;
		
		return false;
	
	}
	
	
	std::vector<CodePoint> CaseConverter::ToLower (const CodePoint * begin, const CodePoint * end) const {
	
		return impl(
			begin,
			end,
			&CodePointInfo::LowercaseMappings,
			&CodePointInfo::SimpleLowercaseMapping
		);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::ToUpper (const CodePoint * begin, const CodePoint * end) const {
	
		return impl(
			begin,
			end,
			&CodePointInfo::UppercaseMappings,
			&CodePointInfo::SimpleUppercaseMapping
		);
	
	}
	
	
	std::vector<CodePoint> CaseConverter::Fold (const CodePoint * begin, const CodePoint * end) const {
	
		//	In certain cases normalization is required
		//	before case folding, check if this is one
		//	of those cases
		std::optional<std::vector<CodePoint>> normalized;
		if (requires_normalization(begin,end)) {
			
			Normalizer n(locale);
			normalized=n.ToNFD(begin,end);
			auto & vec=*normalized;
			begin=&vec[0];
			end=&vec[0]+vec.size();
		
		}
		
		return impl(
			begin,
			end,
			&CodePointInfo::CaseFoldings,
			&CodePointInfo::SimpleCaseFolding
		);
	
	}
	

}
