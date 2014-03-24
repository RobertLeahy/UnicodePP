#include <unicode/ucs2.hpp>


namespace Unicode {


	static const unsigned char bom_array []={0xFE,0xFF};
	static const ByteOrderMark bom(bom_array);


	void UCS2::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		Get(buffer,static_cast<CodeUnit>(cp));
	
	}
	
	
	std::optional<EncodingErrorType> UCS2::Decoder (
		CodePoint & cp,
		const unsigned char * & begin,
		const unsigned char * end,
		std::optional<Unicode::Endianness> order
	) const {
	
		//	Fallback if byte order unspecified
		if (!order) {
		
			//	Fail if there's no fallback byte order
			if (!InputOrder) return EncodingErrorType::Endianness;
			
			order=*InputOrder;
		
		}
		
		//	Get next code unit
		auto cu=Get<CodeUnit>(begin,end,*order);
		if (!cu) return EncodingErrorType::UnexpectedEnd;
		
		cp=*cu;
		
		return std::nullopt;
	
	}
	
	
	ByteOrderMark UCS2::BOM () const noexcept {
	
		return bom;
	
	}
	
	
	bool UCS2::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<=0xFFFFU;
	
	}
	
	
	std::size_t UCS2::Count (CodePoint cp) const noexcept {
	
		return CanRepresent(cp) ? 1 : 0;
	
	}
	
	
	std::size_t UCS2::Size () const noexcept {
	
		return sizeof(CodeUnit);
	
	}


}
