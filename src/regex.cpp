#include <unicode/normalizer.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/vector.hpp>
#include <optional>
#include <utility>


namespace Unicode {
	
	
	void Regex::advance (const CodePoint * & loc) const noexcept {
	
		if (rtl) --loc;
		else ++loc;
	
	}
	
	
	bool Regex::is_done (const CodePoint * begin, const CodePoint * loc, const CodePoint * end) const noexcept {
	
		return loc==(rtl ? begin : end);
	
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
	
	
	static void complete (RegexMatch & match, const CodePoint * begin, const CodePoint * end) {
	
		if (begin>end) std::swap(begin,end);
		
		match.Complete(begin,end);
	
	}
	
	
	std::optional<RegexMatch> Regex::Match (
		const CodePoint * begin,
		const CodePoint * & loc,
		const CodePoint * end,
		const CodePoint * & last
	) const {
	
		//	Set the location appropriately if it's
		//	not set
		if (loc==nullptr) loc=last=rtl ? end : begin;
	
		std::optional<RegexMatch> retr;
		
		for (;!is_done(begin,loc,end);advance(loc)) {
		
			RegexMatch match;
			RegexEngine engine(
				loc,
				begin,
				end,
				rtl,
				last,
				pattern,
				match
			);
			if (engine()) {
			
				auto l=engine.begin().Base();
				complete(match,loc,l);
				loc=l;
				last=l;
				
				retr.emplace(std::move(match));
				
				break;
			
			}
		
		}
		
		return retr;
	
	}
	
	
	std::optional<RegexMatch> Regex::Match (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		const CodePoint * loc=nullptr;
		const CodePoint * last=nullptr;
	
		Normalizer n(locale);
		if (n.IsNFD(begin,end)) return Match(begin,loc,end,last);
		
		auto normalized=n.ToNFD(begin,end);
		return Match(Begin(normalized),loc,End(normalized),last);
	
	}
	
	
	std::optional<RegexMatch> Regex::Match (const String & str) const {
	
		return Match(str.begin(),str.end(),str.GetLocale());
	
	}
	
	
	std::vector<RegexToString> Regex::ToString () const {
	
		std::vector<RegexToString> retr;
		for (auto & element : pattern) retr.push_back(element->ToString());
		
		return retr;
	
	}


}
