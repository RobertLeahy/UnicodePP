#include <unicode/codepoint.hpp>


namespace Unicode {


	bool Condition::operator () (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) const noexcept {
	
		auto retr=func(loc,begin,end,locale);
		
		if (negated) retr=!retr;
		
		return retr;
	
	}


}
