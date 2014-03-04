/**
 *	\file
 */
 
 
#pragma once


#include <unicode/endianencoding.hpp>
#include <unicode/string.hpp>
#include <unicode/utf8.hpp>
#include <unicode/utf16.hpp>
#include <unicode/utf32.hpp>
#include <cstddef>
#include <iostream>


namespace Unicode {


	/**
	 *	\cond
	 */


	namespace OutputStream {
	
	
		template <typename CharT>
		std::vector<unsigned char> Encode (const String & str) {
		
			constexpr std::size_t size=sizeof(CharT);
		
			static_assert(
				(size==1) || (size==2) || (size==4),
				"CharT size incompatible"
			);
		
			auto s=str.ToNFC();
			
			switch (size) {
			
				case 1:
					return UTF8{}.Encode(s);
				case 2:{
				
					UTF16 encoder(EndianEncoding::Detect());
					encoder.OutputBOM=false;
					
					return encoder.Encode(s);
				
				}
				case 4:{
				
					UTF32 encoder(EndianEncoding::Detect());
					encoder.OutputBOM=false;
					
					return encoder.Encode(s);
				
				}
				//	This path should never be taken
				//	in practice (static_assert above
				//	prevents it)
				default:
					return std::vector<unsigned char>{};
			
			}
		
		}
	
	
	}
	
	
	/**
	 *	\endcond
	 */


	/**
	 *	Outputs a Unicode string to an output stream.
	 *
	 *	\param [in] os
	 *		The output stream.
	 *	\param [in] str
	 *		The Unicode string.
	 *
	 *	\return
	 *		\em os.
	 */
	template <typename CharT, typename Traits>
	std::basic_ostream<CharT,Traits> & operator << (std::basic_ostream<CharT,Traits> & os, const String & str) {
	
		auto vec=OutputStream::Encode<CharT>(str);
		os.write(
			reinterpret_cast<const CharT *>(vec.data()),
			vec.size()/sizeof(CharT)
		);
		
		return os;
	
	}


}
