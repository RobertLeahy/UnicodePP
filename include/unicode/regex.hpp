/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/regexoptions.hpp>
#include <unicode/string.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	Encapsulates a string representation of a
	 *	pattern element.
	 */
	class RegexToString {
	
	
		public:
		
		
			/**
			 *	A string description of the pattern element
			 *	on which ToString was called.
			 */
			String Parent;
			/** 
			 *	Descriptions of all the child pattern elements
			 *	of the pattern element on which ToString was
			 *	called.
			 */
			std::vector<RegexToString> Children;
	
	
	};
	
	
	/**
	 *	Contains the regular expression engine's internal
	 *	state.
	 */
	class RegexState {
	
	
		public:
		
		
			class Iterator : public std::iterator<std::random_access_iterator_tag,const CodePoint> {
			
			
				private:
				
				
					typedef std::reverse_iterator<const CodePoint *> r_type;
				
				
					bool reversed;
					union {
						r_type r;
						const CodePoint * i;
					};
					
					
					void destroy () noexcept;
					void copy (const Iterator &) noexcept;
					void advance (std::ptrdiff_t) noexcept;
					void retreat (std::ptrdiff_t) noexcept;
					
					
				public:
				
				
					Iterator () noexcept;
					Iterator (const Iterator &) noexcept;
					Iterator & operator = (const Iterator &) noexcept;
					~Iterator () noexcept;
					
					
					Iterator (const CodePoint *, bool reversed=false) noexcept;
					
					
					bool Reversed () const noexcept;
					
					
					void Reverse () & noexcept;
					Iterator Reverse () const & noexcept;
					
					
					const CodePoint * Base () const noexcept;
					
					
					const CodePoint & operator * () const noexcept;
					const CodePoint * operator -> () const noexcept;
					
					
					Iterator & operator ++ () noexcept;
					Iterator operator ++ (int) noexcept;
					Iterator & operator -- () noexcept;
					Iterator operator -- (int) noexcept;
					
					
					Iterator & operator += (std::ptrdiff_t) noexcept;
					Iterator & operator -= (std::ptrdiff_t) noexcept;
					
					
					Iterator operator + (std::ptrdiff_t) const noexcept;
					std::ptrdiff_t operator - (const Iterator &) const noexcept;
					Iterator operator - (std::ptrdiff_t) const noexcept;
					
					
					const CodePoint & operator [] (std::size_t) const noexcept;
					
					
					bool operator < (const Iterator &) const noexcept;
					bool operator > (const Iterator &) const noexcept;
					bool operator <= (const Iterator &) const noexcept;
					bool operator >= (const Iterator &) const noexcept;
					
					
					bool operator == (const Iterator &) const noexcept;
					bool operator != (const Iterator &) const noexcept;
			
			
			};
	
	
		private:
		
		
			Iterator b;
			Iterator l;
			Iterator e;
			
			
		public:
		
		
			/**
			 *	Creates a new regular expression engine state.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a sequence
			 *		of code points against which the regular
			 *		expression engine will attempt to match.
			 *	\param [in] end
			 *		An iterator to the end of a sequence of
			 *		code points against which the regular expression
			 *		engine will attempt to match.
			 *	\param [in] reversed
			 *		\em true if the regular expression engine
			 *		should proceed from end to begin, \em false
			 *		otherwise.
			 */
			RegexState (const CodePoint * begin, const CodePoint * end, bool reversed) noexcept;
			
			
			/**
			 *	Determines if the regular expression engine is
			 *	at a valid position within the string against which
			 *	it is matching.
			 *
			 *	\return
			 *		\em true if there are code points remaining in
			 *		the sequence, \em false otherwise.
			 */
			operator bool () const noexcept;
			/**
			 *	Returns an iterator to the regular expression
			 *	engine's current position within the seequence
			 *	against which it is matching.
			 *
			 *	\return
			 *		An iterator.
			 */
			Iterator begin () const noexcept;
			/**
			 *	Returns an iterator to the beginning of the
			 *	underlying sequence.
			 *
			 *	This function is unaffected by whether the
			 *	underlying sequence is reversed or not, and
			 *	will always return an iterator to the beginning
			 *	of the underlying string.
			 *
			 *	\return
			 *		An iterator.
			 */
			const CodePoint * Begin () const noexcept;
			/**
			 *	Returns an iterator to the end of the sequence
			 *	against which the regular expression engine is
			 *	matching.
			 *
			 *	\return
			 *		An iterator.
			 */
			Iterator end () const noexcept;
			/**
			 *	Returns an iterator to the end of the
			 *	underlying sequence.
			 *
			 *	This function is unaffected by whether the
			 *	underlying sequence is reversed or not, and
			 *	will always return an iterator to the end
			 *	of the underlying string.
			 *
			 *	\return
			 *		An iterator.
			 */
			const CodePoint * End () const noexcept;
			/**
			 *	Obtains a reference to the current code point
			 *	within the sequence against which the regular
			 *	expression engine is matching.
			 *
			 *	\return
			 *		A reference to a code point.
			 */
			const CodePoint & operator * () const noexcept;
			/**
			 *	Obtains a pointer to the current code point within
			 *	the sequence against which the regular expression
			 *	engine is matching.
			 *
			 *	\return
			 *		A pointer to a code point.
			 */
			const CodePoint * operator -> () const noexcept;
			/**
			 *	Advances the regular expression engine to the next
			 *	code point in the sequence.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexState & operator ++ () noexcept;
			/**
			 *	Backs the regular expression up to the previous code
			 *	point in the sequence.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexState & operator -- () noexcept;
			/**
			 *	Advances the regular expression engine by a specific
			 *	number of code points.
			 *
			 *	\param [in] dist
			 *		The number of code points by which the engine should
			 *		advance.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexState & operator += (std::ptrdiff_t dist) noexcept;
			/**
			 *	Backs the regular expression engine up by a specific
			 *	number of code points.
			 *
			 *	\param [in] dist
			 *		The number of code points by which the engine
			 *		should retreat.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexState & operator -= (std::ptrdiff_t dist) noexcept;
			/**
			 *	Determines the number of code points remaining in the
			 *	sequence.
			 *
			 *	\return
			 *		The number of code points remaining in the sequence.
			 */
			std::size_t Remaining () const noexcept;
			/**
			 *	Determines whether the underlying sequence is being
			 *	traversed in reverse order or not.
			 *
			 *	\return
			 *		\em true if the underlying sequence is being
			 *		traversed in reverse order, \em false otherwise.
			 */
			bool Reversed () const noexcept;
	
	
	};


	/**
	 *	The base type from which all pattern elements derive.
	 *
	 *	A pattern element is a single element in a compiled
	 *	regular expression.
	 */
	class RegexPatternElement {
	
	
		protected:
		
		
			RegexOptions Options;
			const Unicode::Locale & Locale;
	
	
		public:
		
		
			RegexPatternElement (RegexOptions options, const Unicode::Locale & locale) noexcept;
			/**
			 *	Polymorphically cleans up all resources held
			 *	by this RegexPatternElement.
			 */
			virtual ~RegexPatternElement () noexcept;
			
			
			/**
			 *	Obtains a string representation of a pattern
			 *	element.
			 *
			 *	\return
			 *		A RegexToString object containing a string
			 *		representation of this pattern element and
			 *		all child pattern elements (if applicable).
			 */
			virtual RegexToString ToString () const = 0;
	
	
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
			
			
			/**
			 *	Obtains a vector of RegexToString objects which
			 *	may be formatted to obtain a string representation
			 *	of this regular expression.
			 *
			 *	\return
			 *		A vector of RegexToString objects.
			 */
			std::vector<RegexToString> ToString () const;
	
	
	};


}
