/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/regexoptions.hpp>
#include <iterator>
#include <limits>
#include <memory>
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */


	class RegexPatternElement;
	class RegexParser;
	
	
	/**
	 *	\endcond
	 */
	
	
	/**
	 *	Compiles regular expressions.
	 */
	class RegexCompiler {
	
	
		public:
		
		
			/**
			 *	The type of each pattern element in a pattern
			 *	formed by the compiler.
			 */
			typedef std::unique_ptr<RegexPatternElement> Element;
			/**
			 *	The type of the pattern formed by the compiler.
			 */
			typedef std::vector<Element> Pattern;
		
		
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	that will be used to parse regular expressions.
			 *
			 *	Not thread safe.
			 *
			 *	\param [in] parser
			 *		The parser to add.
			 *	\param [in] priority
			 *		The relative priority of the parser.  Parsers with
			 *		a lower priority will be called on each position in
			 *		a string earlier than parsers with a higher priority.
			 */
			static void Add (
				const RegexParser & parser,
				std::size_t priority=std::numeric_limits<std::size_t>::max()/2
			);
		
		
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the pattern.
			 *	\param [in] end
			 *		An iterator to the end of the pattern.
			 *	\param [in] options
			 *		The options with which to compile.
			 *	\param [in] locale
			 *		The locale with which to compile.
			 *
			 *	\return
			 *		A compiled regular expression pattern.
			 */
			Pattern operator () (
				const CodePoint * begin,
				const CodePoint * end,
				RegexOptions options,
				const Locale & locale
			) const;
	
	
	};
	
	
	/**
	 *	Parses pattern elements from a regular expression.
	 */
	class RegexParser {
	
	
		public:
		
		
			/**
			 *	Cleans up all resources held by any
			 *	derived class.
			 */
			virtual ~RegexParser () noexcept;
		
		
			/**
			 *	Parses a pattern element or elements from a certain
			 *	location in a regular expression.
			 *
			 *	\param [in,out] pattern
			 *		The pattern.
			 *	\param [in] successive
			 *		\em true if the last successful parse for this
			 *		regular expression was made by this parser,
			 *		\em false otherwise.
			 *	\param [in,out] begin
			 *		A reference to an iterator to the current location
			 *		within the pattern.
			 *	\param [in] end
			 *		An iterator to the end of the pattern.  Guaranteed
			 *		not to be equal to \em begin.
			 *	\param [in] options
			 *		The options that are set at this location within
			 *		the pattern.
			 *	\param [in] locale
			 *		The locale that is set at this location within the
			 *		pattern.
			 *
			 *	\return
			 *		\em true if this element successfully parsed at the
			 *		given location, \em false otherwise.
			 */
			virtual bool operator () (
				RegexCompiler::Pattern & pattern,
				bool successive,
				const CodePoint * & begin,
				const CodePoint * end,
				RegexOptions options,
				const Locale & locale
			) const = 0;
	
	
	};
	
	
	/**
	 *	When constructed as a global object, installs a certain
	 *	type of RegexParser into the RegexCompiler.
	 *
	 *	\tparam T
	 *		The type of RegexParser to install.
	 */
	template <typename T>
	class RegexParserInstaller {
	
	
		private:
		
		
			T parser;
			
			
		public:
		
		
			/**
			 *	Installs a parser of type \em T.
			 */
			RegexParserInstaller () {
			
				RegexCompiler::Add(parser);
			
			}
			/**
			 *	Installs a parser of type \em T with a certain
			 *	priority.
			 *
			 *	\param [in] priority
			 *		The priority.
			 */
			RegexParserInstaller (std::size_t priority) {
			
				RegexCompiler::Add(parser,priority);
			
			}
			
			
			RegexParserInstaller (const RegexParserInstaller &) = delete;
			RegexParserInstaller (RegexParserInstaller &&) = delete;
			RegexParserInstaller & operator = (const RegexParserInstaller &) = delete;
			RegexParserInstaller & operator = (RegexParserInstaller &&) = delete;
	
	
	};


}
