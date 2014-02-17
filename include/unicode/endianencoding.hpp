/**
 *	\file
 */
 
 
#pragma once


#include <unicode/encoding.hpp>
#include <algorithm>
#include <iterator>


namespace Unicode {


	/**
	 *	The base class for all encodings which
	 *	have endianness.
	 */
	class EndianEncoding : public Encoding {
	
	
		public:
		
		
			/**
			 *	Detects this system's endianness.
			 *
			 *	\return
			 *		The endianness of this system.
			 */
			static Endianness GetEndianness () noexcept {
			
				return
				#if __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__
				Endianness::Little
				#else
				Endianness::Big
				#endif
				;
			
			}
			
			
		private:
		
		
			Endianness get_fallback (const void *) const;
	
	
		protected:
		
		
			template <typename T>
			static void Correct (T & obj, Endianness order) noexcept {
			
				if (order==GetEndianness()) return;
				
				union {
					T in;
					unsigned char bytes [sizeof(in)];
				};
				in=obj;
				std::reverse(std::begin(bytes),std::end(bytes));
				
				obj=in;
			
			}
			
			
			Endianness GetEndianness (const void * & begin, const void * end, ByteOrderMark bom) const;
	
	
		public:
		
		
			/**
			 *	If \em true the encoder will attempt
			 *	to detect the endianness when decoding.
			 */
			bool Detect;
			/**
			 *	If present, the endianness that should be
			 *	used when decoding.  If not present, and
			 *	Detect is \em false, or the endianness
			 *	could not be detected, an exception will
			 *	be thrown.
			 */
			std::optional<Endianness> DecodeOrder;
			/**
			 *	The endianness that should be used when
			 *	encoding.
			 */
			Endianness EncodeOrder;
			
			
			/**
			 *	Creates a new encoding.
			 *
			 *	\param [in] order
			 *		The byte order that should be used for
			 *		both encoding and decoding.  Defaults to
			 *		big endian.
			 */
			EndianEncoding (Endianness order=Endianness::Big) noexcept;
	
	
	};


}
 