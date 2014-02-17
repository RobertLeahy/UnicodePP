/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <cstddef>


namespace Unicode {


	class Grapheme {
	
	
		private:
		
		
			enum class Mode {
				
				NonOwning,
				Owning,
				Single
				
			};
			
			
			Mode mode;
		
		
			union {
				CodePoint * ptr;
				CodePoint cp;
			};
			
			
			std::size_t count;
			
			
			void allocate ();
			void destroy () noexcept;
			void copy (const Grapheme &);
			void move (Grapheme &&) noexcept;
			
			
		public:
		
		
			Grapheme () noexcept;
			~Grapheme () noexcept;
			Grapheme (const Grapheme &);
			Grapheme (Grapheme &&) noexcept;
			Grapheme & operator = (const Grapheme &);
			Grapheme & operator = (Grapheme &&) noexcept;
			
			
			Grapheme (char c) noexcept;
			
			
			CodePoint * begin () noexcept;
			const CodePoint * begin () const noexcept;
			CodePoint * end () noexcept;
			const CodePoint * end () const noexcept;
	
	
	};


}
