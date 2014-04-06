#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	Regex::Regex (const String & pattern, RegexOptions options, const Locale & locale)
		:	pattern(RegexCompiler{}(RegexCompilerState(
				pattern.begin(),
				pattern.end(),
				options,
				locale
			)))
	{	}
	
	
	Regex::Regex (const CodePoint * begin, const CodePoint * end, RegexOptions options, const Locale & locale)
		:	pattern(RegexCompiler{}(RegexCompilerState(
				begin,
				end,
				options,
				locale
			)))
	{	}
	
	
	std::vector<RegexToString> Regex::ToString () const {
	
		std::vector<RegexToString> retr;
		for (auto & element : pattern) retr.push_back(element->ToString());
		
		return retr;
	
	}


}
