#include <unicode/boundaries.hpp>
#include <unicode/makereverseiterator.hpp>
#include <cstddef>
#include <optional>


namespace Unicode {


	typedef GraphemeClusterBreak GCB;


	static GCB get_gcb (CodePoint cp, const Locale & locale) noexcept {
	
		auto cpi=cp.GetInfo(locale);
		return (cpi==nullptr) ? GCB::XX : cpi->GraphemeClusterBreak;
	
	}
	
	
	static bool is_control (GCB gcb) noexcept {
	
		switch (gcb) {
		
			case GCB::CR:
			case GCB::LF:
			case GCB::Control:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	static bool is_hangul_syllable_sequence (GCB a, GCB b) noexcept {
	
		switch (a) {
		
			case GCB::L:
				switch (b) {
					
					case GCB::L:
					case GCB::V:
					case GCB::LV:
					case GCB::LVT:
						return true;
					default:
						return false;
					
				}
			case GCB::LV:
			case GCB::V:
				switch (b) {
				
					case GCB::V:
					case GCB::T:
						return true;
					default:
						return false;
				
				}
			case GCB::LVT:
			case GCB::T:
				return b==GCB::T;
			default:
				return false;
		
		}
	
	}


	bool IsGraphemeClusterBoundary (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
	
		//	Break at the start and end of text
		if (
			(loc==begin) ||
			(loc==end)
		) return true;
		
		auto a=get_gcb(*(loc-1),locale);
		auto b=get_gcb(*loc,locale);
		
		//	Do not break between a CR and LF
		if ((a==GCB::CR) && (b==GCB::LF)) return false;
		
		//	Break before and after controls
		if (is_control(a) || is_control(b)) return true;
		
		//	Do not break Hangul syllable sequences
		if (is_hangul_syllable_sequence(a,b)) return false;
		
		//	Do not break between regional indicator symbols
		if ((a==GCB::RegionalIndicator) && (b==GCB::RegionalIndicator)) return false;
		
		//	Do not break before extending characters
		if (b==GCB::Extend) return false;
		
		//	Do not break before SpacingMarks, or after Prepend characters
		if ((b==GCB::SpacingMark) || (a==GCB::Prepend)) return false;
		
		//	Otherwise, break everywhere
		return true;
	
	}
	
	
	typedef WordBreak WB;
	
	
	static WB get_wb (CodePoint cp, const Locale & locale) noexcept {
	
		auto cpi=cp.GetInfo(locale);
		return (cpi==nullptr) ? WB::XX : cpi->WordBreak;
	
	}
	
	
	static bool is_ignored_wb (WB wb) noexcept {
	
		switch (wb) {
		
			case WB::Extend:
			case WB::Format:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	template <typename T>
	std::optional<WB> get_wb (T begin, T end, const Locale & locale, std::size_t skip=0) noexcept {
	
		std::size_t skipped=0;
		
		for (;begin!=end;++begin) {
		
			auto wb=get_wb(*begin,locale);
			if (!is_ignored_wb(wb)) {
			
				if (skipped==skip) return wb;
				
				++skipped;
			
			}
		
		}
		
		return std::optional<WB>{};
	
	}
	
	
	static bool is_newline (WB wb) noexcept {
	
		switch (wb) {
		
			case WB::Newline:
			case WB::CR:
			case WB::LF:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	static bool is_most_letters (WB wb) noexcept {
	
		switch (wb) {
		
			case WB::ALetter:
			case WB::HebrewLetter:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	static bool is_certain_punctuation (WB wb) noexcept {
	
		switch (wb) {
		
			case WB::MidLetter:
			case WB::MidNumLet:
			case WB::SingleQuote:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	static bool wb_6_and_7 (WB a, WB b, WB c) noexcept {
	
		return is_most_letters(a) && is_certain_punctuation(b) && is_most_letters(c);
	
	}
	
	
	static bool wb_7_b_and_c (WB a, WB b, WB c) noexcept {
	
		return (
			(a==WB::HebrewLetter) &&
			(b==WB::DoubleQuote) &&
			(c==WB::HebrewLetter)
		);
	
	}
	
	
	static bool wb_11_and_12 (WB a, WB b, WB c) noexcept {
	
		return (a==WB::Numeric) && is_certain_punctuation(b) && (c==WB::Numeric);
	
	}
	
	
	static bool wb_13_a_and_b (WB a, WB b) noexcept {
	
		if (b!=WB::ExtendNumLet) return false;
		
		switch (a) {
		
			case WB::ALetter:
			case WB::HebrewLetter:
			case WB::Numeric:
			case WB::Katakana:
			case WB::ExtendNumLet:
				return true;
			default:
				return false;
		
		}
	
	}
	
	
	bool IsWordBoundary (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
	
		//	Break at the start and end of text
		if (
			(loc==begin) ||
			(loc==end)
		) return true;
		
		auto a=get_wb(*(loc-1),locale);
		auto b=get_wb(*loc,locale);
		
		//	Do not break within CRLF
		if ((a==WB::CR) && (b==WB::LF)) return false;
		
		//	Otherwise break before and after Newlines
		//	(including CR and LF)
		if (is_newline(a) || is_newline(b)) return true;
		
		//	Ignore Format and Extend characters,
		//	except when they appear at the beginning of
		//	a region of text
		if (is_ignored_wb(b)) return false;
		
		if (is_ignored_wb(a)) {
		
			auto a_n=get_wb(
				MakeReverseIterator(loc),
				MakeReverseIterator(begin),
				locale
			);
			
			if (!a_n) return false;
			
			a=*a_n;
		
		}
		
		//	Do not break between most letters
		if (is_most_letters(a) && is_most_letters(b)) return false;
		
		//	Do not break letters across certain punctuation
		
		//	WB7a
		if ((a==WB::HebrewLetter) && (b==WB::SingleQuote)) return false;
		
		//	WB6 and 7b
		auto c=get_wb(
			loc,
			end,
			locale,
			1
		);
		if (
			c &&
			(
				wb_6_and_7(a,b,*c) ||
				wb_7_b_and_c(a,b,*c)
			)
		) return false;
		
		//	WB7 and 7c
		auto d=get_wb(
			MakeReverseIterator(loc),
			MakeReverseIterator(begin),
			locale,
			1
		);
		if (
			d &&
			(
				wb_6_and_7(*d,a,b) ||
				wb_7_b_and_c(*d,a,b)
			)
		) return false;
		
		//	Do not break within sequences of digits, or digits
		//	adjacent to letters ("3a", or "A3")
		if (b==WB::Numeric) {
		
			if (
				(a==WB::Numeric) ||
				is_most_letters(a)
			) return false;
			
		} else if (
			(b==WB::Numeric) ||
			is_most_letters(b)
		) {
		
			return false;
			
		}
		
		//	Do not break within sequences, such as
		//	"3.2" or "3,456.789"
		if (c && wb_11_and_12(a,b,*c)) return false;
		if (d && wb_11_and_12(*d,a,b)) return false;
		
		//	Do not break Katakana
		if ((a==WB::Katakana) && (b==WB::Katakana)) return false;
		
		//	Do not break from extenders
		if (wb_13_a_and_b(a,b) || wb_13_a_and_b(b,a)) return false;
		
		//	Do not break between regional indicator symbols
		if ((a==WB::RegionalIndicator) && (b==WB::RegionalIndicator)) return false;
		
		//	Otherwise, break everywhere (including around ideographs)
		return true;
	
	}


}
