#include <unicode/normalizer.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/vector.hpp>
#include <optional>


namespace Unicode {


	static void complete (RegexMatch & match, RegexEngine & engine) {
	
		if (engine.Reversed()) match.Complete(
			engine.begin().Base(),
			engine.End()
		);
		else match.Complete(
			engine.Begin(),
			engine.begin().Base()
		);
	
	}


	std::optional<RegexMatch> Regex::match (const CodePoint * begin, const CodePoint * end) const {
	
		std::optional<RegexMatch> retr;
		
		while (begin!=end) {
		
			RegexMatch match;
			RegexEngine engine(
				begin,
				end,
				rtl,
				pattern,
				match
			);
			if (engine()) {
			
				complete(match,engine);
			
				retr.emplace(std::move(match));
				
				return retr;
				
			}
			
			if (rtl) --end;
			else ++begin;
			
		}
		
		return retr;
	
	}


	String Regex::Escape (String str) {
	
		String retr;
		for (auto cp : str) {
		
			if (cp.IsWhiteSpace()) goto escape;
		
			switch (cp) {
			
				case '\\':
				case '*':
				case '+':
				case '?':
				case '|':
				case '{':
				case '}':
				case '[':
				case ']':
				case '(':
				case ')':
				case '^':
				case '$':
				case '.':
				case '#':
				case '-':
					escape:
					retr << CodePoint('\\');
				default:
					retr << cp;
					break;
			
			}
		
		}
		
		return retr;
	
	}
	
	
	Regex::Regex (const String & pattern, RegexOptions options, const Locale & locale)
		:	Regex(pattern.begin(),pattern.end(),options,locale)
	{	}
	
	
	Regex::Regex (const CodePoint * begin, const CodePoint * end, RegexOptions options, const Locale & locale)
		:	pattern(RegexCompiler::Compile(begin,end,options,locale)),
			rtl(Check(options,RegexOptions::RightToLeft))
	{	}
	
	
	std::optional<RegexMatch> Regex::Match (const CodePoint * begin, const CodePoint * end) const {
	
		Normalizer n;
		if (n.IsNFD(begin,end)) return match(begin,end);
		
		auto normalized=n.ToNFD(begin,end);
		return match(Begin(normalized),End(normalized));
	
	}
	
	
	std::optional<RegexMatch> Regex::Match (const String & str) const {
	
		return Match(str.begin(),str.end());
	
	}
	
	
	std::vector<RegexToString> Regex::ToString () const {
	
		std::vector<RegexToString> retr;
		for (auto & element : pattern) retr.push_back(element->ToString());
		
		return retr;
	
	}


}
