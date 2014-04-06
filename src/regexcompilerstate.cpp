#include <unicode/regexcompiler.hpp>


namespace Unicode {


	//	Empty implementation of virtual destructor
	RegexCompilerState::~RegexCompilerState () noexcept {	}
	
	
	bool RegexCompilerState::Done () {
	
		return !static_cast<bool>(*this);
	
	}


}
