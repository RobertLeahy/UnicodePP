#include <unicode/encoding.hpp>
#include <type_traits>


namespace Unicode {


	static const unsigned char * to_byte (const void * ptr) noexcept {
	
		return reinterpret_cast<const unsigned char *>(ptr);
	
	}


	std::optional<Endianness> ByteOrderMark::Detect (const void * & begin, const void * end) const noexcept {
	
		std::optional<Endianness> retr;
		
		auto b=to_byte(begin);
		auto e=to_byte(end);
		auto c=Count();
		if (static_cast<std::size_t>(e-b)<c) return retr;
		e=b+c;
		
		if (std::equal(b,e,this->b,this->e)) retr=Endianness::Big;
		else if (std::equal(b,e,reverse(this->e),reverse(this->b))) retr=Endianness::Little;
		
		if (retr) begin=e;
		
		return retr;
	
	}


}
