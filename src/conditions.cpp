#include <unicode/conditions.hpp>
#include <unicode/locale.hpp>
#include <unicode/makereverseiterator.hpp>
#include <iterator>
#include <utility>


namespace Unicode {


	namespace Conditions {
	
		
		template <typename T>
		bool final_sigma (T begin, T end, const Locale & locale) noexcept {
		
			for (;begin!=end;++begin) {
			
				auto cpi=locale.GetInfo(*begin);
				if (cpi==nullptr) return false;
				
				if (cpi->Cased) return true;
				
				if (!cpi->CaseIgnorable) return false;
			
			}
			
			return false;
		
		}
		
		
		//	For the purposes of all comments in this
		//	file, as in the 3.13 of The Unicode Standard,
		//	C shall be the code point which is pointed
		//	to be the loc pointer -- i.e. the code point
		//	whose context is being processed.


		//	From Table 3-14 of The Unicode Standard:
		//
		//	C is preceded by a sequence consisting of
		//	a cased letter and then zero or more
		//	case-ignorable characters, and C is
		//	not followed by a sequence consisting
		//	of zero or more case-ignorable characters
		//	and then a cased letter.
		bool FinalSigma (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
		
			return final_sigma(
				MakeReverseIterator(loc),
				MakeReverseIterator(begin),
				locale
			) && !final_sigma(
				loc+1,
				end,
				locale
			);
		
		}
		
		
		//	From Table 3-14 of The Unicode Standard:
		//
		//	There is a Soft_Dotted character before C,
		//	with no intervening character of combining
		//	class 0 or 230 (Above).
		bool AfterSoftDotted (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
		
			for (
				auto rbegin=MakeReverseIterator(loc),rend=MakeReverseIterator(begin);
				rbegin!=rend;
				++rbegin
			) {
			
				auto cpi=locale.GetInfo(*rbegin);
				if (cpi==nullptr) return false;
				
				if (cpi->SoftDotted) return true;
				
				switch (cpi->CanonicalCombiningClass) {
				
					case 0:
					case 230:
						return false;
					default:
						break;
				
				}
			
			}
			
			return false;
		
		}
		
		
		//	From Table 3-14 of The Unicode Standard:
		//
		//	C is followed by a character of combining
		//	class 230 (Above) with no intervening character
		//	of combining class 0 or 230 (Above).
		bool MoreAbove (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
			
			for (++loc;loc!=end;++loc) {
			
				auto cpi=locale.GetInfo(*loc);
				if (cpi==nullptr) return false;
				
				switch (cpi->CanonicalCombiningClass) {
				
					case 0:
						return false;
					case 230:
						return true;
					default:
						break;
				
				}
			
			}
			
			return false;
		
		}
		
		
		//	From Table 3-14 of The Unicode Standard:
		//
		//	C is followed by COMBINING DOT ABOVE (U+0307)
		//	with no intervening character of combining class
		//	0 or 230 (Above).
		bool BeforeDot (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
			
			for (++loc;loc!=end;++loc) {
			
				if (*loc==0x307) return true;
				
				auto cpi=locale.GetInfo(*loc);
				if (cpi==nullptr) return false;
				
				switch (cpi->CanonicalCombiningClass) {
				
					case 0:
					case 230:
						return false;
					default:
						break;
				
				}
			
			}
			
			return false;
		
		}
		
		
		//	From Table 3-14 of The Unicode Standard:
		//
		//	There is an uppercase I before C, and there
		//	is no intervening combining character class
		//	230 (Above) or 0.
		bool AfterI (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) noexcept {
		
			for (
				auto rbegin=MakeReverseIterator(loc),rend=MakeReverseIterator(begin);
				rbegin!=rend;
				++rbegin
			) {
			
				if (*rbegin=='I') return true;
				
				auto cpi=locale.GetInfo(*rbegin);
				if (cpi==nullptr) return false;
				
				switch (cpi->CanonicalCombiningClass) {
				
					case 0:
					case 230:
						return false;
					default:
						break;
				
				}
			
			}
			
			return false;
		
		}
		
		
	}


}
