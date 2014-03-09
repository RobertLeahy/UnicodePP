#include <unicode/regexerror.hpp>


namespace Unicode {


	RegexError::RegexError (const char * what, const CodePoint * where) noexcept : Error(what), w(where) {	}
	
	
	const CodePoint * RegexError::where () const noexcept {
	
		return w;
	
	}


}
