#include <unicode/ascii.hpp>


namespace Unicode {


	void ASCII::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		buffer.push_back(static_cast<unsigned char>(cp));
	
	}
	
	
	std::optional<EncodingErrorType> ASCII::Decoder (CodePoint & cp, const unsigned char * & begin, const unsigned char * end, std::optional<Unicode::Endianness>) const {
	
		if ((*begin>128) && !Strict.Ignored()) return EncodingErrorType::Strict;
		
		cp=*begin;
		
		return std::nullopt;
	
	}
	
	
	ASCII::ASCII () noexcept : Encoding(false,false) {	}
	
	
	bool ASCII::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<128;
	
	}
	
	
	std::size_t ASCII::Count (CodePoint cp) const noexcept {
	
		return CanRepresent(cp) ? 1 : 0;
	
	}
	
	
	std::size_t ASCII::Size () const noexcept {
	
		return sizeof(CodeUnit);
	
	}


}
