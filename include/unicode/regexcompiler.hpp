/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/regexerror.hpp>
#include <unicode/regexoptions.hpp>
#include <cstddef>
#include <iterator>
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
			 *	The current location within the pattern.
			 */
			const CodePoint * Current;
			/**
			 *	The beginning of the pattern
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
			 *	If \em true, the regular expression compiler is compiling
			 *	a character class.
			 *
			 *	Defaults to \em false.
			 */
			bool CharacterClass;
			
			
			/**
			 *	Creates a new RegexCompilerState.
			 *
			 *	\param [in] loc
			 *		An iterator to the current location within the
			 *		pattern.
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
				const CodePoint * loc,
				const CodePoint * begin,
				const CodePoint * end,
				RegexOptions options,
				const Unicode::Locale & locale
			) noexcept
				:	Current(loc),
					Begin(begin),
					End(end),
					Options(options),
					Locale(locale),
					Fail(true),
					CharacterClass(false)
			{	}
			
			
			/**
			 *	Creates a new RegexCompilerState whose location is the
			 *	beginning of the pattern.
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
			) noexcept : RegexCompilerState(begin,begin,end,options,locale) {	}
			
			
			/**
			 *	Polymorphically releases all resources held by this
			 *	object.
			 */
			virtual ~RegexCompilerState () noexcept;
			
			
			/**
			 *	Checks to see if the location within the pattern is
			 *	valid.
			 *
			 *	\return
			 *		\em true if Current!=End, \em false if Current==End.
			 */
			explicit operator bool () const noexcept {
			
				return Current!=End;
			
			}
			/**
			 *	Retrieves a reference to the code point at the current
			 *	location within the pattern.
			 *
			 *	\return
			 *		A reference to a code point.
			 */
			const CodePoint & operator * () const noexcept {
			
				return *Current;
			
			}
			/**
			 *	Retrieves a pointer to the code point at the current
			 *	location within the pattern.
			 *
			 *	\return
			 *		A pointer to a code point.
			 */
			const CodePoint * operator -> () const noexcept {
			
				return Current;
			
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
			
				Current+=dist;
				
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
			
				Current-=dist;
				
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
			 *		The result of End-Current.
			 */
			std::size_t Remaining () const noexcept {
			
				return static_cast<std::size_t>(End-Current);
			
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
	
	
			/**
			 *	Throws an exception at the current location within the
			 *	pattern.
			 *
			 *	\param [in] what
			 *		A message describing the error.
			 */
			[[noreturn]]
			void Raise (const char * what) const {
			
				throw RegexError(what,Current);
			
			}
			
			
			/**
			 *	Determines whether or not compilation is finished at
			 *	the current point in the pattern.
			 *
			 *	Default implementation returns \em false until the
			 *	end of the pattern.
			 *
			 *	If this method returns \em false at the end of the
			 *	pattern, the result is undefined behaviour.
			 *
			 *	\return
			 *		\em true if compilation is finished at the current
			 *		point within the pattern, \em false otherwise.
			 */
			virtual bool Done ();
	
	
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
			 */
			static void Add (const RegexParser & parser);
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
			static void Add (const RegexParser & parser, std::size_t priority);
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	that will be used to parse regular expressions.
			 *
			 *	Not thread safe.
			 *
			 *	\param [in] parser
			 *		The parser to add.
			 *	\param [in] character_class
			 *		\em true if this parser should be invoked only
			 *		within character classes, \em false if this
			 *		parser should only be invoked outside of character
			 *		classes.
			 */
			static void Add (const RegexParser & parser, bool character_class);
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
			 *	\param [in] character_class
			 *		\em true if this parser should be invoked only
			 *		within character classes, \em false if this
			 *		parser should only be invoked outside of character
			 *		classes.
			 */
			static void Add (const RegexParser & parser, std::size_t priority, bool character_class);
			
			
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] state
			 *		The compiler's internal state.
			 *
			 *	\return
			 *		A compiled regular expression pattern.
			 */
			Pattern operator () (RegexCompilerState && state) const;
			
			
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] state
			 *		The compiler's internal state.
			 *
			 *	\return
			 *		A compiled regular expression pattern.
			 */
			Pattern operator () (RegexCompilerState & state) const;
	
	
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
			 *	Installs a parser of type \em T.
			 *
			 *	\param [in] priority
			 *		The priority.
			 */
			RegexParserInstaller (std::size_t priority) {
			
				RegexCompiler::Add(parser,priority);
			
			}
			/**
			 *	Installs a parser of type \em T.
			 *
			 *	\param [in] character_class
			 *		\em true if the parser should only be invoked
			 *		within character classes, \em false if it should
			 *		only be invoked outside of character classes.
			 */
			RegexParserInstaller (bool character_class) {
			
				RegexCompiler::Add(parser,character_class);
			
			}
			/**
			 *	Installs a parser of type \em T.
			 *
			 *	\param [in] priority
			 *		The priority.
			 *	\param [in] character_class
			 *		\em true if the parser should only be invoked
			 *		within character classes, \em false if it should
			 *		only be invoked outside of character classes.
			 */
			RegexParserInstaller (std::size_t priority, bool character_class) {
			
				RegexCompiler::Add(parser,priority,character_class);
			
			}
			
			
			RegexParserInstaller (const RegexParserInstaller &) = delete;
			RegexParserInstaller (RegexParserInstaller &&) = delete;
			RegexParserInstaller & operator = (const RegexParserInstaller &) = delete;
			RegexParserInstaller & operator = (RegexParserInstaller &&) = delete;
	
	
	};


}
