#include <unicode/utf8.hpp>


namespace Unicode {


	static const unsigned char bom_array []={0xEF,0xBB,0xBF};
	static const ByteOrderMark bom(bom_array);


	UTF8::UTF8 () noexcept : Encoding(false,false) {	}
	
	
	//	Creates a leading byte from a code point
	static unsigned char get_leading_byte (std::size_t len, CodePoint::Type cp) noexcept {
	
		unsigned char retr=0;
		for (std::size_t i=0;i<len;++i) {
		
			retr>>=1;
			retr|=128;
		
		}
		
		retr|=static_cast<unsigned char>(cp>>(6*(len-1)));
		
		return retr;
	
	}
	
	
	//	Creates a continuation byte from a code point
	static unsigned char get_continuation_byte (std::size_t i, CodePoint::Type cp) noexcept {
	
		return static_cast<unsigned char>(
			(cp>>(6*(i-1)))&static_cast<CodePoint::Type>(0x3F)
		)|static_cast<unsigned char>(128);
	
	}
	
	
	void UTF8::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		auto len=Count(cp);
		
		//	This shouldn't happen
		if (len==0) return;
		
		//	Trivial
		if (len==1) {
		
			buffer.push_back(static_cast<unsigned char>(cp));
			
			return;
		
		}
		
		//	Multi-byte
		buffer.push_back(get_leading_byte(len,cp));
		while ((len--)>1) buffer.push_back(get_continuation_byte(len,cp));
	
	}
	
	
	//	Gets the number of bytes in the UTF-8 encoding
	//	of a code point, based on the leading byte
	static std::size_t count (unsigned char b) noexcept {
	
		unsigned char mask=128;
		//	Check the high bit, if it's not set
		//	it's just 1 byte
		if ((b&mask)==0) return 1;
		
		//	Count the number of high bits set
		std::size_t retr=1;
		for (
			mask>>=1;
			!((mask==0) || ((b&mask)==0));
			mask>>=1,++retr
		);
		
		//	Sift out bad return values
		//
		//	UTF-8 can't by more than 6 bytes long,
		//	and 1 byte at this point wouldn't make
		//	sense, that would indicate a continuation
		//	byte where one shouldn't be
		return ((retr==1) || (retr>6)) ? 0 : retr;
	
	}
	
	
	//	Gets the contribution to the final code point
	//	from the leading byte
	static CodePoint::Type get_leading_byte (std::size_t len, unsigned char b) noexcept {
	
		//	Determine the number of low bits we
		//	need to mask off
		len=8-1-len;
		
		unsigned char mask=0;
		for (std::size_t i=0;i<len;++i) {
		
			mask<<=1;
			mask|=static_cast<unsigned char>(1);
		
		}
		
		return b&mask;
	
	}
	
	
	//	Gets the contribution to the final code point
	//	from a continuation byte
	static std::optional<CodePoint::Type> get_continuation_byte (CodePoint::Type curr, unsigned char b) noexcept {
	
		//	Check to make sure the high bit is
		//	set and the second highest bit is
		//	not set
		if ((b&static_cast<unsigned char>(192))!=128) return std::optional<CodePoint::Type>{};
		
		curr<<=6;
		curr|=static_cast<CodePoint::Type>(static_cast<unsigned char>(63)&b);
		
		return curr;
	
	}
	
	
	std::optional<EncodingErrorType> UTF8::Decoder (std::vector<CodePoint> & cps, const unsigned char * & begin, const unsigned char * end, std::optional<Endianness>) const {
	
		//	Determine the number of bytes in this sequence
		auto len=count(*begin);
		
		//	Invalid sequence
		if (len==0) return EncodingErrorType::Strict;
		
		//	Trivial
		if (len==1) {
		
			cps.push_back(static_cast<CodePoint::Type>(*(begin++)));
			
			return std::optional<EncodingErrorType>{};
		
		}
		
		//	Perform length check
		if (static_cast<std::size_t>(end-begin)<len) return EncodingErrorType::UnexpectedEnd;
		
		CodePoint::Type cp=get_leading_byte(len,*(begin++));
		for (std::size_t i=0;i<(len-1);++i) {
		
			auto cp_o=get_continuation_byte(cp,*(begin++));
			if (!cp_o) return EncodingErrorType::Strict;
			cp=*cp_o;
		
		}
		
		cps.push_back(cp);
	
		return std::optional<EncodingErrorType>{};
	
	}
	
	
	ByteOrderMark UTF8::BOM () const noexcept {
	
		return bom;
	
	}
	
	
	bool UTF8::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<=0x7FFFFFFF;
	
	}
	
	
	std::size_t UTF8::Count (CodePoint cp) const noexcept {
	
		if (cp<=0x7F) return 1;
		if (cp<=0x7FF) return 2;
		if (cp<=0xFFFF) return 3;
		if (cp<=0x1FFFFF) return 4;
		if (cp<=0x3FFFFFF) return 5;
		if (cp<=0x7FFFFFFF) return 6;
		
		return 0;
	
	}


}
