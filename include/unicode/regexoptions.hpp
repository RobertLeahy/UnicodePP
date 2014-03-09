/**
 *	\file
 */
 
 
#pragma once


#include <type_traits>


namespace Unicode {


	/**
	 *	Options which may be passed to the regular
	 *	expression engine to control its behaviour.
	 */
	enum class RegexOptions {
	
		/**
		 *	No special options.
		 */
		None=0,
		/**
		 *	Use case-insensitive matching.
		 */
		IgnoreCase=1,
		/**
		 *	^ and $ will match the beginning and end of
		 *	each line rather than the beginning and end
		 *	of the string.
		 */
		Multiline=2,
		/**
		 *	The period matches every character rather than
		 *	every character except LINE FEED (U+000A).
		 */
		Singleline=4,
		/**
		 *	Unnamed groups will not caputer.
		 */
		ExplicitCapture=8,
		/**
		 *	Unescaped white space in the pattern will be ignored,
		 *	and comments may preced from a number sign to the end
		 *	of a line.
		 */
		IgnorePatternWhitespace=16,
		/**
		 *	The search will move from right to left rather than
		 *	vice versa.
		 */
		RightToLeft=32,
		/**
		 *	The default locale will be used rather than the current
		 *	locale when matching.
		 */
		CultureInvariant=64
	
	};
	
	
	inline RegexOptions operator | (RegexOptions a, RegexOptions b) noexcept {
	
		typedef std::underlying_type<RegexOptions>::type type;
	
		return static_cast<RegexOptions>(
			static_cast<type>(a)|static_cast<type>(b)
		);
	
	}
	
	
	inline RegexOptions operator & (RegexOptions a, RegexOptions b) noexcept {
	
		typedef std::underlying_type<RegexOptions>::type type;
		
		return static_cast<RegexOptions>(
			static_cast<type>(a)&static_cast<type>(b)
		);
	
	}
	
	
	inline RegexOptions operator ^ (RegexOptions a, RegexOptions b) noexcept {
	
		typedef std::underlying_type<RegexOptions>::type type;
		
		return static_cast<RegexOptions>(
			static_cast<type>(a)^static_cast<type>(b)
		);
	
	}
	
	
	inline RegexOptions operator ~ (RegexOptions a) noexcept {
	
		return static_cast<RegexOptions>(
			~static_cast<std::underlying_type<RegexOptions>::type>(a)
		);
	
	}
	
	
	inline RegexOptions & operator |= (RegexOptions & a, RegexOptions b) noexcept {
	
		a=a|b;
		
		return a;
	
	}
	
	
	inline RegexOptions & operator &= (RegexOptions & a, RegexOptions b) noexcept {
	
		a=a&b;
		
		return a;
	
	}
	
	
	inline RegexOptions & operator ^= (RegexOptions & a, RegexOptions b) noexcept {
	
		a=a^b;
		
		return a;
	
	}


}
