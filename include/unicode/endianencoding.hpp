/**
 *	\file
 */
 
 
#pragma once


#include <unicode/encoding.hpp>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>


namespace Unicode {


	/**
	 *	The base class for all encodings to which
	 *	the concept of endianness is relevant.
	 */
	class EndianEncoding : public Encoding {
	
	
		public:
		
		
			/**
			 *	Detects the endianness of this machine.
			 *
			 *	\return
			 *		An Endianness enumeration value representing
			 *		the endianness of this machine.
			 */
			static constexpr Unicode::Endianness Detect () noexcept {
			
				#ifdef __BYTE_ORDER__
				
				#if __BYTE_ORDER__==__ORDER_BIG_ENDIAN__
				return Unicode::Endianness::Big;
				#elif __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__
				return Unicode::Endianness::Little;
				#else
				#error "Unknown endianness"
				#endif
				
				#else
				
				#error "__BYTE_ORDER__ is not defined"
				
				#endif
			
			}
			
			
			/**
			 *	Changes the endianness of an object.
			 *
			 *	\tparam T
			 *		The type of object whose endianness shall
			 *		be corrected.
			 *
			 *	\param [in] in
			 *		The object whose endianness shall be corrected.
			 *
			 *	\return
			 *		\em in with its endianness changed.
			 *
			 */
			template <typename T>
			static T Correct (const T & in) noexcept {
			
				union {
					T t;
					unsigned char buffer [sizeof(T)];
				};
				t=in;
				
				std::reverse(std::begin(buffer),std::end(buffer));
				
				return t;
			
			}
			/**
			 *	Corrects the endianness of an object.
			 *
			 *	\tparam T
			 *		The type of object whose endianness shall
			 *		be corrected.
			 *
			 *	\param [in] in
			 *		The object whose endianness shall be corrected.
			 *	\param [in] order
			 *		If \em in is a deserialized value, the endianness
			 *		of the stream from which it was extracted.  If
			 *		\em in is a value to be serialized, the endianness
			 *		of the stream to which it shall be serialized.
			 *
			 *	\return
			 *		\em in with its endianness changed if necessary.
			 */
			template <typename T>
			static T Correct (const T & in, Unicode::Endianness order) noexcept {
			
				return (order==Detect()) ? in : Correct(in);
			
			}
		
		
			/**
			 *	The endianness this encoder shall output.
			 */
			Unicode::Endianness OutputOrder;
			/**
			 *	The endianness this encoder shall accept as input
			 *	unless the byte order is detected from a BOM.
			 *
			 *	If this is disengaged, and no byte order is
			 *	detected, an endianness error will occur.
			 */
			std::optional<Unicode::Endianness> InputOrder;
			
			
		protected:
		
		
			template <typename T>
			void Get (std::vector<unsigned char> & cps, const T & cu) const {
			
				union {
					T in;
					unsigned char buffer [sizeof(T)];
				};
				in=Correct(cu,OutputOrder);
				
				cps.insert(cps.end(),std::begin(buffer),std::end(buffer));
			
			}
			
			
			template <typename T>
			static std::optional<T> Get (const unsigned char * & begin, const unsigned char * end, Unicode::Endianness order) noexcept {
			
				if (static_cast<std::size_t>(end-begin)<sizeof(T)) return std::optional<T>{};
				
				union {
					T out;
					unsigned char buffer [sizeof(T)];
				};
				
				for (auto & b : buffer) b=*(begin++);
				
				return Correct(out,order);
			
			}
			
			
			virtual void GetBOM (std::vector<unsigned char> & buffer) const override;
			
			
		public:
			
			
			/**
			 *	Creates a new EndianEncoding.
			 *
			 *	\param [in] output_order
			 *		The initial value of OutputOrder.
			 *	\param [in] input_order
			 *		The initial value of InputOrder.
			 */
			EndianEncoding (
				Unicode::Endianness output_order=Unicode::Endianness::Big,
				std::optional<Unicode::Endianness> input_order=Unicode::Endianness::Big) noexcept;
	
	
	};


}
