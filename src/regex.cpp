#include <unicode/normalizer.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/vector.hpp>
#include <optional>
#include <utility>


namespace Unicode {


	//
	//	Regex::Iterator
	//


	bool Regex::Iterator::is_end () const noexcept {
	
		return loc==(rtl ? begin : end);
	
	}


	void Regex::Iterator::advance () noexcept {
	
		if (rtl) --loc;
		else ++loc;
	
	}
	
	
	static void complete (RegexMatch & match, const CodePoint * begin, const CodePoint * end) {
	
		if (begin>end) std::swap(begin,end);
		
		match.Complete(begin,end);
	
	}


	void Regex::Iterator::next () {
	
		//	If we're at the end, don't do anything, just
		//	flag this iterator as finished and abort
		if (at_end) {
		
			finished=true;
			
			return;
		
		}
		
		//	Loop until either a match is formed or
		//	the end of the string is encountered and
		//	there's no match there
		for (;;) {
		
			//	Track start position, in the case where
			//	a match is formed, but it is zero length,
			//	we must catch this to advance so as to
			//	avoid an infinite loop
			auto start=loc;
		
			//	Attempt match
		
			RegexMatch match;
			RegexEngine engine(
				loc,
				begin,
				end,
				rtl,
				last,
				*pattern,
				match
			);
			
			if (engine()) {
			
				//	Match success
			
				//	Complete the match and update the state
				auto l=engine.begin().Base();
				complete(match,loc,l);
				loc=last=l;
				this->match=std::move(match);
				
				//	If the match was zero length, there's
				//	special handling
				if (loc==start) {
				
					//	If we're at the end, mark it and
					//	return
					if (is_end()) {
					
						at_end=true;
						
						return;
					
					}
					
					//	Otherwise, advance to the next position
					advance();
				
				}
				
				//	We return straight away.  If the end of the
				//	string was reached, BUT characters were consumed,
				//	we're not done, A MATCH MUST BE ATTEMPTED AT
				//	EACH POSITION, INCLUDING THE LAST ONE
				return;
			
			}
			
			//	Stop looking if the end has been reached and
			//	there's no match there -- we're done
			if (is_end()) {
			
				at_end=finished=true;
				
				return;
			
			}
			
			//	Couldn't match at this point, move to the next
			//	point
			advance();
		
		}
	
	}


	Regex::Iterator::Iterator () noexcept
		:	pattern(nullptr),
			rtl(false),
			at_end(false),
			finished(true),
			loc(nullptr),
			begin(nullptr),
			end(nullptr)
	{	}
	
	
	Regex::Iterator::Iterator (
		const Pattern & pattern,
		bool rtl,
		const CodePoint * begin,
		const CodePoint * end
	)	:	pattern(&pattern),
			rtl(rtl),
			at_end(false),
			finished(false),
			loc(rtl ? end : begin),
			begin(begin),
			end(end),
			last(loc)
	{
	
		next();
	
	}
	
	
	RegexMatch & Regex::Iterator::operator * () {
	
		return *match;
	
	}
	
	
	RegexMatch * Regex::Iterator::operator -> () {
	
		return &(*match);
	
	}
	
	
	Regex::Iterator & Regex::Iterator::operator ++ () {
	
		next();
	
		return *this;
	
	}
	
	
	Regex::Iterator Regex::Iterator::operator ++ (int) {
	
		auto retr=*this;
		
		next();
		
		return retr;
	
	}
	
	
	bool Regex::Iterator::operator == (const Iterator & other) const noexcept {
	
		return finished && other.finished;
	
	}
	
	
	bool Regex::Iterator::operator != (const Iterator & other) const noexcept {
	
		return !(*this==other);
	
	}
	
	
	//
	//	Regex::Iterable
	//
	
	
	Regex::Iterable::Iterable (const Regex & r, const CodePoint * begin, const CodePoint * end, const Locale & locale) : r(r) {
	
		Normalizer n(locale);
		if (n.IsNFD(begin,end)) {
		
			b=begin;
			e=end;
			
			return;
		
		}
		
		s.emplace(n.ToNFD(begin,end));
		b=s->begin();
		e=s->end();
	
	}
	
	
	Regex::Iterable::Iterable (const Regex & r, const String & str) : Iterable(r,str.begin(),str.end(),str.GetLocale()) {	}
	
	
	Regex::Iterable::Iterable (const Regex & r, String && str)
		:	r(r),
			s(std::move(str))
	{
	
		s->ToNFD();
		
		b=s->begin();
		e=s->end();
	
	}
	
	
	Regex::Iterator Regex::Iterable::begin () const {
	
		return r.Begin(b,e);
	
	}
	
	
	Regex::Iterator Regex::Iterable::end () const noexcept {
	
		return r.End();
	
	}
	
	
	//
	//	Regex
	//


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
	
	
	std::optional<RegexMatch> Regex::Match (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		std::optional<RegexMatch> retr;
		for (auto & match : Iterate(begin,end,locale)) {
		
			retr=std::move(match);
			
			break;
		
		}
		
		return retr;
	
	}
	
	
	std::optional<RegexMatch> Regex::Match (const String & str) const {
	
		return Match(str.begin(),str.end(),str.GetLocale());
	
	}
	
	
	std::vector<RegexMatch> Regex::Matches (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		std::vector<RegexMatch> retr;
		for (auto & match : Iterate(begin,end,locale)) retr.push_back(std::move(match));
		
		return retr;
	
	}
	
	
	std::vector<RegexMatch> Regex::Matches (const String & str) const {
	
		return Matches(str.begin(),str.end(),str.GetLocale());
	
	}
	
	
	Regex::Iterable Regex::Iterate (const CodePoint * begin, const CodePoint * end, const Locale & locale) const {
	
		return Iterable(*this,begin,end,locale);
	
	}
	
	
	Regex::Iterable Regex::Iterate (const String & str) const {
	
		return Iterable(*this,str);
	
	}
	
	
	Regex::Iterable Regex::Iterate (String && str) const {
	
		return Iterable(*this,std::move(str));
	
	}
	
	
	Regex::Iterator Regex::Begin (const CodePoint * begin, const CodePoint * end) const {
	
		return Iterator(pattern,rtl,begin,end);
	
	}
	
	
	Regex::Iterator Regex::End () const noexcept {
	
		return Iterator{};
	
	}
	
	
	std::vector<RegexToString> Regex::ToString () const {
	
		std::vector<RegexToString> retr;
		for (auto & element : pattern) retr.push_back(element->ToString());
		
		return retr;
	
	}


}
