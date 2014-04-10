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
#include <unicode/regexoptions.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */


	class RegexParser;
	
	
	/**
	 *	\endcond
	 */
	
	
	/**
	 *	Compiles regular expression patterns.
	 */
	class RegexCompiler {
	
	
		public:
		
		
			/**
			 *	The type of an element produced by a
			 *	RegexCompiler.
			 */
			typedef std::unique_ptr<RegexPatternElement> Element;
			/**
			 *	The type of a compiled pattern produced
			 *	by a RegexCompiler.
			 */
			typedef std::vector<Element> Pattern;
			/**
			 *	The iterator type RegexCompilers use to
			 *	traverse the underlying pattern.
			 */
			typedef const CodePoint * Iterator;
	
	
		private:
		
		
			class RegexParserInfo {
			
			
				public:
				
				
					const RegexParser * Parser;
					std::size_t Priority;
					std::optional<bool> CharacterClass;
			
			
			};
			
			
			static std::vector<RegexParserInfo> & get_parsers ();
			static void add_impl (const RegexParser &, std::size_t, std::optional<bool>);
		
		
			Pattern pattern;
			const RegexParser * last;
			const RegexParser * current;
			
			
			void complete ();
			bool should_invoke (const RegexParserInfo &) const noexcept;
			void rewind (Iterator) noexcept;
	
	
		public:
		
		
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	RegexCompilers use to parse and compiler regular
			 *	expression patterns.
			 *
			 *	\param [in] parser
			 *		The parser.
			 */
			static void Add (const RegexParser & parser);
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	RegexCompilers use to parse and compiler regular
			 *	expression patterns.
			 *
			 *	\param [in] parser
			 *		The parser.
			 *	\param [in] priority
			 *		The priority with which \em parser will be
			 *		added.  Parsers with a lower priority will be
			 *		invoked earlier for each point in a pattern, with
			 *		parsers with a higher priority being invoked
			 *		later.
			 */
			static void Add (const RegexParser & parser, std::size_t priority);
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	RegexCompilers use to parse and compiler regular
			 *	expression patterns.
			 *
			 *	\param [in] parser
			 *		The parser.
			 *	\param [in] character_class
			 *		\em true if this parser should be invoked only
			 *		within custom character classes, \em false if
			 *		it should only be invoked without custom character
			 *		classes.
			 */
			static void Add (const RegexParser & parser, bool character_class);
			/**
			 *	Adds a parser to the internal collection of parsers
			 *	RegexCompilers use to parse and compiler regular
			 *	expression patterns.
			 *
			 *	\param [in] parser
			 *		The parser.
			 *	\param [in] priority
			 *		The priority with which \em parser will be
			 *		added.  Parsers with a lower priority will be
			 *		invoked earlier for each point in a pattern, with
			 *		parsers with a higher priority being invoked
			 *		later.
			 *	\param [in] character_class
			 *		\em true if this parser should be invoked only
			 *		within custom character classes, \em false if
			 *		it should only be invoked without custom character
			 *		classes.
			 */
			static void Add (const RegexParser & parser, std::size_t priority, bool character_class);
			
			
			/**
			 *	Compiles a regular expression pattern.
			 *
			 *	\param [in] begin
			 *		The beginning of the pattern.
			 *	\param [in] end
			 *		The end of the pattern.
			 *	\param [in] options
			 *		The options with which to compile.
			 *	\param [in] locale
			 *		The locale with which to compile.
			 *
			 *	\return
			 *		The compiled pattern.
			 */
			static Pattern Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale);
		
		
			/**
			 *	The current location within the pattern.
			 */
			Iterator Current;
			/**
			 *	The beginning of the pattern.
			 */
			Iterator Begin;
			/**
			 *	The end of the pattern.
			 */
			Iterator End;
			
			
			/**
			 *	The options with which the pattern is being
			 *	compiled.
			 */
			RegexOptions Options;
			/**
			 *	The locale with which the pattern is being
			 *	compiled.
			 */
			const Unicode::Locale & Locale;
			/**
			 *	\em true if the currently executing parser is
			 *	being invoked at a point in the pattern immediately
			 *	after a point at which it already successfully
			 *	parsed, \em false otherwise.
			 */
			bool Successive;
			/**
			 *	\em true if the compiler is compiling a custom
			 *	character class, \em false otherwise.
			 */
			bool CharacterClass;
			
			
			/**
			 *	Creates a new RegexCompiler.
			 *
			 *	\param [in] begin
			 *		The beginning of the pattern.
			 *	\param [in] end
			 *		The end of the pattern.
			 *	\param [in] options
			 *		The options with which to compile.
			 *	\param [in] locale
			 *		The locale with which to compile.
			 */
			RegexCompiler (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept;
			/**
			 *	Creates a new RegexCompiler.
			 *
			 *	\param [in] curr
			 *		The location within the pattern at which
			 *		compilation shall begin.
			 *	\param [in] begin
			 *		The beginning of the pattern.
			 *	\param [in] end
			 *		The end of the pattern.
			 *	\param [in] options
			 *		The options with which to compile.
			 *	\param [in] locale
			 *		The locale with which to compile.
			 */
			RegexCompiler (Iterator curr, Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept;
			/**
			 *	Copies a RegexCompiler, except the pattern it has
			 *	compiled and related state information.
			 *
			 *	\param [in] other
			 *		The RegexCompiler to copy.
			 */
			RegexCompiler (const RegexCompiler & other) noexcept;
			
			
			/**
			 *	Checks to see if the end of the pattern has been
			 *	reached.
			 *
			 *	\return
			 *		\em false if the end of the pattern has been
			 *		reached, \em true otherwise.
			 */
			operator bool () const noexcept {
			
				return Current!=End;
			
			}
			/**
			 *	Obtains the current code point from the pattern.
			 *
			 *	\return
			 *		A reference to a code point.
			 */
			const CodePoint & operator * () const noexcept {
			
				return *Current;
			
			}
			/**
			 *	Obtains a pointer to the current code point from
			 *	the pattern.
			 *
			 *	\return
			 *		A pointer to a code point.
			 */
			const CodePoint * operator -> () const noexcept {
			
				return Current;
			
			}
			/**
			 *	Advances the compiler to the next code point within
			 *	the pattern.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompiler & operator ++ () noexcept {
			
				++Current;
				
				return *this;
			
			}
			/**
			 *	Backs the compiler up to the last code point within
			 *	the pattern.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompiler & operator -- () noexcept {
			
				--Current;
				
				return *this;
			
			}
			
			
			/**
			 *	Checks to see if a certain option or options are
			 *	set.
			 *
			 *	\param [in] option
			 *		The option or options to check.
			 *
			 *	\return
			 *		\em true if all options specified by \em option
			 *		are set, \em false otherwise.
			 */
			bool Check (RegexOptions option) const noexcept {
			
				return Unicode::Check(Options,option);
			
			}
			
			
			/**
			 *	Retrieves and clears the compiled pattern.
			 *
			 *	\return
			 *		The compiled pattern.
			 */
			Pattern Get ();
			/**
			 *	Clears and sets the compiled pattern.
			 */
			void Set (Pattern pattern);
			
			
			/**
			 *	Determines the number of pattern elements stored
			 *	within this compiler.
			 *
			 *	\return
			 *		The number of RegexPatternElements in the
			 *		pattern the compiler is building.
			 */
			std::size_t Size () const noexcept {
			
				return pattern.size();
			
			}
			
			
			/**
			 *	Retrieves the rear pattern element and removes
			 *	it from the internal collection of pattern elements.
			 *
			 *	If the compiler contains no pattern elements,
			 *	behaviour is undefined.
			 *
			 *	\return
			 *		The rear pattern element.
			 */
			Element Pop ();
			
			
			/**
			 *	Retrieves the last compiled RegexPatternElement
			 *	cast to a certain type.
			 *
			 *	If there are no compiled RegexPatternElements, or
			 *	the last RegexPatternElement is not of type \em T,
			 *	the behaviour is undefined.
			 *
			 *	\tparam T
			 *		The type to which the last RegexPatternElement
			 *		shall be cast.
			 *
			 *	\return
			 *		A reference to the last RegexPatternElement cast
			 *		to type \em T.
			 */
			template <typename T>
			T & Back () noexcept {
			
				return reinterpret_cast<T &>(*(pattern.back().get()));
			
			}
			
			
			/**
			 *	Creates a new pattern element of a certain type.
			 *
			 *	Options and Locale will always, implicitly be passed,
			 *	respectively, as the last two arguments to the
			 *	constructor of \em T.
			 *
			 *	\tparam T
			 *		The type of RegexPatternElement to create.
			 *	\tparam Args
			 *		The types of the arguments to forward through
			 *		to the constructor of \em T.
			 *
			 *	\param [in] args
			 *		The arguments of types \em Args which shall be
			 *		forwarded through to the constructor of \em T.
			 *
			 *	\return
			 *		A std::unique_ptr to the created RegexPatternElement.
			 */
			template <typename T, typename... Args>
			Element Create (Args &&... args) {
			
				return Element(
					new T(
						std::forward<Args>(args)...,
						Options,
						Locale
					)
				);
			
			}
			
			
			/**
			 *	Adds a std::unique_ptr to a RegexPatternElement
			 *	to the back of the pattern.
			 *
			 *	\param [in] element
			 *		A std::unique_ptr to the RegexPatternElement
			 *		to add.
			 */
			void Add (Element element);
			
			
			/**
			 *	Adds a RegexPatternElement of a certain type to
			 *	the back of the pattern.
			 *
			 *	The RegexPatternElement will be obtained through
			 *	a call to Create.
			 *
			 *	\tparam T
			 *		The type of RegexPatternElement to add.
			 *	\tparam Args
			 *		The types of the arguments to forward through
			 *		to Create of \em T.
			 *
			 *	\param [in] args
			 *		The arguments of types \em Args which shall
			 *		be forwarded through to Create.
			 *
			 *	\return
			 *		A reference to the newly-created RegexPatternElement.
			 */
			template <typename T, typename... Args>
			T & Add (Args &&... args) {
			
				Add(Create<T>(std::forward<Args>(args)...));
				
				return Back<T>();
			
			}
			
			
			/**
			 *	Throws a RegexError with a certain error message.
			 *
			 *	\param [in] what
			 *		The error message.
			 */
			[[noreturn]]
			void Raise (const char * what) const;
			
			
			/**
			 *	Checks to see if compilation is finished at this
			 *	point in the string.
			 *
			 *	Default implementation returns \em true only when
			 *	operator bool returns \em false.
			 *
			 *	If \em true is not returned when operator bool
			 *	returns \em false, the behaviour is undefined.
			 *
			 *	\return
			 *		\em true if compilation is finished, \em false
			 *		otherwise.
			 */
			virtual bool Done ();
			
			
			/**
			 *	Compiles the regular expression pattern.
			 */
			void operator () ();
	
	
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
			 *	\param [in] compiler
			 *		The regular expression compiler.
			 *
			 *	\return
			 *		\em true if the parse succeeded, \em false
			 *		otherwise.
			 */
			virtual bool operator () (RegexCompiler & compiler) const = 0;
			
			
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
