/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/error.hpp>
#include <unicode/locale.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>
#include <type_traits>


namespace Unicode {
	
	
	/**
	 *	Thrown when an error is encountered converting
	 *	to or from a Unicode string.
	 */
	class ConversionError : public Error {
	
	
		public:
		
		
			using Error::Error;
	
	
	};
	
	
	template <typename T, typename=std::true_type>
	class Converter;
	
	
	/**
	 *	Converts between Unicode strings and integers
	 *	of arbitrary type.
	 *
	 *	\tparam T
	 *		The type of integer to convert to/from.
	 */
	template <typename T>
	class Converter<T,typename std::is_integral<T>::type> {
	
	
		private:
		
		
			const Locale & locale;
		
		
			void add (std::vector<CodePoint> & vec, std::size_t digit) const {
			
				//	TODO: Accommodate variable endianness of output
			
				vec.insert(
					vec.begin(),
					locale.Digits[digit]
				);
			
			}
			
			
			static std::size_t mod (T a, T b) noexcept {
			
				auto result=a%b;
				
				if (result<0) result*=-1;
				
				return static_cast<std::size_t>(result);
			
			}
	
	
			void skip_whitespace (const CodePoint * & begin, const CodePoint * end) const {
			
				for (;(begin!=end) && begin->IsWhiteSpace();++begin);
			
			}
	
	
			bool negative (const CodePoint * & begin, const CodePoint * end) const {
			
				skip_whitespace(begin,end);
				
				if ((begin!=end) && (*begin==locale.Negative)) {
				
					++begin;
					
					return true;
				
				}
				
				return false;
			
			}
			
			
			[[noreturn]]
			static void invalid_digit () {
			
				throw ConversionError("Invalid digit");
			
			}
			
			
			T get_value (CodePoint cp) const {
			
				auto cpi=cp.GetInfo(locale);
				
				if (
					(cpi==nullptr) ||
					(!cpi->Numeric) ||
					(cpi->Numeric->Type!=NumericType::Decimal)
				) invalid_digit();
				
				return static_cast<T>(cpi->Numeric->Value);
			
			}
			
			
			T get_base (std::optional<std::size_t> base) const {
			
				if (!base) base=locale.Base;
				
				auto & b=*base;
					
				if (
					(b==0) ||
					(b>std::numeric_limits<T>::max()) ||
					(b>locale.Digits.Size)
				) throw ConversionError("Base out of range");
				
				return static_cast<T>(b);
			
			}
			
			
			//	Assumption:
			//
			//	-	i is the integer being built (and may
			//		be negative)
			//	-	b is the base (and is thus always
			//		positive)
			static T shift (T i, T b) {
			
				if (i<0) {
				
					if ((std::numeric_limits<T>::min()/b)>i) throw std::underflow_error(
						"Multiply underflowed"
					);
				
				} else if ((std::numeric_limits<T>::max()/b)<i) throw std::overflow_error(
					"Multiply overflowed"
				);
				
				return i*b;
			
			}
			
			
			//	Assumption:
			//
			//	Both operands always have the same sign
			static T add (T a, T b) {
			
				if (a<0) {
				
					if ((std::numeric_limits<T>::min()-a)>b) throw std::underflow_error(
						"Addition underflowed"
					);
				
				} else if ((std::numeric_limits<T>::max()-a)<b) throw std::overflow_error(
					"Addition overflowed"
				);
				
				return a+b;
			
			}
	
		
		public:
		
		
			/**
			 *	Creates a new Converter.
			 *
			 *	\param [in] locale
			 *		The locale that this converter will use.
			 *		Defaults to the current locale.
			 */
			Converter (const Locale & locale=Locale::Get()) noexcept : locale(locale) {	}
		
		
			/**
			 *	Converts an integer to a string of Unicode
			 *	code points.
			 *
			 *	\param [in] i
			 *		The integer to convert.
			 *	\param [in] base
			 *		The base.  Defaults to disengaged.  If
			 *		disengaged the default base for this
			 *		locale will be used.
			 *
			 *	\return
			 *		A vector of Unicode code points containing
			 *		the resulting string.
			 */
			std::vector<CodePoint> operator () (T i, std::optional<std::size_t> base=std::nullopt) const {
			
				//	Get the base
				auto b=get_base(base);
				
				std::vector<CodePoint> retr;
				
				//	Handle the case where the integer to
				//	be converted is exactly zero
				//
				//	If this was handled by the normal algorithm,
				//	it would result in an empty string, clearly
				//	not desirable
				if (i==0) {
				
					add(retr,0);
					
					return retr;
				
				}
				
				//	Detect negative integers
				bool negative=i<0;
				
				//	Extract all digits
				do {
				
					add(
						retr,
						mod(i,b)
					);
					i/=b;
				
				} while (i!=0);
				
				//	Add negative sign if necessary
				if (negative) retr.insert(retr.begin(),locale.Negative);
				
				return retr;
			
			}
		
		
			/**
			 *	Attempts to extract an integer from a string of
			 *	Unicode code points.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a string of
			 *		Unicode code points.
			 *	\param [in] end
			 *		An iterator to the end of a string of Unicode
			 *		code points.
			 *	\param [in] base
			 *		The base.  Defaults to disengaged.  If
			 *		disengaged the default base for this locale
			 *		will be used.
			 *
			 *	\return
			 *		The integer contained in the string bounded by
			 *		\em begin and \em end.
			 */
			T operator () (const CodePoint * begin, const CodePoint * end, std::optional<std::size_t> base=std::nullopt) const {
			
				//	Get the base
				auto b=get_base(base);
			
				//	Determine whether or not the
				//	integer being parsed is negative
				auto negative=this->negative(begin,end);
				
				bool found=false;
				T retr=0;
				for (;;++begin) {
				
					skip_whitespace(begin,end);
					
					if (begin==end) break;
					
					auto digit=get_value(*begin);
					if (digit>=b) invalid_digit();
					if (negative) digit*=-1;
					retr=add(shift(retr,b),digit);
					
					found=true;
				
				}
				
				if (!found) throw ConversionError("No digits");
				
				return retr;
			
			}
		
	
	};


}
