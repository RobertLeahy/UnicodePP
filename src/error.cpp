#include <unicode/error.hpp>


namespace Unicode {


	Error::Error (const char * what) noexcept : w(what) {	}
	
	
	const char * Error::what () const noexcept {
	
		return w;
	
	}


}
