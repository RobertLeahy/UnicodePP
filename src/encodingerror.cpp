#include <unicode/encoding.hpp>


namespace Unicode {


	EncodingError::EncodingError (EncodingErrorType type, const void * where, const char * what) noexcept
		:	Error(what),
			t(type),
			w(where)
	{	}
	
	
	const void * EncodingError::where () const noexcept {
	
		return w;
	
	}
	
	
	EncodingErrorType EncodingError::type () const noexcept {
	
		return t;
	
	}


}
