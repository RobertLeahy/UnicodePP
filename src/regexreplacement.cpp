#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>


namespace Unicode {


	String RegexReplacement::Escape (const String & str) {
	
		String retr;
		for (auto cp : str) {
		
			if (cp=='$') retr << CodePoint('$');
			retr << cp;
		
		}
		
		return retr;
	
	}
	
	
	RegexReplacement::RegexReplacement (const String & str)
		:	elements(RegexReplacementCompiler::Compile(str.begin(),str.end()))
	{	}


	String RegexReplacement::operator () (RegexMatch & match, const CodePoint * begin, const CodePoint * end) const {
	
		String retr;
		for (auto & element : elements) retr << (*element)(match,begin,end);
		
		return retr;
	
	}


}
