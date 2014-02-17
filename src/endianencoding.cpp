#include <unicode/endianencoding.hpp>
#include <algorithm>
#include <iterator>


namespace Unicode {


	Endianness EndianEncoding::get_fallback (const void * where) const {
	
		if (!DecodeOrder) throw EncodingError(
			"No fallback byte order",
			where,
			EncodingErrorType::Strict
		);
		
		return *DecodeOrder;
	
	}


	Endianness EndianEncoding::GetEndianness (const void * & begin, const void * end, ByteOrderMark bom) const {

		if (!Detect) return get_fallback(begin);
		
		auto endianness=bom.Detect(begin,end);
		
		return endianness ? *endianness : get_fallback(begin);
	
	}


	EndianEncoding::EndianEncoding (Endianness order) noexcept
		:	Detect(false),
			DecodeOrder(order),
			EncodeOrder(order)
	{	}


}
