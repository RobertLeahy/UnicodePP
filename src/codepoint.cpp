#include <unicode/codepoint.hpp>
#include <algorithm>
#include <iterator>
#include "data.cpp"


namespace Unicode {

	
	bool CodePoint::IsValid () const noexcept {
	
		return !(
			//	Too large to be valid Unicode
			(cp>Max) ||
			//	Reserved for UTF-16 lead/trail
			//	surrogates
			(
				(cp>=0xD800U) &&
				(cp<=0xDFFFU)
			)
		);
	
	}
	
	
	const CodePointInfo * CodePoint::GetInfo () const noexcept {
	
		auto last=std::end(map);
		auto iter=std::lower_bound(
			std::begin(map),
			last,
			cp,
			[] (const CodePointInfo & a, Type b) noexcept {	return a.CP<b;	}
		);
		
		return (iter==last) ? nullptr : iter;
	
	}
	
	
	bool CodePoint::IsWhitespace () const noexcept {
	
		auto info=GetInfo();
		if (info==nullptr) return false;
		
		switch (info->Category) {
		
			default:
				return false;
			
			case GeneralCategory::Zs:
			case GeneralCategory::Zl:
			case GeneralCategory::Zp:
				return true;
		
		}
	
	}


}
