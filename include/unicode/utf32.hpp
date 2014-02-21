/**
 *	\file
 */
 
 
#pragma once


#include <unicode/endianencoding.hpp>
#include <limits>


namespace Unicode {


	class UTF32 : public EndianEncoding {
	
	
		public:
		
		
			typedef CodePoint::Type CodeUnit;
			
			
		private:
		
		
			bool from_bytes (const unsigned char * &, const unsigned char *, CodeUnit &, Endianness) const noexcept;
	
	
		protected:
		
		
			virtual void Encoder (std::vector<char> & buffer, const CodePoint * begin, const CodePoint * end) const override;
			virtual void Decoder (std::vector<CodePoint> & cps, const void * begin, const void * end) const override;
			
			
		public:
		
		
			static constexpr CodePoint::Type Max=std::numeric_limits<CodePoint::Type>::max();
		
		
			virtual bool CanRepresent (CodePoint cp) const noexcept override;
			virtual std::size_t Count (CodePoint cp) const noexcept override;
	
	
	};


}
