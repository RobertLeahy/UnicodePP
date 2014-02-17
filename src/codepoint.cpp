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
	
	
	bool CodePoint::IsWhitespace (const Locale & locale) const noexcept {
	
		auto info=GetInfo(locale);
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
	
	
	static std::optional<bool> hangul_is_break_lvt_t (const CodePointInfo * n) noexcept {
	
		if (n->Break==GraphemeBreak::T) return false;
		
		return std::optional<bool>{};
	
	}
	
	
	static std::optional<bool> hangul_is_break_lv_v (const CodePointInfo * n) noexcept {
	
		switch (n->Break) {
		
			case GraphemeBreak::V:
			case GraphemeBreak::T:
				return false;
				
			default:
				break;
		
		}
		
		return std::optional<bool>{};
	
	}
	
	
	static std::optional<bool> hangul_is_break_l (const CodePointInfo * n) noexcept {
	
		switch (n->Break) {
		
			case GraphemeBreak::L:
			case GraphemeBreak::V:
			case GraphemeBreak::LV:
			case GraphemeBreak::LVT:
				return false;
				
			default:
				break;
		
		}
		
		return std::optional<bool>{};
	
	}
	
	
	static std::optional<bool> hangul_is_break (const CodePointInfo * t, const CodePointInfo * n) noexcept {
	
		switch (t->Break) {
		
			case GraphemeBreak::L:
				return hangul_is_break_l(n);
			case GraphemeBreak::LV:
			case GraphemeBreak::V:
				return hangul_is_break_lv_v(n);
			case GraphemeBreak::LVT:
			case GraphemeBreak::T:
				return hangul_is_break_lvt_t(n);
				
			default:
				break;
		
		}
		
		return std::optional<bool>{};
	
	}
	
	
	bool CodePoint::IsBreak (CodePoint next, const Locale & locale) const noexcept {
	
		//	Do not break between a CR and a LF
		if ((*this=='\r') && (next=='\n')) return false;
		
		auto t=GetInfo(locale);
		if (t==nullptr) return true;
		auto n=next.GetInfo(locale);
		if (n==nullptr) return true;
		
		//	Break before and after controls
		if (
			(t->Category==GeneralCategory::Cc) ||
			(n->Category==GeneralCategory::Cc)
		) return true;
		
		//	Do not break Hangul syllable sequences
		auto h=hangul_is_break(t,n);
		if (h) return *h;
		
		//	Do not break between regional indicator
		//	symbols
		if (
			(t->Break==GraphemeBreak::RegionalIndicator) &&
			(n->Break==GraphemeBreak::RegionalIndicator)
		) return false;
		
		//	Do not break before extending characters
		if (n->Break==GraphemeBreak::Extend) return false;
		
		//	Do not break before spacing marks
		if (n->Break==GraphemeBreak::SpacingMark) return false;
		
		//	Do not break after prepend characters
		if (t->Break==GraphemeBreak::Prepend) return false;
		
		//	Otherwise, break everywhere
		return true;
	
	}
	
	
	std::optional<CodePoint> CodePoint::DoesCompose (CodePoint cp, const Locale & locale) const noexcept {
	
		return locale.GetComposition(*this,cp);
	
	}


}
