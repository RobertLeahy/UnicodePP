/**
 *	\file
 */
 
 
#pragma once
 
 
#include <unicode/codepoint.hpp>
#include <unicode/error.hpp>
 
 
namespace Unicode {


	/**
	 *	The type of exception that shall be thrown on a
	 *	regular expression engine error.
	 */
	class RegexError : public Error {
	
	
		private:
		
		
			const CodePoint * w;
			
			
		public:
		
		
			/**
			 *	Creates a new RegexError.
			 *
			 *	\param [in] what
			 *		A string describing the error that
			 *		was encountered.
			 *	\param [in] where
			 *		The location at which the error was
			 *		encountered.
			 */
			RegexError (const char * what, const CodePoint * where) noexcept;
			
			
			/**
			 *	Retrieves the location at which the error
			 *	occurred.
			 *
			 *	\return
			 *		An iterator to the location at which the
			 *		error was encountered.
			 */
			const CodePoint * where () const noexcept;
	
	
	};


}
