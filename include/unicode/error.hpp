/**
 *	\file
 */
 
 
#pragma once


#include <exception>


namespace Unicode {


	/**
	 *	The type of exception that shall be thrown
	 *	on a Unicode error.
	 */
	class Error : public std::exception {
	
	
		private:
		
		
			const char * w;
			
			
		public:
		
		
			/** 
			 *	Creates a new Error.
			 *
			 *	\param [in] what
			 *		A string describing the error
			 *		that was encountered.
			 */
			Error (const char * what) noexcept;
			
			
			/**
			 *	Returns a string describing the error.
			 *
			 *	\return
			 *		A string describing the error.
			 */
			virtual const char * what () const noexcept override;
	
	
	};


}
