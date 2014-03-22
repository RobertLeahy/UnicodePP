#include <unicode/utf32.hpp>


namespace Unicode {


	static const unsigned char bom_array []={0,0,0xFE,0xFF};
	static const ByteOrderMark bom(bom_array);
	
	
	void UTF32::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		Get(buffer,static_cast<CodeUnit>(cp));
	
	}
	
	
	std::optional<EncodingErrorType> UTF32::Decoder (
		CodePoint & cp,
		const unsigned char * & begin,
		const unsigned char * end,
		std::optional<Unicode::Endianness> order
	) const {
	
		//	Fallback if byte order was not inferred
		if (!order) {
		
			//	Fail on no fallback
			if (!InputOrder) return EncodingErrorType::Endianness;
			
			order=*InputOrder;
		
		}
		
		//	Get next code point/unit
		auto cu=Get<CodeUnit>(begin,end,*order);
		if (!cu) return EncodingErrorType::UnexpectedEnd;
		
		cp=*cu;
		
		return std::nullopt;
	
	}


	ByteOrderMark UTF32::BOM () const noexcept {
	
		return bom;
	
	}
	
	
	bool UTF32::CanRepresent (CodePoint cp) const noexcept {
	
		return true;
	
	}
	
	
	std::size_t UTF32::Count (CodePoint cp) const noexcept {
	
		return 1;
	
	}
	
	
	std::size_t UTF32::Size () const noexcept {
	
		return sizeof(CodeUnit);
	
	}


}
