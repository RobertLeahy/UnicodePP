#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/regexerror.hpp>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>


namespace Unicode {


	constexpr std::size_t default_priority=std::numeric_limits<std::size_t>::max()/2;
	
	
	typedef RegexCompilerAcquisitionPolicy::Type parser_info;
	
	
	static std::vector<parser_info> & get_impl () {
	
		static std::vector<parser_info> vec;
		
		return vec;
	
	}
	
	
	static void add_impl (
		const RegexParser & parser,
		std::size_t priority=default_priority,
		std::optional<bool> character_class=std::nullopt
	) {
	
		auto & parsers=get_impl();
		parsers.insert(
			std::find_if(
				parsers.begin(),
				parsers.end(),
				[&] (const parser_info & t) noexcept {
				
					return priority<=t.Priority;
				
				}
			),
			{&parser,priority,character_class}
		);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser) {
	
		add_impl(parser);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, std::size_t priority) {
	
		add_impl(parser,priority);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, bool character_class) {
	
		add_impl(parser,default_priority,character_class);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, std::size_t priority, bool character_class) {
	
		add_impl(parser,priority,character_class);
	
	}
	
	
	const std::vector<RegexCompilerAcquisitionPolicy::Type> & RegexCompilerAcquisitionPolicy::Get () {
	
		return get_impl();
	
	}
	
	
	bool RegexCompilerInvocationPolicy::Invoke (RegexCompilerBaseType & compiler, const parser_info & info) {
	
		auto & c=reinterpret_cast<RegexCompiler &>(compiler);
	
		if (info.CharacterClass && (*info.CharacterClass!=c.CharacterClass)) return false;
		
		return (*info.Parser)(c);
	
	}
	
	
	void RegexCompiler::get_capturing_group (GroupsInfo & info, const RegexPatternElement * & ptr) {
		
		if (info.Elements.size()==0) info.Pending.push_back(&ptr);
		else ptr=info.Elements[0];
		
	}
	
	
	RegexCompiler::Elements RegexCompiler::Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) {
	
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
			Unicode::Check(options,RegexOptions::CultureInvariant) ? DefaultLocale : locale
		);
		c();
		
		c.complete(numbered);
		c.complete(named);
		
		return c.Get();
	
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
	
	
	RegexCompiler::RegexCompiler (
		Iterator curr,
		Iterator begin,
		Iterator end,
		std::size_t & automatic,
		GroupsMapping<std::size_t> & numbered,
		GroupsMapping<String> & named,
		RegexOptions options,
		const Unicode::Locale & locale
	) noexcept
		:	RegexCompilerBase(curr,begin,end),
			automatic(automatic),
			numbered(numbered),
			named(named),
			Options(options),
			Locale(locale),
			CharacterClass(false)
	{	}
	
	
	RegexCompiler::RegexCompiler (const RegexCompiler & other) noexcept
		:	RegexCompiler(
				other.Current,
				other.Begin,
				other.End,
				other.automatic,
				other.numbered,
				other.named,
				other.Options,
				other.Locale
			)
	{	}
	
	
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


}
