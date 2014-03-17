/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/error.hpp>
#include <unicode/locale.hpp>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
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
	
	
	template <typename, typename=std::true_type>
	class ConverterImpl;
	
	
	/**
	 *	Converts between Unicode strings and integers
	 *	of arbitrary type.
	 *
	 *	\tparam T
	 *		The type of integer to convert to/from.
	 */
	template <typename T>
	class ConverterImpl<T,typename std::is_integral<T>::type> {
	
	
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
			 *	Creates a new ConverterImpl.
			 *
			 *	\param [in] locale
			 *		The locale that this converter implementation
			 *		will use.  Defaults to the current locale.
			 */
			ConverterImpl (const Locale & locale=Locale::Get()) noexcept : locale(locale) {	}
		
		
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
	
	
	/**
	 *	The various output formats for floating point
	 *	values.
	 */
	enum class FloatingPointFormat {
	
		Fixed,
		Scientific,
		Default
	
	};
	
	
	/**
	 *	\cond
	 */
	
	
	[[noreturn]]
	inline void NotFloat () {
	
		throw ConversionError("String did not contain a valid representation of a floating point value");
	
	}
	
	
	[[noreturn]]
	inline void FloatOutOfRange () {
	
		throw std::overflow_error("Floating point value out of range");
	
	}
	
	
	template <typename T>
	T FloatParser (const char *) {
	
		throw std::logic_error("No C standard library function to parse this floating point type");
	
	}
	
	
	template <>
	inline float FloatParser<float> (const char * str) {
	
		char * last;
		auto retr=std::strtof(str,&last);
		
		if (last==str) NotFloat();
		
		if (retr==HUGE_VALF) FloatOutOfRange();
		
		return retr;
	
	}
	
	
	template <>
	inline double FloatParser<double> (const char * str) {
	
		char * last;
		auto retr=std::strtod(str,&last);
		
		if (last==str) NotFloat();
		
		if (retr==HUGE_VAL) FloatOutOfRange();
		
		return retr;
	
	}
	
	
	template <>
	inline long double FloatParser<long double> (const char * str) {
	
		char * last;
		auto retr=std::strtold(str,&last);
		
		if (last==str) NotFloat();
		
		if (retr==HUGE_VALL) FloatOutOfRange();
		
		return retr;
	
	}
	
	
	/**
	 *	\endcond
	 */
	
	
	/**
	 *	Converts between Unicode strings and floating point
	 *	values of arbitrary type.
	 */
	template <typename T>
	class ConverterImpl<T,typename std::is_floating_point<T>::type> {
	
	
		public:
		
		
			/**
			 *	The precision of the output.
			 *
			 *	If disengaged, the default precision is
			 *	used.
			 */
			std::optional<std::size_t> Precision;
			/**
			 *	The format of the output.
			 *
			 *	Defaults to FloatingPointFormat::Default.
			 */
			FloatingPointFormat Format;
			/**
			 *	If \em true, a plus sign shall be shown for
			 *	positive values.
			 *
			 *	Defaults to \em false.
			 */
			bool ShowPositive;
	
	
		private:
		
		
			template <typename CharT, typename Traits>
			void setup (std::basic_ostringstream<CharT,Traits> & ss) const {
				
				if (ShowPositive) ss << std::showpos;
				if (Precision) ss << std::setprecision(static_cast<int>(*Precision));
				switch (Format) {
				
					case FloatingPointFormat::Fixed:
						ss << std::fixed;
						break;
					case FloatingPointFormat::Scientific:
						ss << std::scientific;
						break;
					case FloatingPointFormat::Default:
					default:
						ss.unsetf(std::ios_base::floatfield);
						break;
				
				}
			
			}
	
	
		public:
		
		
			/**
			 *	Creates a new ConverterImpl.
			 *
			 *	\param [in] locale
			 *		The locale that this converter implementation
			 *		will use.  Defaults to the current locale.
			 */
			ConverterImpl (const Locale & locale=Locale::Get()) noexcept
				:	Format(FloatingPointFormat::Default),
					ShowPositive(false)
			{	}
			
			
			/**
			 *	Converts a floating point value to a string of Unicode
			 *	code points.
			 *
			 *	\param [in] f
			 *		The floating point value.
			 *
			 *	\return
			 *		A vector containing Unicode code points which represent
			 *		\em f.
			 */
			std::vector<CodePoint> operator () (T f) const {
			
				std::ostringstream ss;
				setup(ss);
				
				ss << f;
				
				auto str=ss.str();
				
				return std::vector<CodePoint>(str.begin(),str.end());
			
			}
			
			
			/**
			 *	Converts a string of Unicode code points to a floating
			 *	point value.
			 *
			 *	Throws if the converson could not be made.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string of Unicode
			 *		characters.
			 *	\param [in] end
			 *		An iterator to the end of the string of Unicode characters.
			 *
			 *	\return
			 *		The floating point value represented by the string
			 *		bounded by \em begin and \em end.
			 */
			T operator () (const CodePoint * begin, const CodePoint * end) const {
			
				//	TODO: More advanced handling (convert numerics to ASCII,
				//	for example)
				std::vector<char> str;
				for (;begin!=end;++begin) {
				
					if (*begin>std::numeric_limits<char>::max()) throw std::out_of_range(
						"Code point out of range"
					);
					
					str.push_back(static_cast<char>(*begin));
				
				}
				
				//	Null terminate
				str.push_back(0);
				
				return FloatParser<T>(str.data());
			
			}
	
	
	};
	
	
	/**
	 *	Converts between Unicode strings and objects of
	 *	type \em T.
	 *
	 *	\tparam T
	 *		The type of object to convert to and from
	 *		Unicode strings.
	 */
	template <typename T>
	class Converter : public ConverterImpl<T> {
	
	
		public:
		
		
			using ConverterImpl<T>::ConverterImpl;
	
	
	};


}
