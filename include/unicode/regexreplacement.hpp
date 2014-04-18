/**
 *	\file
 */
 
 
#pragma once


#include <unicode/regex.hpp>
#include <unicode/regexcompilerbase.hpp>
#include <cstddef>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */
	 
	 
	class RegexReplacementParser;
	
	
	class RegexReplacementCompilerAcquisitionPolicy {
	
	
		public:
		
		
			class Type {
			
			
				public:
				
				
					const RegexReplacementParser * Parser;
					std::size_t Priority;
			
			
			};
			
			
			static void Add (const RegexReplacementParser &);
			static void Add (const RegexReplacementParser &, std::size_t);
			
			
			static const std::vector<Type> & Get ();
			constexpr static const RegexReplacementParser * Get (const Type & t) noexcept {
			
				return t.Parser;
			
			}
	
	
	};
	
	
	class RegexReplacementCompilerInvocationPolicy;
	typedef RegexCompilerBase<
		RegexReplacementElement,
		RegexReplacementParser,
		RegexReplacementCompilerAcquisitionPolicy,
		RegexReplacementCompilerInvocationPolicy
	> RegexReplacementCompilerBase;
	
	
	class RegexReplacementCompilerInvocationPolicy {
	
	
		public:
		
		
			static bool Invoke (RegexReplacementCompilerBase &, const RegexReplacementCompilerAcquisitionPolicy::Type &);
	
	
	};
	 
	 
	/**
	 *	\endcond
	 */
	 
	 
	/**
	 *	Compiles regular expression replacements.
	 */
	class RegexReplacementCompiler : public RegexReplacementCompilerBase {
	
	
		private:
		
		
			RegexReplacementCompiler (Iterator begin, Iterator end);
	
	
		public:
		
		
			/**
			 *	Compiles a regular expression replacement.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the replacement.
			 *	\param [in] end
			 *		An iterator to the end of the replacement.
			 *
			 *	\return
			 *		The elements which implement the compiled regular
			 *		expression replacement.
			 */
			static Elements Compile (Iterator begin, Iterator end);
	
	
	};
	
	
	/**
	 *	Parses regular expression replacements into elements which
	 *	implement that replacement.
	 */
	class RegexReplacementParser {
	
	
		public:
		
		
			/**
			 *	Polymorphically cleans up this object.
			 */
			virtual ~RegexReplacementParser () noexcept;
			
			
			/**
			 *	Attempts to parse an element at a certain point in a string.
			 *
			 *	\param [in] compiler
			 *		The regular expression replacement compiler.
			 *
			 *	\return
			 *		\em true if the parse succeeded, \em false
			 *		otherwise.
			 */
			virtual bool operator () (RegexReplacementCompiler & compiler) const = 0;
			
			
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
			virtual void Complete (RegexReplacementElement & element) const;
			
			
			/**
			 *	Attempts to split a smaller element out of another element.
			 *
			 *	Default implementation always fails.
			 *
			 *	\param [in] element
			 *		The element to split.
			 *
			 *	\return
			 *		A std::unique_ptr to a RegexReplacementElement on success,
			 *		a null std::unique_ptr on failure.
			 */
			virtual RegexReplacementCompiler::Element GetLast (RegexReplacementElement & element) const;
	
	
	};
	
	
	/**
	 *	Installs regular expression replacement parsers at start up.
	 *
	 *	\tparam T
	 *		The type of RegexReplacementParser to install.
	 */
	template <typename T>
	class RegexReplacementParserInstaller {
	
	
		private:
		
		
			T parser;
			
			
			template <typename... Args>
			void add (Args &&... args) {
			
				RegexReplacementCompilerAcquisitionPolicy::Add(parser,std::forward<Args>(args)...);
			
			}
			
			
		public:
		
		
			/**
			 *	Creates a new RegexReplacementParserInstaller.
			 */
			RegexReplacementParserInstaller () {
			
				add();
			
			}
			
			
			/**
			 *	Creates a new RegexReplacementParserInstaller which installs
			 *	the associated type of RegexReplacementParserInstaller with
			 *	a certain priority.
			 *
			 *	\param [in] priority
			 *		The numerical priority of the associated RegexReplacementParser.
			 *		Parsers with a lower priority are invoked earlier at each
			 *		given point in a replacement string.
			 */
			RegexReplacementParserInstaller (std::size_t priority) {
			
				add(priority);
			
			}
			
			
			RegexReplacementParserInstaller (const RegexReplacementParserInstaller &) = delete;
			RegexReplacementParserInstaller (RegexReplacementParserInstaller &&) = delete;
			RegexReplacementParserInstaller & operator = (const RegexReplacementParserInstaller &) = delete;
			RegexReplacementParserInstaller & operator = (RegexReplacementParserInstaller &&) = delete;
	
	
	};


}
