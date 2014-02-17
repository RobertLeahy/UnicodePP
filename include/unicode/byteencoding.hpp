/**
 *	\file
 */
 
 
#pragma once


#include <unicode/encoding.hpp>


namespace Unicode {


	/**
	 *	A base class for encodings with a code unit
	 *	size of one.  I.e. encodings which do not have
	 *	endianness concerns.
	 */
	class ByteEncoding : public Encoding {
	
	
		protected:
		
		
			void SkipBOM (const void * &, const void *, ByteOrderMark) const noexcept;
	
	
		public:
		
		
			/**
			 *	If \em true the encoder will disregard
			 *	the BOM at the beginning of byte streams.
			 */
			bool IgnoreBOM;
			
			
			ByteEncoding () noexcept;
	
	
	};


}
