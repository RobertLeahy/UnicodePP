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
	
	
		public:
		
		
			/**
			 *	A type which represents a UTF-8
			 *	code unit.
			 */
			typedef unsigned char CodeUnit;
	
	
		private:
		
		
			void to_bytes (std::vector<char> &, CodePoint) const;
			bool from_bytes (std::vector<CodePoint> &, const CodeUnit * &, const CodeUnit *) const;
			
			
		protected:
		
		
			virtual void Encoder (std::vector<char> &, const CodePoint *, const CodePoint *) const override;
			virtual void Decoder (std::vector<CodePoint> &, const void *, const void *) const override;
	
	
		public:
		
		
			/**
			 *	The largest value that can be represented
			 *	as UTF-8.
			 */
			static constexpr CodePoint::Type Max=0x7FFFFFFFU;
			
			
			virtual bool CanRepresent (CodePoint) const noexcept override;
			virtual std::size_t Count (CodePoint) const noexcept override;
	
	
	};


}
