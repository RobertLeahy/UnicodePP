#include <unicode/regexcompiler.hpp>


namespace Unicode {


	//	Virtual destructor implementation, no body
	RegexParser::~RegexParser () noexcept {	}
	
	
	void RegexParser::Complete (RegexPatternElement &) const {	}
	
	
	RegexCompiler::Element RegexParser::GetLast (RegexPatternElement &) const {
	
		return RegexCompiler::Element{};
	
	}


}
