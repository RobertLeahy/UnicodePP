#include <unicode/encoding.hpp>


namespace Unicode {


	std::optional<Endianness> ByteOrderMark::Detect (const void * & begin, const void * end) const noexcept {
	
		std::optional<Endianness> retr;
	
		auto b=reinterpret_cast<const unsigned char *>(begin);
		auto e=reinterpret_cast<const unsigned char *>(end);
		
		auto len=static_cast<std::size_t>(e-b);
		auto size=bom.size();
		if (len<size) return retr;
		e=b+size;
		
		auto b_b=bom.begin();
		auto b_e=bom.end();
		
		if (std::equal(b_b,b_e,b)) retr=Endianness::Big;
		else if (std::equal(
			b_b,
			b_e,
			MakeReverseIterator(e)
		)) retr=Endianness::Little;
		
		if (retr) begin=b+size;
		
		return retr;
	
	}


}
