#include <unicode/byteencoding.hpp>


namespace Unicode {


	void ByteEncoding::SkipBOM (const void * & begin, const void * end, ByteOrderMark bom) const noexcept {
	
		if (!IgnoreBOM) bom.Detect(begin,end);
	
	}


	ByteEncoding::ByteEncoding () noexcept : IgnoreBOM(false) {	}


}
