/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/regexoptions.hpp>
#include <cstddef>
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
	 *	Holds the regular expression compiler's internal
	 *	state.
	 */
	class RegexCompilerState {
		
		
		public:
		
		
			/**
			 *	The current location within the regular expression
			 *	pattern.
			 */
			const CodePoint * Begin;
			/**
			 *	The end of the regular expression pattern.
			 */
			const CodePoint * const End;
			/**
			 *	The options currently set.
			 */
			RegexOptions Options;
			/**
			 *	The current locale.
			 */
			const Unicode::Locale & Locale;
			/**
			 *	If \em false, the regular expression compiler will not
			 *	fail even if a call to a parser fails.
			 *
			 *	Defaults to \em true.
			 */
			bool Fail;
			
			
			/**
			 *	Creates a new RegexCompilerState.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the pattern.
			 *	\param [in] end
			 *		An iterator to the end of the pattern.
			 *	\param [in] options
			 *		The options set for the pattern being compiled.
			 *	\param [in] locale
			 *		The locale in which the pattern is being compiled.
			 */
			RegexCompilerState (
				const CodePoint * begin,
				const CodePoint * end,
				RegexOptions options,
				const Unicode::Locale & locale
			) noexcept
				:	Begin(begin),
					End(end),
					Options(options),
					Locale(locale),
					Fail(true)
			{	}
			
			
			/**
			 *	Checks to see if the location within the pattern is
			 *	valid.
			 *
			 *	\return
			 *		\em true if Begin!=End, \em false if Begin==End.
			 */
			explicit operator bool () const noexcept {
			
				return Begin!=End;
			
			}
			/**
			 *	Retrieves a reference to the code point at the current
			 *	location within the pattern.
			 *
			 *	\return
			 *		A reference to a code point.
			 */
			const CodePoint & operator * () const noexcept {
			
				return *Begin;
			
			}
			/**
			 *	Retrieves a pointer to the code point at the current
			 *	location within the pattern.
			 *
			 *	\return
			 *		A pointer to a code point.
			 */
			const CodePoint * operator -> () const noexcept {
			
				return Begin;
			
			}
			/**
			 *	Advances the location within the pattern by a certain
			 *	amount.
			 *
			 *	\param [in] dist
			 *		The number of code points by which the location in
			 *		the pattern should be advanced.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerState & operator += (std::ptrdiff_t dist) noexcept {
			
				Begin+=dist;
				
				return *this;
			
			}
			/**
			 *	Retreats the location within the pattern by a certain
			 *	amount.
			 *
			 *	\param [in] dist
			 *		The number of code points by which the location in
			 *		the pattern should retreat.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerState & operator -= (std::ptrdiff_t dist) noexcept {
			
				Begin-=dist;
				
				return *this;
			
			}
			/**
			 *	Advances the location within the pattern by one code
			 *	point.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerState & operator ++ () noexcept {
			
				return *this+=1;
			
			}
			/**
			 *	Retreats the location within the pattern by one code
			 *	point.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerState & operator -- () noexcept {
			
				return *this-=1;
			
			}
			/**
			 *	Determines how many code points remain in the pattern.
			 *
			 *	\return
			 *		The result of End-Begin.
			 */
			std::size_t Remaining () const noexcept {
			
				return static_cast<std::size_t>(End-Begin);
			
			}
			
			
			/**
			 *	Checks to see if a certain option or options are set.
			 *
			 *	\param [in] option
			 *		The option or options to check.
			 *
			 *	\return
			 *		\em true if all the options specified by \em option
			 *		are set, \em false otherwise.
			 */
			bool Check (RegexOptions option) const noexcept {
			
				return Unicode::Check(Options,option);
			
			}
			
			
			/**
			 *	Resets the flags set or unset on the state object to
			 *	their default state.
			 */
			void Reset () noexcept {
			
				Fail=true;
			
			}
	
	
	};
	
	
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
			 *	\param [in] state
			 *		The regular expression compiler's internal
			 *		state.
			 *
			 *	\return
			 *		A std::unique_ptr to a RegexPatternElement if
			 *		parsing was successful, a null std::unique_ptr
			 *		otherwise.
			 */
			virtual RegexCompiler::Element operator () (RegexCompilerState & state) const = 0;
			
			
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
			 *	\param [in] state
			 *		The regular expression compiler's internal
			 *		state.
			 *
			 *	\return
			 *		\em true if parsing was successful, \em false
			 *		otherwise.  If \em false is returned the compiler
			 *		will try and obtain a new pattern element by calling
			 *		the other overload.
			 */
			virtual bool operator () (RegexPatternElement & element, RegexCompilerState & state) const;
			
			
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
