#include <unicode/regexreplacement.hpp>
#include <algorithm>
#include <limits>


namespace Unicode {


	typedef RegexReplacementCompilerAcquisitionPolicy::Type parser_info;
	
	
	static std::vector<parser_info> & get_impl () {
	
		static std::vector<parser_info> vec;
		
		return vec;
	
	}
	
	
	constexpr std::size_t default_priority=std::numeric_limits<std::size_t>::max()/2;
	
	
	static void add_impl (const RegexReplacementParser & parser, std::size_t priority=default_priority) {
	
		auto & parsers=get_impl();
		parsers.insert(
			std::find_if(
				parsers.begin(),
				parsers.end(),
				[&] (const parser_info & info) noexcept {
				
					return priority<=info.Priority;
				
				}
			),
			{&parser,priority}
		);
	
	}
	
	
	void RegexReplacementCompilerAcquisitionPolicy::Add (const RegexReplacementParser & parser) {
	
		add_impl(parser);
	
	}
	
	
	void RegexReplacementCompilerAcquisitionPolicy::Add (const RegexReplacementParser & parser, std::size_t priority) {
	
		add_impl(parser,priority);
	
	}
	
	
	const std::vector<parser_info> & RegexReplacementCompilerAcquisitionPolicy::Get () {
	
		return get_impl();
	
	}
	
	
	bool RegexReplacementCompilerInvocationPolicy::Invoke (RegexReplacementCompilerBase & compiler, const parser_info & info) {
	
		return (*info.Parser)(reinterpret_cast<RegexReplacementCompiler &>(compiler));
	
	}


	RegexReplacementCompiler::RegexReplacementCompiler (Iterator begin, Iterator end)
		:	RegexReplacementCompilerBase(begin,begin,end)
	{	}
	
	
	RegexReplacementCompiler::Elements RegexReplacementCompiler::Compile (Iterator begin, Iterator end) {
	
		RegexReplacementCompiler c(begin,end);
		c();
		
		return c.Get();
	
	}


}
