#include <unicode/encoding.hpp>


namespace Unicode {


	EncodingError::EncodingError (const char * what, const void * where, EncodingErrorType type) noexcept : Error(what), w(where), t(type) {	}
	
	
	const void * EncodingError::where () const noexcept {
	
		return w;
	
	}
	
	
	EncodingErrorType EncodingError::type () const noexcept {
	
		return t;
	
	}


}
