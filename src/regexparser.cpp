#include <unicode/regexcompiler.hpp>


namespace Unicode {


	//	Virtual destructor implementation, no body
	RegexParser::~RegexParser () noexcept {	}
	
	
	bool RegexParser::operator () (RegexPatternElement &, RegexCompilerState &) const {
	
		return false;
	
	}
	
	
	void RegexParser::Complete (RegexPatternElement &) const {	}


}
