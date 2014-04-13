#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/regexerror.hpp>
#include <algorithm>
#include <limits>
#include <type_traits>
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
	
	
	void RegexCompiler::get_capturing_group (GroupsInfo & info, const RegexPatternElement * & ptr) {
	
		if (info.Elements.size()==0) info.Pending.push_back(&ptr);
		else ptr=info.Elements[0];
	
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
	
		std::size_t automatic=0;
		GroupsMapping<std::size_t> numbered;
		GroupsMapping<String> named;
		RegexCompiler c(
			begin,
			begin,
			end,
			automatic,
			numbered,
			named,
			options,
			locale
		);
		c();
		
		c.complete(numbered);
		c.complete(named);
		
		return c.Get();
	
	}
	
	
	void RegexCompiler::complete () {
	
		if (last==nullptr) return;
		
		last->Complete(*(pattern.back().get()));
		last=nullptr;
	
	}
	
	
	template <typename T>
	void RegexCompiler::complete (GroupsMapping<T> & map) {
	
		for (auto & pair : map) {
		
			auto & info=pair.second;
			if (info.Pending.size()==0) continue;
			if (info.Elements.size()==0) Raise("Forward referenced capturing group was not declared");
			
			for (auto & pending : info.Pending) *pending=info.Elements[0];
		
		}
	
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
	
	
	RegexCompiler::RegexCompiler (
		Iterator begin,
		Iterator curr,
		Iterator end,
		std::size_t & automatic,
		GroupsMapping<std::size_t> & numbered,
		GroupsMapping<String> & named,
		RegexOptions options,
		const Unicode::Locale & locale
	) noexcept
		:	last(nullptr),
			current(nullptr),
			automatic(automatic),
			numbered(numbered),
			named(named),
			Current(curr),
			Begin(begin),
			End(end),
			Options(options),
			Locale(locale),
			Successive(false),
			CharacterClass(false)
	{	}
	
	
	RegexCompiler::RegexCompiler (const RegexCompiler & other) noexcept
		:	RegexCompiler(
				other.Begin,
				other.Current,
				other.End,
				other.automatic,
				other.numbered,
				other.named,
				other.Options,
				other.Locale
			)
	{	}
	
	
	bool RegexCompiler::IsNext (CodePoint cp, bool rewind) noexcept {
	
		if (*this && (*(*this)==cp)) {
		
			if (!rewind) ++*this;
			
			return true;
			
		}
		
		return false;
	
	}
	
	
	template <typename T>
	bool is_next (const T * str, RegexCompiler & compiler) noexcept {
	
		if (str==nullptr) return true;
		
		if (!compiler) return *str==0;
		
		for (
			;
			(*str!='\0') && (*str==*compiler) && compiler;
			++str,++compiler
		);
		
		return *str=='\0';
	
	}
	
	
	template <typename T>
	bool is_next (const T * str, RegexCompiler & compiler, bool rewind) noexcept {
	
		auto loc=compiler.Current;
		auto result=is_next(str,compiler);
		if (!result || rewind) compiler.Current=loc;
		
		return result;
	
	}
	
	
	bool RegexCompiler::IsNext (const char32_t * str, bool rewind) noexcept {
	
		return is_next(str,*this,rewind);
	
	}
	
	
	bool RegexCompiler::IsNext (const char * str, bool rewind) noexcept {
	
		return is_next(str,*this,rewind);
	
	}
	
	
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
	
		auto back=pattern.end()-1;
		
		auto extracted=last->GetLast(*(*back));
		
		complete();
		
		if (extracted) return std::move(extracted);
		
		auto retr=std::move(*back);
		pattern.erase(back);
		
		return retr;
	
	}
	
	
	void RegexCompiler::Add (Element element) {
	
		complete();
		
		pattern.push_back(std::move(element));
		
		last=current;
	
	}
	
	
	std::size_t RegexCompiler::GetCaptureNumber () noexcept {
	
		return ++automatic;
	
	}
	
	
	std::size_t RegexCompiler::GetLastCaptureNumber () noexcept {
	
		return automatic;
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (const String & key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(named[key],ptr);
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (String && key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(named[std::move(key)],ptr);
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (std::size_t key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(numbered[key],ptr);
	
	}
	
	
	RegexCompiler::Groups & RegexCompiler::operator [] (const String & key) {
	
		return named[key].Elements;
	
	}
	
	
	RegexCompiler::Groups & RegexCompiler::operator [] (String && key) {
	
		return named[std::move(key)].Elements;
	
	}
	
	
	RegexCompiler::Groups & RegexCompiler::operator [] (std::size_t key) {
	
		return numbered[key].Elements;
	
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
