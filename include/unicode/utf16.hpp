/**
 *	\file
 */
 
 
#pragma once


#include <unicode/endianencoding.hpp>


namespace Unicode {


	/**
	 *	The UTF-16 encoding.
	 */
	class UTF16 : public EndianEncoding {
	
	
		protected:
		
		
			virtual void Encoder (
				std::vector<unsigned char> & buffer,
				CodePoint cp
			) const override;
			
			virtual std::optional<EncodingErrorType> Decoder (
				CodePoint & cp,
				const unsigned char * & begin,
				const unsigned char * end,
				std::optional<Unicode::Endianness> order
			) const override;
	
	
		public:
		
		
			/**
			 *	The type of code unit used by this
			 *	encoding.  Unsigned integer exactly
			 *	16 bits wide.
			 */
			typedef char16_t CodeUnit;
		
		
			using EndianEncoding::EndianEncoding;
			
			
			virtual ByteOrderMark BOM () const noexcept override;
			virtual bool CanRepresent (CodePoint) const noexcept override;
			virtual std::size_t Count (CodePoint) const noexcept override;
	
	
	};


}
