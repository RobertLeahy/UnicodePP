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
	
	
	class RegexCompiler {
	
	
		public:
		
		
			typedef std::unique_ptr<RegexPatternElement> Element;
			typedef std::vector<Element> Pattern;
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
		
		
			static void Add (const RegexParser & parser);
			static void Add (const RegexParser & parser, std::size_t priority);
			static void Add (const RegexParser & parser, bool character_class);
			static void Add (const RegexParser & parser, std::size_t priority, bool character_class);
			
			
			static Pattern Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale);
		
		
			Iterator Current;
			Iterator Begin;
			Iterator End;
			
			
			RegexOptions Options;
			const Unicode::Locale & Locale;
			bool Successive;
			bool CharacterClass;
			
			
			RegexCompiler (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept;
			RegexCompiler (Iterator curr, Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept;
			
			
			operator bool () const noexcept {
			
				return Current!=End;
			
			}
			const CodePoint & operator * () const noexcept {
			
				return *Current;
			
			}
			const CodePoint * operator -> () const noexcept {
			
				return Current;
			
			}
			RegexCompiler & operator ++ () noexcept {
			
				++Current;
				
				return *this;
			
			}
			RegexCompiler & operator -- () noexcept {
			
				--Current;
				
				return *this;
			
			}
			
			
			bool Check (RegexOptions option) const noexcept {
			
				return Unicode::Check(Options,option);
			
			}
			
			
			Pattern Get ();
			void Set (Pattern pattern);
			
			
			template <typename T>
			T & Back () noexcept {
			
				return reinterpret_cast<T &>(*(pattern.back().get()));
			
			}
			
			
			template <typename T, typename... Args>
			T & Add (Args &&... args) {
			
				complete();
			
				pattern.push_back(
					Element(
						new T(
							std::forward<Args>(args)...,
							Options,
							Locale
						)
					)
				);
				
				last=current;
				
				return Back<T>();
			
			}
			
			
			[[noreturn]]
			void Raise (const char * what) const;
			
			
			virtual bool Done ();
			
			
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
