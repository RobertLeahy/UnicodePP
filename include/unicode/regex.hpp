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
#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */
	 
	 
	class RegexEngine;
	class RegexState;
	 
	 
	/**
	 *	\endcond
	 */
	 

	class RegexIterator : public std::iterator<std::random_access_iterator_tag,const CodePoint> {
	
	
		private:
		
		
			typedef std::reverse_iterator<const CodePoint *> r_type;
		
		
			bool reversed;
			union {
				r_type r;
				const CodePoint * i;
			};
			
			
			void destroy () noexcept;
			void copy (const RegexIterator &) noexcept;
			void advance (std::ptrdiff_t) noexcept;
			void retreat (std::ptrdiff_t) noexcept;
			
			
		public:
		
		
			RegexIterator () noexcept;
			RegexIterator (const RegexIterator &) noexcept;
			RegexIterator & operator = (const RegexIterator &) noexcept;
			~RegexIterator () noexcept;
			
			
			RegexIterator (const CodePoint *, bool reversed=false) noexcept;
			
			
			bool Reversed () const noexcept;
			
			
			void Reverse () & noexcept;
			RegexIterator Reverse () const & noexcept;
			
			
			const CodePoint * Base () const noexcept;
			
			
			const CodePoint & operator * () const noexcept;
			const CodePoint * operator -> () const noexcept;
			
			
			RegexIterator & operator ++ () noexcept;
			RegexIterator operator ++ (int) noexcept;
			RegexIterator & operator -- () noexcept;
			RegexIterator operator -- (int) noexcept;
			
			
			RegexIterator & operator += (std::ptrdiff_t) noexcept;
			RegexIterator & operator -= (std::ptrdiff_t) noexcept;
			
			
			RegexIterator operator + (std::ptrdiff_t) const noexcept;
			std::ptrdiff_t operator - (const RegexIterator &) const noexcept;
			RegexIterator operator - (std::ptrdiff_t) const noexcept;
			
			
			const CodePoint & operator [] (std::size_t) const noexcept;
			
			
			bool operator < (const RegexIterator &) const noexcept;
			bool operator > (const RegexIterator &) const noexcept;
			bool operator <= (const RegexIterator &) const noexcept;
			bool operator >= (const RegexIterator &) const noexcept;
			
			
			bool operator == (const RegexIterator &) const noexcept;
			bool operator != (const RegexIterator &) const noexcept;
	
	
	};


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
	 *	The base type from which all pattern elements derive.
	 *
	 *	A pattern element is a single element in a compiled
	 *	regular expression.
	 */
	class RegexPatternElement {
	
	
		protected:
		
		
			RegexOptions Options;
			const Unicode::Locale & Locale;
			
			
			bool Check (RegexOptions option) const noexcept {
			
				return Unicode::Check(Options,option);
			
			}
	
	
		public:
		
		
			RegexPatternElement (RegexOptions options, const Unicode::Locale & locale) noexcept;
			/**
			 *	Polymorphically cleans up all resources held
			 *	by this RegexPatternElement.
			 */
			virtual ~RegexPatternElement () noexcept;
			
			
			/**
			 *	Attempts to match the pattern element.
			 *
			 *	\param [in] engine
			 *		The regular expression engine.
			 *	\param [in] state
			 *		The state for this pattern element.
			 *
			 *	\return
			 *		\em true if the match succeeded, \em false
			 *		otherwise.
			 */
			virtual bool operator () (RegexEngine & engine, RegexState & state) const = 0;
			
			
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
	 *	A base class from which state objects specific to
	 *	a certain type of pattern element may derive.
	 */
	class RegexPrivateState {
	
	
		public:
		
		
			RegexPrivateState () = default;
			RegexPrivateState (const RegexPrivateState &) = delete;
			RegexPrivateState (RegexPrivateState &&) = delete;
			RegexPrivateState & operator = (const RegexPrivateState &) = delete;
			RegexPrivateState & operator = (RegexPrivateState &&) = delete;
		
		
			/**
			 *	Polymorphically cleans up all resources held
			 *	by this object.
			 */
			virtual ~RegexPrivateState () noexcept;
			
			
			/**
			 *	Rewinds the regular expression engine.
			 *
			 *	Default implementation does nothing.
			 *
			 *	\param [in] engine
			 *		The regular expression engine.
			 */
			virtual void Rewind (RegexEngine & engine);
	
	
	};
	
	
	/**
	 *	The state associated with a certain pattern element
	 *	during the execution of a regular expression.
	 */
	class RegexState {
	
	
		private:
		
		
			typedef RegexPrivateState state_type;
			typedef std::unique_ptr<state_type> ptr_type;
		
		
			ptr_type state;
			RegexIterator loc;
			
			
		public:
		
		
			/**
			 *	Set to \em true if the pattern element associated
			 *	with this state can be invoked again to obtain a
			 *	different match.
			 *
			 *	Defaults to \em false.
			 */
			bool CanBacktrack;
			/**
			 *	Set to \em true if the pattern element associated
			 *	with this state cannot be backtracked over.
			 *
			 *	Defaults to \em false.
			 */
			bool PreventsBacktracking;
		
		
			RegexState () = delete;
			/**
			 *	Creates a new RegexPatternElement which stores
			 *	the location at which the associated pattern
			 *	element was invoked.
			 *
			 *	\param [in] loc
			 *		The location at which the pattern element was
			 *		invoked.
			 */
			RegexState (RegexIterator loc) noexcept
				:	loc(loc),
					CanBacktrack(false),
					PreventsBacktracking(false)
			{	}
			
			
			/**
			 *	Rewinds the regular expression engine to the
			 *	it was at before the associated pattern element
			 *	was invoked.
			 *
			 *	\param [in] engine
			 *		The regular expression engine.
			 */
			void Rewind (RegexEngine & engine);
		
		
			/**
			 *	Determines whether or not this state object has
			 *	been imbued with a private state.
			 *
			 *	\return
			 *		\em true if this state object contains a
			 *		private state, \em false otherwise.
			 */
			explicit operator bool () const noexcept {
			
				return static_cast<bool>(state);
			
			}
		
		
			/**
			 *	Retrieves the private state associated with
			 *	this state object.
			 *
			 *	If this state object has not been imbued,
			 *	results in undefined behaviour.
			 *
			 *	\tparam T
			 *		The type of the private state.  If this
			 *		type is incorrect, the result is undefined
			 *		behaviour.
			 *
			 *	\return
			 *		A reference to the private state.
			 */
			template <typename T>
			T & Get () noexcept {
			
				return reinterpret_cast<T &>(*state);
			
			}
		
		
			/**
			 *	Imbues this state object with a private state.
			 *
			 *	\tparam T
			 *		The type of the private state.  Must be
			 *		derived from RegexPrivateState.
			 *
			 *	\return
			 *		A reference to the private state.
			 */
			template <typename T, typename... Args>
			T & Imbue (Args &&... args) {
			
				state=ptr_type(
					new state_type(
						std::forward<Args>(args)...
					)
				);
				
				return Get<T>();
			
			}
	
	
	};
	
	
	/**
	 *	A compiled regular expression.
	 */
	class Regex {
	
	
		public:
		
		
			typedef std::vector<std::unique_ptr<RegexPatternElement>> Pattern;
	
	
		private:
		
		
			Pattern pattern;
			bool rtl;
			
			
		public:
		
		
			/**
			 *	Escapes a string.
			 *
			 *	Inserting the escaped string into a regular expression
			 *	pattern will cause all code points therein to be treated
			 *	as literals by the regular expression engine, i.e. none of
			 *	them will be interpreted as metacharacters.
			 *
			 *	\param [in] str
			 *		The string to escape.
			 *
			 *	\return
			 *		The escaped string.
			 */
			static String Escape (String str);
		
		
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
	
	
	/**
	 *	Evaluates regular expressions.
	 */
	class RegexEngine {
	
	
		private:
		
		
			RegexIterator b;
			RegexIterator l;
			RegexIterator e;
			
			
			const Regex::Pattern & pattern;
			std::vector<RegexState> states;
			
			
			template <typename Iterator>
			bool backtrack (Iterator & begin, Iterator end);
			template <typename Iterator>
			bool execute (Iterator begin, Iterator end);
			
			
		public:
		
		
			/**
			 *	Only meaningful after the engine has been invoked
			 *	at least once.
			 *
			 *	Determines whether or not the engine may be invoked
			 *	again to obtain a different match.
			 */
			bool CanBacktrack;
			/**
			 *	Only meaningful after the engine has been invoked
			 *	at least once.
			 *
			 *	Determines whether or not the pattern elements the
			 *	engine matches against may be backtracked over.
			 */
			bool PreventsBacktracking;
			
			
			RegexEngine (const RegexEngine &) = delete;
			RegexEngine (RegexEngine &&) = delete;
			RegexEngine & operator = (const RegexEngine &) = delete;
			RegexEngine & operator = (RegexEngine &&) = delete;
		
		
			/**
			 *	Creates a new regular expression engine.
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
			RegexEngine (const CodePoint * begin, const CodePoint * end, bool reversed, const Regex::Pattern & pattern) noexcept;
			
			
			/**
			 *	Determines if the regular expression engine is
			 *	at a valid position within the string against which
			 *	it is matching.
			 *
			 *	\return
			 *		\em true if there are code points remaining in
			 *		the sequence, \em false otherwise.
			 */
			explicit operator bool () const noexcept;
			/**
			 *	Returns an iterator to the regular expression
			 *	engine's current position within the seequence
			 *	against which it is matching.
			 *
			 *	\return
			 *		An iterator.
			 */
			RegexIterator begin () const noexcept;
			/**
			 *	Sets the regular expression engine's current position
			 *	within the sequence against which it is matching.
			 *
			 *	\param [in] i
			 *		The iterator.
			 */
			void begin (RegexIterator i) noexcept;
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
			RegexIterator end () const noexcept;
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
			RegexEngine & operator ++ () noexcept;
			/**
			 *	Backs the regular expression up to the previous code
			 *	point in the sequence.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexEngine & operator -- () noexcept;
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
			RegexEngine & operator += (std::ptrdiff_t dist) noexcept;
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
			RegexEngine & operator -= (std::ptrdiff_t dist) noexcept;
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
			
			
			/**
			 *	Invokes the engine.
			 *
			 *	\return
			 *		\em true if a match was found, \em false otherwise.
			 */
			bool operator () ();
	
	
	};


}
