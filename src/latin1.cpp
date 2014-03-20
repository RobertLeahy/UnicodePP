#include <unicode/latin1.hpp>
#include <limits>


namespace Unicode {


	void Latin1::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		buffer.push_back(static_cast<unsigned char>(cp));
	
	}
	
	
	std::optional<EncodingErrorType> Latin1::Decoder (CodePoint & cp, const unsigned char * & begin, const unsigned char * end, std::optional<Unicode::Endianness>) const {
	
		cp=*begin;
		
		return std::nullopt;
	
	}
	
	
	Latin1::Latin1 () noexcept : Encoding(false,false) {	}
	
	
	ByteOrderMark Latin1::BOM () const noexcept {
	
		return ByteOrderMark{};
	
	}
	
	
	bool Latin1::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<=std::numeric_limits<unsigned char>::max();
	
	}
	
	
	std::size_t Latin1::Count (CodePoint cp) const noexcept {
	
		return CanRepresent(cp) ? 1 : 0;
	
	}


}
