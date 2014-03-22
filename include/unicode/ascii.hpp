/**
 *	\file
 */
 
 
#pragma once


#include <unicode/encoding.hpp>


namespace Unicode {


	/**
	 *	An encoder/decoder which transforms Unicode text
	 *	as best as possible between Unicode and ASCII.
	 */
	class ASCII : public Encoding {
	
	
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
		
		
			typedef unsigned char CodeUnit;
			
			
			ASCII () noexcept;
			
			
			virtual bool CanRepresent (CodePoint) const noexcept override;
			virtual std::size_t Count (CodePoint) const noexcept override;
			virtual std::size_t Size () const noexcept;
	
	
	};


}
