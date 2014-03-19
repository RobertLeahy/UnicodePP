#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <optional>


namespace Unicode {


	const Locale & CodePoint::get_locale () noexcept {
	
		return Locale::Get();
	
	}

	
	bool CodePoint::IsValid () const noexcept {
	
		//	To large to be valid Unicode
		if (cp>Max) return false;
		
		//	UTF-16 lead/trail surrogate
		if ((cp>=0xD800U) && (cp<=0xDFFFU)) return false;
		
		//	Arbitrary contiguous range of 32 non-character
		//	code points
		if ((cp>=0xFDD0U) && (cp<=0xFDEFU)) return false;
		
		//	Invalid Unicode
		auto low=cp&65535U;
		if ((low==0xFFFEU) || (low==0xFFFFU)) return false;
		
		//	All good
		return true;
	
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
