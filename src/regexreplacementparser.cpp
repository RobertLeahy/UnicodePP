#include <unicode/regexreplacement.hpp>


namespace Unicode {


	//	Empty implementation of virtual base destructor
	RegexReplacementParser::~RegexReplacementParser () noexcept {	}
	
	
	void RegexReplacementParser::Complete (RegexReplacementElement &) const {	}
	
	
	RegexReplacementCompiler::Element RegexReplacementParser::GetLast (RegexReplacementElement &) const {
	
		return RegexReplacementCompiler::Element{};
	
	}


}
