/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/regexoptions.hpp>
#include <unicode/string.hpp>
#include <memory>
#include <vector>


namespace Unicode {


	/**
	 *	The base type from which all pattern elements derive.
	 *
	 *	A pattern element is a single element in a compiled
	 *	regular expression.
	 */
	class RegexPatternElement {
	
	
		public:
		
		
			/**
			 *	Polymorphically cleans up all resources held
			 *	by this RegexPatternElement.
			 */
			virtual ~RegexPatternElement () noexcept;
	
	
	};
	
	
	/**
	 *	A compiled regular expression.
	 */
	class Regex {
	
	
		private:
		
		
			std::vector<std::unique_ptr<RegexPatternElement>> pattern;
			
			
		public:
		
		
			Regex () = delete;
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] pattern
			 *		A string giving the regular expression pattern
			 *		to compile.
			 *	\param [in] options
			 *		The options with which to compile \em pattern.
			 *		Defaults to RegexOptions::None.
			 *	\param [in] locale
			 *		The locale with which to compile \em pattern.
			 *		Defaults to the current locale.
			 */
			Regex (const String & pattern, RegexOptions options=RegexOptions::None, const Locale & locale=Locale::Get());
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the pattern.
			 *	\param [in] end
			 *		An iterator to the end of the pattern.
			 *	\param [in] options
			 *		The options with which to compile the pattern.
			 *		Defaults to RegexOptions::None.
			 *	\param [in] locale
			 *		The locale with which to compile the pattern.
			 *		Defaults to the current locale.
			 */
			Regex (const CodePoint * begin, const CodePoint * end, RegexOptions options=RegexOptions::None, const Locale & locale=Locale::Get());
	
	
	};


}
