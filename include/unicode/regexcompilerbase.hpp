/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/regexerror.hpp>
#include <memory>
#include <utility>


namespace Unicode {


	/**
	 *	\cond
	 */


	class RegexCompilerDefaultCreationPolicy {
	
	
		public:
		
		
			template <typename T, typename TCompiler, typename... Args>
			static typename TCompiler::ElementType * Create (const TCompiler &, Args &&... args) {
			
				return new T(std::forward<Args>(args)...);
			
			}
	
	
	};
	
	
	/**
	 *	\endcond
	 */


	/**
	 *	The base class for both the regular expression compiler and the
	 *	regular expression replacement compiler.
	 */
	template <
		typename TElement,
		typename TParser,
		typename AcquisitionPolicy,
		typename InvocationPolicy,
		typename CreationPolicy=RegexCompilerDefaultCreationPolicy
	>
	class RegexCompilerBase {
	
	
		public:
		
		
			typedef TElement ElementType;
			typedef TParser ParserType;
			typedef std::unique_ptr<TElement> Element;
			typedef std::vector<Element> Elements;
			typedef const CodePoint * Iterator;
			
			
		private:
			
			
			void complete () {
			
				if (last==nullptr) return;
				
				last->Complete(*(elements.back().get()));
				last=nullptr;
			
			}
			
			
			template <typename T>
			bool is_next (const T * str) noexcept {
			
				if (str==nullptr) return true;
				
				auto & self=*this;
				
				if (!self) return *str==0;
				
				for (;(*str!=0) && (*str==*self) && self;++str,++self);
				
				return *str==0;
			
			}
			
			
			template <typename T>
			bool is_next (const T * str, bool rewind) noexcept {
			
				auto loc=Current;
				auto result=is_next(str);
				if (!result || rewind) Current=loc;
				
				return result;
			
			}
	
	
		protected:
		

			Elements elements;
			const TParser * last;
			const TParser * current;
			
			
			RegexCompilerBase (Iterator current, Iterator begin, Iterator end) noexcept
				:	last(nullptr),
					current(nullptr),
					Current(current),
					Begin(begin),
					End(end),
					Successive(false)
			{	}
			
			
		public:
		
		
			/**
			 *	The current location within the string being compiled.
			 */
			Iterator Current;
			/**
			 *	The beginning of the string being compiled.
			 */
			Iterator Begin;
			/**
			 *	The end of the string being compiled.
			 */
			Iterator End;
			
			
			/**
			 *	\em true if the currently executing parser is
			 *	being invoked at a point in the string immediately
			 *	after a point at which it already successfully
			 *	parsed, \em false otherwise.
			 */
			bool Successive;
			
			
			virtual ~RegexCompilerBase () noexcept {	}
			
			
			/**
			 *	Checks to see if the end of the string has been
			 *	reached.
			 *
			 *	\return
			 *		\em false if the end of the string has been
			 *		reached, \em true otherwise.
			 */
			operator bool () const noexcept {
			
				return Current!=End;
			
			}
			/**
			 *	Obtains the current code point from the string.
			 *
			 *	\return
			 *		A reference to a code point.
			 */
			const CodePoint & operator * () const noexcept {
			
				return *Current;
			
			}
			/**
			 *	Obtains a pointer to the current code point from
			 *	the string.
			 *
			 *	\return
			 *		A pointer to a code point.
			 */
			const CodePoint * operator -> () const noexcept {
			
				return Current;
			
			}
			/**
			 *	Advances the compiler to the next code point within
			 *	the string.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerBase & operator ++ () noexcept {
			
				++Current;
				
				return *this;
			
			}
			/**
			 *	Backs the compiler up to the last code point within
			 *	the string.
			 *
			 *	\return
			 *		A reference to this object.
			 */
			RegexCompilerBase & operator -- () noexcept {
			
				--Current;
				
				return *this;
			
			}
			
			
			/**
			 *	Determines if a certain code point is next in the string.
			 *
			 *	If the code point is found, the compiler's location will
			 *	be advanced by one, otherwise the compiler's location
			 *	will not change.
			 *
			 *	\param [in] cp
			 *		A Unicode code point.
			 *	\param [in] rewind
			 *		If \em true the compiler will be rewound after
			 *		checking, even if the check is successful.  Defaults
			 *		to \em false.
			 *
			 *	\return
			 *		\em true if \em cp was next, \em false otherwise.
			 */
			bool IsNext (CodePoint cp, bool rewind=false) noexcept {
			
				auto & self=*this;
				
				if (self && (*self==cp)) {
				
					if (!rewind) ++self;
					
					return true;
				
				}
				
				return false;
			
			}
			/**
			 *	Determines if a certain string is next in the string.
			 *
			 *	If the string is found, the compiler's location will
			 *	be advanced to one past the end of the string, otherwise
			 *	the compiler's location will not change.
			 *
			 *	\param [in] str
			 *		A null-terminated string of Unicode code points.
			 *	\param [in] rewind
			 *		If \em true the compiler will be rewound after
			 *		checking, even if the check is successful.  Defaults
			 *		to \em false.
			 *
			 *	\return
			 *		\em true if \em str was next in the string,
			 *		\em false otherwise.
			 */
			bool IsNext (const char32_t * str, bool rewind=false) noexcept {
			
				return is_next(str,rewind);
			
			}
			/**
			 *	Determines if a certain string is next in the string.
			 *
			 *	If the string is found, the compiler's location will
			 *	be advanced to one past the end of the string, otherwise
			 *	the compiler's location will not change.
			 *
			 *	\param [in] str
			 *		A null-terminated string of Unicode code points.
			 *	\param [in] rewind
			 *		If \em true the compiler will be rewound after
			 *		checking, even if the check is successful.  Defaults
			 *		to \em false.
			 *
			 *	\return
			 *		\em true if \em str was next in the string,
			 *		\em false otherwise.
			 */
			bool IsNext (const char * str, bool rewind=false) noexcept {
			
				return is_next(str,rewind);
			
			}
			
			
			/**
			 *	Retrieves and clears the compiled elements.
			 *
			 *	\return
			 *		The compiled elements.
			 */
			Elements Get () {
			
				complete();
				
				auto retr=std::move(elements);
				elements=Elements{};
				last=nullptr;
				
				return retr;
			
			}
			/**
			 *	Clears and sets the collection of compiled elements.
			 */
			void Set (Elements elements) {
			
				this->elements=std::move(elements);
				last=nullptr;
			
			}
			
			
			/**
			 *	Determines the number of elements stored within
			 *	this compiler.
			 *
			 *	\return
			 *		The number of TElements in the collection of
			 *		elements the compiler is building.
			 */
			std::size_t Size () const noexcept {
			
				return elements.size();
			
			}
			
			
			/**
			 *	Retrieves the rear element and removes
			 *	it from the internal collection of elements.
			 *
			 *	If the compiler contains no elements,
			 *	behaviour is undefined.
			 *
			 *	\return
			 *		The rear element.
			 */
			Element Pop () {
			
				auto back=elements.end()-1;
				
				auto extracted=last->GetLast(*(*back));
				
				complete();
				
				if (extracted) return std::move(extracted);
				
				auto retr=std::move(*back);
				elements.erase(back);
				
				return retr;
			
			}
			
			
			/**
			 *	Retrieves the last compiled element cast to a certain
			 *	type.
			 *
			 *	If there are no compiled TElements, or the last TElement
			 *	is not of type \em T, the behaviour is undefined.
			 *
			 *	\tparam T
			 *		The type to which the last TElement shall be cast.
			 *
			 *	\return
			 *		A reference to the last TElement cast
			 *		to type \em T.
			 */
			template <typename T>
			T & Back () noexcept {
			
				return reinterpret_cast<T &>(*(elements.back().get()));
			
			}
			
			
			/**
			 *	Creates a new element of a certain type according to
			 *	CreationPolicy.
			 *
			 *	\tparam T
			 *		The type of TElement to create.
			 *	\tparam Args
			 *		The types of the arguments to forward through
			 *		to the constructor of \em T.
			 *
			 *	\param [in] args
			 *		The arguments of types \em Args which shall be
			 *		forwarded through to the constructor of \em T.
			 *
			 *	\return
			 *		An Element.
			 */
			template <typename T, typename... Args>
			Element Create (Args &&... args) {
			
				return Element(
					CreationPolicy::template Create<T>(
						*this,
						std::forward<Args>(args)...
					)
				);
			
			}
			
			
			/**
			 *	Adds an Element to the back of the internal collection.
			 *
			 *	\param [in] element
			 *		An Element to add.
			 */
			void Add (Element element) {
			
				complete();
			
				elements.push_back(std::move(element));
				
				last=current;
			
			}
			
			
			/**
			 *	Adds a TElement of a certain type to the back of the
			 *	collection of elements.
			 *
			 *	The TElement will be obtained through a call to
			 *	Create.
			 *
			 *	\tparam T
			 *		The type of TElement to add.
			 *	\tparam Args
			 *		The types of the arguments to forward through
			 *		to Create of \em T.
			 *
			 *	\param [in] args
			 *		The arguments of types \em Args which shall
			 *		be forwarded through to Create.
			 *
			 *	\return
			 *		A reference to the newly-created TElement.
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
			void Raise (const char * what) const {
			
				throw RegexError(what,Current);
			
			}
			
			
			/**
			 *	Checks to see if compilation is finished at this
			 *	point in the string.
			 *
			 *	Default implementation returns \em true when
			 *	operator bool returns \em false.
			 *
			 *	If an override of this method does not return \em true
			 *	when operator bool returns \em false, the behaviour is
			 *	undefined.
			 *
			 *	\return
			 *		\em true if compilation is finished, \em false
			 *		otherwise.
			 */
			virtual bool Done () {
			
				return !*this;
			
			}
			
			
			/**
			 *	Performs compilation.
			 */
			void operator () () {
			
				auto & parsers=AcquisitionPolicy::Get();
				
				//	Loop until done
				while (!Done()) {
				
					//	Track start position so that we can
					//	rewind on failure
					auto start=Current;
					
					//	Loop over each parser
					for (auto & parser : parsers) {
					
						current=AcquisitionPolicy::Get(parser);
						Successive=current==last;
						if (InvocationPolicy::Invoke(*this,parser)) {
						
							//	SUCCESS
							
							//	If the parser didn't advance the compiler's
							//	position, do it
							if (Current==start) ++Current;
							
							goto success;
						
						}
						
						//	Rewind to try again
						Current=start;
					
					}
					
					//	If no parser could be found to parse at this
					//	location, throw
					Raise("No matching parser");
					
					success:;
				
				}
				
				//	Make sure everything is wrapped up
				complete();
			
			}
	
	
	};


}
