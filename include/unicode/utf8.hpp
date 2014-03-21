/**
 *	\file
 */
 
 
#pragma once


#include <unicode/encoding.hpp>


namespace Unicode {


	/**
	 *	The UTF-8 multi-byte encoding.
	 */
	class UTF8 : public Encoding {
	
	
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
			 *	encoder.
			 */
			typedef unsigned char CodeUnit;
		
		
			/**
			 *	Creates a new UTF8 encoder/decoder.
			 *
			 *	By default UTF8 encoder/decoders do not
			 *	detect or output a byte order mark, as
			 *	recommended by the Unicode consortium.
			 */
			UTF8 () noexcept;
			
			
			virtual ByteOrderMark BOM () const noexcept override;
			virtual bool CanRepresent (CodePoint) const noexcept override;
			virtual std::size_t Count (CodePoint) const noexcept override;
	
	
	};


}
