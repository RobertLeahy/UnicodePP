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
#include <unicode/regexcompilerbase.hpp>
#include <unicode/regexoptions.hpp>
#include <unicode/string.hpp>
#include <cstddef>
#include <iterator>
#include <optional>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */


	class RegexParser;
	
	
	class RegexCompilerAcquisitionPolicy {
	
	
		public:
		
		
			class Type {
			
			
				public:
				
				
					const RegexParser * Parser;
					std::size_t Priority;
					std::optional<bool> CharacterClass;
			
			
			};
		
		
			static void Add (const RegexParser &);
			static void Add (const RegexParser &, std::size_t);
			static void Add (const RegexParser &, bool);
			static void Add (const RegexParser &, std::size_t, bool);
			
			
			static const std::vector<Type> & Get ();
			constexpr static const RegexParser * Get (const Type & t) noexcept {
			
				return t.Parser;
			
			}
	
	
	};
	
	
	class RegexCompilerCreationPolicy;
	class RegexCompilerInvocationPolicy;
	typedef RegexCompilerBase<
		RegexPatternElement,
		RegexParser,
		RegexCompilerAcquisitionPolicy,
		RegexCompilerInvocationPolicy,
		RegexCompilerCreationPolicy
	> RegexCompilerBaseType;
	

	class RegexCompilerCreationPolicy {
	
	
		public:
		
		
			template <typename T, typename... Args>
			static RegexCompilerBaseType::ElementType * Create (const RegexCompilerBaseType &, Args &&...);
	
	
	};
	
	
	class RegexCompilerInvocationPolicy {
	
	
		public:
		
		
			static bool Invoke (RegexCompilerBaseType &, const RegexCompilerAcquisitionPolicy::Type &);
	
	
	};
	
	
	/**
	 *	\endcond
	 */
	
	
	/**
	 *	Compiles regular expression patterns.
	 */
	class RegexCompiler : public RegexCompilerBaseType {
	
	
		public:
		
		
			/**
			 *	The type RegexCompilers use to maintain a
			 *	list of RegexPatternElement's associated with
			 *	a certain named or numbered capturing group.
			 */
			typedef std::vector<const RegexPatternElement *> Groups;
	
	
		private:
		

			typedef std::vector<const RegexPatternElement **> PendingGroups;
			
			
			class GroupsInfo {
			
			
				public:
				
				
					PendingGroups Pending;
					Groups Elements;
			
			
			};
			
			
			class GroupInfo {
			
			
				public:
				
				
					const RegexPatternElement * Element;
					PendingGroups Pending;
					
					
					GroupInfo ();
					GroupInfo (const RegexPatternElement &);
			
			
			};
			
			
			template <typename T>
			using GroupsMapping=std::unordered_map<T,GroupsInfo>;
			static void get_capturing_group (GroupsInfo &, const RegexPatternElement * &);
			template <typename T>
			static const RegexPatternElement * get_capturing_group (const GroupsMapping<T> &, const T &);
			std::vector<GroupInfo>::iterator find_groups_end () noexcept;
			void add_capturing_group (const RegexPatternElement &);
			template <typename T>
			void add_capturing_group (T &&, GroupsMapping<typename std::decay<T>::type> &, const RegexPatternElement &);
			std::vector<GroupInfo>::iterator find_relative_capturing_group (std::ptrdiff_t);
			
			
			std::size_t & automatic;
			std::vector<GroupInfo> & groups;
			GroupsMapping<std::size_t> & numbered;
			GroupsMapping<String> & named;
			
			
			[[noreturn]]
			void group_undeclared () const;
			
			
			template <typename T>
			void complete (GroupsMapping<T> &);
			void complete ();
			
			
			RegexCompiler (
				Iterator begin,
				Iterator curr,
				Iterator end,
				std::size_t & automatic,
				std::vector<GroupInfo> & groups,
				GroupsMapping<std::size_t> & numbered,
				GroupsMapping<String> & named,
				RegexOptions options,
				const Unicode::Locale & locale
			) noexcept;
	
	
		public:
			
			
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
			static Elements Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale);
			
			
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
			 *	\em true if the compiler is compiling a custom
			 *	character class, \em false otherwise.
			 */
			bool CharacterClass;
			

			/**
			 *	Copies a RegexCompiler, except the pattern it has
			 *	compiled and related state information.
			 *
			 *	\param [in] other
			 *		The RegexCompiler to copy.
			 */
			RegexCompiler (const RegexCompiler & other) noexcept;
			
			
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
			 *	Retrieves the next automatic capturing group
			 *	number.
			 *
			 *	\return
			 *		An automatic capturing group number.
			 */
			std::size_t GetCaptureNumber () noexcept;
			/**
			 *	Retrieves the last automatic capturing group
			 *	number.
			 *
			 *	\return
			 *		The last automatic capturing group number.
			 */
			std::size_t GetLastCaptureNumber () noexcept;
			/**
			 *	Attempts to obtain a pointer to a RegexPatternElement
			 *	which captures a certain named capturing group.
			 *
			 *	If such a RegexPatternElement has not yet been compiled,
			 *	the compiler will set \em ptr if one is compiled in
			 *	the future.
			 *
			 *	If such a RegexPatternElement is not compiled in the
			 *	future, an exception will be thrown.
			 *
			 *	\param [in] key
			 *		The name.
			 *	\param [in] ptr
			 *		A reference to a const pointer to a RegexPatternElement,
			 *		which will be populated either immediately (if
			 *		a RegexPatternElement which captures \em key exists)
			 *		or later.
			 */
			void GetCapturingGroup (const String & key, const RegexPatternElement * & ptr);
			/**
			 *	Attempts to obtain a pointer to a RegexPatternElement
			 *	which captures a certain named capturing group.
			 *
			 *	If such a RegexPatternElement has not yet been compiled,
			 *	the compiler will set \em ptr if one is compiled in
			 *	the future.
			 *
			 *	If such a RegexPatternElement is not compiled in the
			 *	future, an exception will be thrown.
			 *
			 *	\param [in] key
			 *		The name.
			 *	\param [in] ptr
			 *		A reference to a const pointer to a RegexPatternElement,
			 *		which will be populated either immediately (if
			 *		a RegexPatternElement which captures \em key exists)
			 *		or later.
			 */
			void GetCapturingGroup (String && key, const RegexPatternElement * & ptr);
			/**
			 *	Attempts to obtain a pointer to a RegexPatternElement
			 *	which captures a certain numbered capturing group.
			 *
			 *	If such a RegexPatternElement has not yet been compiled,
			 *	the compiler will set \em ptr if one is compiled in
			 *	the future.
			 *
			 *	If such a RegexPatternElement is not compiled in the
			 *	future, an exception will be thrown.
			 *
			 *	\param [in] key
			 *		The number.
			 *	\param [in] ptr
			 *		A reference to a const pointer to a RegexPatternElement,
			 *		which will be populated either immediately (if
			 *		a RegexPatternElement which captures \em key exists)
			 *		or later.
			 */
			void GetCapturingGroup (std::size_t key, const RegexPatternElement * & ptr);
			/**
			 *	Attempts to obtain a pointer to a RegexPatternElement which
			 *	captures a group relative to the current location.
			 *
			 *	For example, if \em dist is -1, a pointer to the last capturing
			 *	group will be retrieved.
			 *
			 *	If such a RegexPatternElement has not yet been compiled,
			 *	the compiler will set \em ptr if one is compiled in
			 *	the future.
			 *
			 *	If such a RegexPatternElement is not compiled in the
			 *	future, an exception will be thrown.
			 *
			 *	\param [in] dist
			 *		The offset of the capturing group to retrieve.
			 *	\param [in] ptr
			 *		A reference to a const pointer to a RegexPatternElement,
			 *		which will be populated either immediately (if
			 *		a RegexPatternElement which captures \em key exists)
			 *		or later.
			 */
			void GetRelativeCapturingGroup (std::ptrdiff_t dist, const RegexPatternElement * & ptr);
			/**
			 *	Retrieves a capturing group associated with a certain name if
			 *	one exists.
			 *
			 *	\param [in] key
			 *		The name of the capturing group to retrieve.
			 *
			 *	\return
			 *		A pointer to a RegexPatternElement if there is a RegexPatternElement
			 *		associated with \em key, \em nullptr otherwise.
			 */
			const RegexPatternElement * GetCapturingGroup (const String & key) const;
			/**
			 *	Retrieves a capturing group associated with a certain number if
			 *	one exists.
			 *
			 *	\param [in] key
			 *		The number of the capturing group to retrieve.
			 *
			 *	\return
			 *		A pointer to a RegexPatternElement if there is a RegexPatternElement
			 *		associated with \em key, \em nullptr otherwise.
			 */
			const RegexPatternElement * GetCapturingGroup (std::size_t key) const;
			/**
			 *	Adds a pattern element as capturing under a certain name.
			 *
			 *	\param [in] key
			 *		The name.
			 *	\param [in] element
			 *		The pattern element.
			 */
			void AddCapturingGroup (const String & key, const RegexPatternElement & element);
			/**
			 *	Adds a pattern element as capturing under a certain name.
			 *
			 *	\param [in] key
			 *		The name.
			 *	\param [in] element
			 *		The pattern element.
			 */
			void AddCapturingGroup (String && key, const RegexPatternElement & element);
			/**
			 *	Adds a pattern element as capturing under a certain number.
			 *
			 *	\param [in] key
			 *		The number.
			 *	\param [in] element
			 *		The pattern element.
			 */
			void AddCapturingGroup (std::size_t key, const RegexPatternElement & element);
	
	
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
			
			
			/**
			 *	Attempts to split a smaller pattern element out of another
			 *	pattern element.
			 *
			 *	Default implementation always fails.
			 *
			 *	\param [in] element
			 *		The element to split.
			 *
			 *	\return
			 *		A std::unique_ptr to a RegexPatternElement on success,
			 *		a null std::unique_ptr on failure.
			 */
			virtual RegexCompiler::Element GetLast (RegexPatternElement & element) const;
	
	
	};
	
	
	/**
	 *	\cond
	 */
	 
	 
	template <typename T, typename... Args>
	RegexCompilerBaseType::ElementType * RegexCompilerCreationPolicy::Create (
		const RegexCompilerBaseType & compiler,
		Args &&... args
	) {
	
		auto & c=reinterpret_cast<const RegexCompiler &>(compiler);
	
		return new T(
			std::forward<Args>(args)...,
			c.Options,
			c.Locale
		);
	
	}
	 
	 
	/**
	 *	\endcond
	 */
	
	
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
			
			
			template <typename... Args>
			void add (Args &&... args) {
			
				RegexCompilerAcquisitionPolicy::Add(parser,std::forward<Args>(args)...);
			
			}
			
			
		public:
		
		
			/**
			 *	Installs a parser of type \em T.
			 */
			RegexParserInstaller () {
			
				add();
			
			}
			/**
			 *	Installs a parser of type \em T.
			 *
			 *	\param [in] priority
			 *		The priority.
			 */
			RegexParserInstaller (std::size_t priority) {
			
				add(priority);
			
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
			
				add(character_class);
			
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
			
				add(priority,character_class);
			
			}
			
			
			RegexParserInstaller (const RegexParserInstaller &) = delete;
			RegexParserInstaller (RegexParserInstaller &&) = delete;
			RegexParserInstaller & operator = (const RegexParserInstaller &) = delete;
			RegexParserInstaller & operator = (RegexParserInstaller &&) = delete;
	
	
	};


}
