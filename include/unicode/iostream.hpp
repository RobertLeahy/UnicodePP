/**
 *	\file
 */
 
 
#pragma once


#include <unicode/boundaries.hpp>
#include <unicode/encoding.hpp>
#include <unicode/string.hpp>
#include <unicode/utf8.hpp>
#include <unicode/utf16.hpp>
#include <unicode/utf32.hpp>
#include <unicode/vector.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>


namespace Unicode {


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
	
		return os << str.ToString<CharT>();
	
	}
	
	
	/**
	 *	Outputs a Unicode code point to an output stream.
	 *
	 *	\param [in] os
	 *		The output stream.
	 *	\param [in] cp
	 *		The code point.
	 *
	 *	\return
	 *		\em os.
	 */
	template <typename CharT, typename Traits>
	std::basic_ostream<CharT,Traits> & operator << (std::basic_ostream<CharT,Traits> & os, CodePoint cp) {
	
		String s;
		s << cp;
		os << s;
		
		return os;
	
	}
	
	
	/**
	 *	\cond
	 */
	 
	 
	//	Wraps an input stream and extracts strings
	//	line-by-line, or single code points
	template <typename CharT, typename Traits>
	class InputStreamWrapper {
	
	
		private:
		
		
			//	Unparsed/-decoded characters
			std::vector<CharT> buffer;
			//	Encoder/decoder
			std::unique_ptr<Encoding> encoder;
			
			
			[[noreturn]]
			static void raise (std::basic_istream<CharT,Traits> &) {
			
				throw std::runtime_error("Stream error");
			
			}
			
			
			bool fetch (std::basic_istream<CharT,Traits> & is) {
			
				CharT c;
				is.get(c);
				
				if (!is) {
				
					if (is.bad()) raise(is);
					
					is.clear();
					is.setstate(std::ios_base::eofbit);
				
					return false;
				
				}
				
				buffer.push_back(c);
				
				return true;
			
			}
			
			
			static bool is_next_lf (std::basic_istream<CharT,Traits> & is) {
			
				auto c=is.peek();
				
				if (is.eof()) return false;
				
				if (!is) raise(is);
				
				return c=='\n';
			
			}
			
			
			std::optional<CodePoint> get_code_point (std::basic_istream<CharT,Traits> & is) {
			
				buffer.clear();
				
				//	Loop until end of stream or a code point
				//	is extracted
				while (fetch(is)) {
				
					const void * begin=Begin(buffer);
					auto cp=encoder->Stream(
						begin,
						End(buffer),
						EndianEncoding::Detect()
					);
					
					if (cp) return cp;
				
				}
				
				//	If we get down here, end of stream was
				//	encountered, which means if the underlying
				//	stream was well-formed, there should be
				//	nothing in our buffer
				if (buffer.size()!=0) encoder->UnexpectedEnd.Throw(nullptr);
				
				return std::nullopt;
				
			}
			
			
		public:
		
		
			InputStreamWrapper () {
			
				//	Select an appropriate encoding
				switch (sizeof(CharT)) {
				
					case sizeof(UTF8::CodeUnit):
						encoder=std::unique_ptr<Encoding>(new UTF8());
						break;
					case sizeof(UTF16::CodeUnit):
						encoder=std::unique_ptr<Encoding>(new UTF16());
						break;
					case sizeof(UTF32::CodeUnit):
						encoder=std::unique_ptr<Encoding>(new UTF32());
						break;
					default:
						throw std::logic_error("No encoding for code units of that size");
				
				}
			
			}
			
			
			std::optional<CodePoint> GetCodePoint (std::basic_istream<CharT,Traits> & is) {
			
				if (is.eof()) {
				
					is.setstate(std::ios_base::failbit);
					
					return std::nullopt;
				
				}
			
				return get_code_point(is);
			
			}
		
		
			String GetLine (std::basic_istream<CharT,Traits> & is) {
			
				if (is.eof()) {
				
					is.setstate(std::ios_base::failbit);
					
					return String{};
				
				}
				
				//	The string that we'll be returning
				std::vector<CodePoint> cps;
				
				for (;;) {
				
					auto cp=get_code_point(is);
					
					//	If the end of stream is reached, that's
					//	the end of this line
					if (!cp) return cps;
					
					//	Special handling for CR
					if (*cp=='\r') {
					
						if (is_next_lf(is)) continue;
						
						return cps;
					
					}
					
					cps.push_back(*cp);
					
					if (IsLineBreak(End(cps),Begin(cps),End(cps))) {
					
						cps.erase(cps.end()-1);
						
						return cps;
					
					}
				
				}
			
			}
	
	
	};
	
	
	/**
	 *	\endcond
	 */
	
	
	/**
	 *	Extracts a line from an input stream.
	 *
	 *	If the end of the stream has been reached, the
	 *	empty string will be returned.
	 *
	 *	\param [in] is
	 *		The input stream.
	 *	\param [out] str
	 *		The Unicode string.
	 *
	 *	\return
	 *		\em is.
	 */
	template <typename CharT, typename Traits>
	std::basic_istream<CharT,Traits> & operator >> (std::basic_istream<CharT,Traits> & is, String & str) {
	
		str=InputStreamWrapper<CharT,Traits>{}.GetLine(is);
		
		return is;
	
	}
	
	
	/**
	 *	Extracts a code point from an input stream.
	 *
	 *	\param [in] is
	 *		The input stream.
	 *	\param [out] cp
	 *		The code point.  If the end of \em is has
	 *		been reached, this code point will remain
	 *		untouched.
	 *
	 *	\return
	 *		\em is.
	 */
	template <typename CharT, typename Traits>
	std::basic_istream<CharT,Traits> & operator >> (std::basic_istream<CharT,Traits> & is, CodePoint & cp) {
	
		auto c=InputStreamWrapper<CharT,Traits>{}.GetCodePoint(is);
		
		if (c) cp=*c;
		
		return is;
	
	}


}
