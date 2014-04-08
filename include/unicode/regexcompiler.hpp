/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
//	This header shouldn't be needed, but GCC's
//	implementation of std::unique_ptr won't compile
//	if it's instantiated with an incomplete type, so
//	I can't just forward declare RegexPatternElement.
//
//	Accordingly, all files which include this file
//	should act as though unicode/regex.hpp is not
//	included automatically, as it may not be when/if
//	GCC's implementation of std::unique_ptr is ever
//	fixed.
#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include <unicode/regexoptions.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */


	class RegexCompilerState;
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
			 *		The compiled pattern.
			 */
			Pattern operator () (const CodePoint * begin, const CodePoint * end, RegexOptions options, const Locale & locale) const;
			
			
			/**
			 *	Compiles a regular expression.
			 *
			 *	\param [in] state
			 *		The compiler's internal state.
			 */
			void operator () (RegexCompilerState & state) const;
	
	
	};
	
	
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
			 *	If \em true, the regular expression compiler is compiling
			 *	a character class.
			 *
			 *	Defaults to \em false.
			 */
			bool CharacterClass;
			/**
			 *	The pattern being built.
			 */
			RegexCompiler::Pattern Pattern;
			/** 
			 *	\em true if the current parser is being invoked again
			 *	after being the last parser to successfully be invoked,
			 *	\em false otherwise.
			 */
			bool Successive;
			/**
			 *	Parsers should set this to \em false if they do not
			 *	generate any pattern elements.
			 *
			 *	Defaults to \em true.
			 */
			bool Parsed;
			
			
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
					CharacterClass(false),
					Successive(false),
					Parsed(true)
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
			
				Parsed=true;
			
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
			
			
			/**
			 *	Fetches the most recently created pattern element
			 *	cast to a certain type.
			 *
			 *	If the most recently created pattern element is not
			 *	of type \em T, or there is no most recently created
			 *	pattern element, the behaviour is undefined.
			 *
			 *	\tparam T
			 *		The type of the pattern element to retrieve.
			 *
			 *	\return
			 *		A reference to the last pattern element.
			 */
			template <typename T>
			T & Back () noexcept {
			
				return reinterpret_cast<T &>(*(Pattern.back().get()));
			
			}
			
			
			/**
			 *	Adds a new pattern element.
			 *
			 *	Options and Locale will always be passed as the
			 *	last two arguments to the constructor of \em T, they
			 *	do not have to be explicitly provided.
			 *
			 *	\tparam T
			 *		The type of the pattern element to add.
			 *	\tparam Args
			 *		The types of the arguments to forward
			 *		through to a constructor of \em T.
			 *
			 *	\param [in] args
			 *		The arguments of types \em Args which
			 *		shall be forwarded through to a constructor
			 *		of \em T.
			 *
			 *	\return
			 *		A reference to the newly-created pattern
			 *		element.
			 */
			template <typename T, typename... Args>
			T & Add (Args &&... args) {
			
				Pattern.push_back(
					RegexCompiler::Element(
						new T(
							std::forward<Args>(args)...,
							Options,
							Locale
						)
					)
				);
				
				return Back<T>();
			
			}
	
	
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
			 *		\em true if the parse succeeded, \em false
			 *		otherwise.
			 */
			virtual bool operator () (RegexCompilerState & state) const = 0;
			
			
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
