#include <unicode/utf32.hpp>
#include <cstring>
#include <iterator>


namespace Unicode {


	static const unsigned char bom_bytes []={0,0,0xFEU,0xFFU};
	static const ByteOrderMark bom(
		std::begin(bom_bytes),
		std::end(bom_bytes)
	);


	bool UTF32::from_bytes (const unsigned char * & begin, const unsigned char * end, CodeUnit & cu, Endianness order) const noexcept {
	
		if (static_cast<std::size_t>(end-begin)<sizeof(cu)) return false;
		
		std::memcpy(
			&cu,
			begin,
			sizeof(cu)
		);
		
		begin+=sizeof(cu);
		
		Correct(cu,order);
		
		return true;
	
	}


	void UTF32::Encoder (std::vector<char> & buffer, const CodePoint * begin, const CodePoint * end) const {
	
		
	
	}
	
	
	void UTF32::Decoder (std::vector<CodePoint> & cps, const void * begin, const void * end) const {
	
		auto order=GetEndianness(begin,end,bom);
		
		auto b=reinterpret_cast<const unsigned char *>(begin);
		auto e=reinterpret_cast<const unsigned char *>(end);
		
		CodeUnit cu;
		while (from_bytes(b,e,cu,order)) {
		
			
		
		}
	
	}
	
	
	bool UTF32::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<Max;
	
	}
	
	
	std::size_t UTF32::Count (CodePoint cp) const noexcept {
	
		return sizeof(CodeUnit);
	
	}


}
