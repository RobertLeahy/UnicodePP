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
			 *	Attempts to parse a pattern element at a certain
			 *	point in a string.
			 *
			 *	\param [in,out] begin
			 *		An iterator to the current location in the
			 *		string.  The parser should advance this iterator
			 *		as it consumes code points.  If the parser fails
			 *		to do so, it will be automatically advanced one
			 *		position.  Guaranteed not to be equal to \em end.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *	\param [in] options
			 *		The RegexOptions currently in effect.
			 *	\param [in] locale
			 *		The current locale.
			 *
			 *	\return
			 *		A std::unique_ptr to a RegexPatternElement if
			 *		parsing was successful, a null std::unique_ptr
			 *		otherwise.
			 */
			virtual RegexCompiler::Element operator () (
				const CodePoint * & begin,
				const CodePoint * end,
				RegexOptions options,
				const Locale & locale
			) const = 0;
			
			
			/**
			 *	Attempts to continue parsing a pattern element at a
			 *	certain point in a string.
			 *
			 *	This method will be invoked only when this parser is
			 *	being invoked at a point in the string immediately
			 *	following a point at which it successfully parsed a
			 *	pattern element.
			 *
			 *	Default implementation unconditionally returns
			 *	\em false.
			 *
			 *	\param [in,out] element
			 *		A reference to the pattern element this parser
			 *		previously produced.
			 *	\param [in,out] begin
			 *		An iterator to the current location in the
			 *		string.  The parser should advance this iterator
			 *		as it consumes code points.  If the parser fails
			 *		to do so, it will be automatically advanced one
			 *		position.  Guaranteed not to be equal to \em end.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		\em true if parsing was successful, \em false
			 *		otherwise.  If \em false is returned the compiler
			 *		will try and obtain a new pattern element by calling
			 *		the other overload.
			 */
			virtual bool operator () (
				RegexPatternElement & element,
				const CodePoint * & begin,
				const CodePoint * end
			) const;
			
			
			/**
			 *	Completes a pattern element.
			 *
			 *	By default is called when a different parser is selected
			 *	to parse at a point immediately after this parser produced
			 *	\em element.
			 *
			 *	Default implementation does nothing.
			 *
			 *	\param [in,out] element
			 *		A reference to the element to complete.
			 */
			virtual void Complete (RegexPatternElement & element) const;
	
	
	};
	
	
	/**
	 *	When constructed, installs a certain type of RegexParser.
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
