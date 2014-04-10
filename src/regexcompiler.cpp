#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/regexerror.hpp>
#include <algorithm>
#include <limits>
#include <utility>


namespace Unicode {
	
	
	std::vector<RegexCompiler::RegexParserInfo> & RegexCompiler::get_parsers () {
	
		static std::vector<RegexParserInfo> parsers;
		
		return parsers;
	
	}
	
	
	void RegexCompiler::add_impl (const RegexParser & parser, std::size_t priority, std::optional<bool> character_class) {
	
		auto & parsers=get_parsers();
		parsers.insert(
			std::find_if(
				parsers.begin(),
				parsers.end(),
				[&] (const RegexParserInfo & info) noexcept {	return info.Priority>=priority;	}
			),
			{&parser,priority,character_class}
		);
	
	}
	
	
	constexpr std::size_t default_priority=std::numeric_limits<std::size_t>::max()/2;
	
	
	void RegexCompiler::Add (const RegexParser & parser) {
	
		Add(parser,default_priority);
	
	}
	
	
	void RegexCompiler::Add (const RegexParser & parser, std::size_t priority) {
	
		add_impl(parser,priority,std::nullopt);
	
	}
	
	
	void RegexCompiler::Add (const RegexParser & parser, bool character_class) {
	
		add_impl(parser,default_priority,character_class);
	
	}
	
	
	void RegexCompiler::Add (const RegexParser & parser, std::size_t priority, bool character_class) {
	
		add_impl(parser,priority,character_class);
	
	}
	
	
	RegexCompiler::Pattern RegexCompiler::Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) {
	
		RegexCompiler c(begin,end,options,locale);
		c();
		
		return c.Get();
	
	}
	
	
	void RegexCompiler::complete () {
	
		if (last==nullptr) return;
		
		last->Complete(*(pattern.back().get()));
		last=nullptr;
	
	}
	
	
	bool RegexCompiler::should_invoke (const RegexParserInfo & info) const noexcept {
	
		if (!info.CharacterClass) return true;
		
		return *info.CharacterClass==CharacterClass;
	
	}
	
	
	void RegexCompiler::rewind (Iterator loc) noexcept {
	
		Current=loc;
		current=nullptr;
		Successive=false;
	
	}
	
	
	RegexCompiler::RegexCompiler (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept
		:	RegexCompiler(begin,begin,end,options,locale)
	{	}
	
	
	RegexCompiler::RegexCompiler (Iterator curr, Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) noexcept
		:	last(nullptr),
			current(nullptr),
			Current(curr),
			Begin(begin),
			End(end),
			Options(options),
			Locale(locale),
			Successive(false),
			CharacterClass(false)
	{	}
	
	
	RegexCompiler::RegexCompiler (const RegexCompiler & other) noexcept
		:	last(nullptr),
			current(nullptr),
			Current(other.Current),
			Begin(other.Begin),
			End(other.End),
			Options(other.Options),
			Locale(other.Locale),
			Successive(false),
			CharacterClass(false)
	{	}
	
	
	RegexCompiler::Pattern RegexCompiler::Get () {
	
		complete();
		
		auto retr=std::move(pattern);
		pattern=Pattern{};
		last=nullptr;
		
		return retr;
	
	}
	
	
	void RegexCompiler::Set (Pattern pattern) {
	
		this->pattern=std::move(pattern);
		last=nullptr;
	
	}
	
	
	RegexCompiler::Element RegexCompiler::Pop () {
	
		complete();
		
		auto retr=std::move(pattern.back());
		pattern.erase(pattern.end()-1);
		
		return retr;
	
	}
	
	
	void RegexCompiler::Add (Element element) {
	
		complete();
		
		pattern.push_back(std::move(element));
		
		last=current;
	
	}
	
	
	void RegexCompiler::Raise (const char * what) const {
	
		throw RegexError(what,Current);
	
	}
	
	
	bool RegexCompiler::Done () {
	
		return !static_cast<bool>(*this);
	
	}
	
	
	void RegexCompiler::operator () () {
	
		auto & parsers=get_parsers();
	
		//	Loop until done
		while (!Done()) {
		
			//	Track start position so that we
			//	can rewind to it on failure
			auto start=Current;
			
			//	Loop over each parser
			for (auto & info : parsers) {
			
				//	Skip parsers that shouldn't be invoked
				if (!should_invoke(info)) continue;
				
				current=info.Parser;
				Successive=current==last;
				try {
				
					if ((*info.Parser)(*this)) {
					
						//	SUCCESS
						
						//	If the parser didn't advance the
						//	iterator, do it for it
						if (Current==start) ++Current;
						
						goto success;
					
					}
				
				} catch (...) {
				
					rewind(start);
					
					throw;
				
				}
				
				rewind(start);
			
			}
		
			//	If no parser was found, throw
			Raise("No matching parser");
			
			success:;
		
		}
		
		//	Make sure everything is wrapped up
		complete();
	
	}


}
