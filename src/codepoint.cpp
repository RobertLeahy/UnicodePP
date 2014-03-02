#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <optional>


namespace Unicode {


	const Locale & CodePoint::get_locale () noexcept {
	
		return Locale::Get();
	
	}

	
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
	
	
	const CodePointInfo * CodePoint::GetInfo (const Locale & locale) const noexcept {
	
		return locale.GetInfo(*this);
	
	}
	
	
	bool CodePoint::IsWhiteSpace (const Locale & locale) const noexcept {
	
		auto info=GetInfo(locale);
		if (info==nullptr) return false;
		
		return info->WhiteSpace;
	
	}


}
