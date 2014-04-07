#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <algorithm>
#include <limits>
#include <optional>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexParserInfo {
		
		
			public:
			
			
				const RegexParser * Parser;
				std::size_t Priority;
				std::optional<bool> CharacterClass;
		
		
		};
	
	
	}
	
	
	static std::vector<RegexParserInfo> & get_parsers () {
	
		static std::vector<RegexParserInfo> parsers;
		
		return parsers;
	
	}
	
	
	static void add_impl (const RegexParser & parser, std::size_t priority, std::optional<bool> character_class) {
	
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
	
	
	RegexCompiler::Pattern RegexCompiler::operator () (RegexCompilerState && state) const {
	
		return (*this)(state);
	
	}
	
	
	static bool should_invoke (const RegexParserInfo & info, const RegexCompilerState & state) noexcept {
	
		return !info.CharacterClass || (*(info.CharacterClass)==state.CharacterClass);
	
	}
	
	
	RegexCompiler::Pattern RegexCompiler::operator () (RegexCompilerState & state) const {
	
		//	The pattern that is being compiled
		Pattern pattern;
		//	The last type of pattern element invoked
		const RegexParser * last=nullptr;
		//	The list of parsers
		auto & parsers=get_parsers();
		
		//	Loop over each code point in the input string,
		//	or until the state reports that compilation is
		//	finished
		while (!state.Done()) {
		
			//	Track the start point so we can detect the
			//	case where no parser can generate a pattern
			//	element
			auto start=state.Current;
			
			//	Loop over each parser
			for (auto & info : parsers) {
			
				//	Skip parsers that shouldn't be invoked in this
				//	context
				if (!should_invoke(info,state)) continue;
			
				//	Make sure the state is clean
				state.Reset();
				
				//	The current parser
				auto & parser=*(info.Parser);
				
				//	Attempt to parse using the preceding
				//	pattern element if this is the same type
				//	of parser as was used previously, and if
				//	this is not a character class
				bool parsed=state.CharacterClass ? false : ((info.Parser==last) && parser(*(pattern.back().get()),state));
				//	If parsing above failed or was not performed,
				//	perform a regular parse -- i.e. attempt to create
				//	a new pattern element
				if (!parsed) {
				
					//	Rewind
					state.Current=start;
				
					auto element=parser(state);
					if (element) {
					
						//	If applicable, complete last pattern element
						if (last!=nullptr) last->Complete(*(pattern.back().get()));
					
						//	Add newly-parsed pattern element
						parsed=true;
						pattern.push_back(std::move(element));
					
					}
				
				}
				
				//	If parsing was successful, perform cleanup
				//	and stop looping
				if (parsed || !state.Fail) {
				
					if (parsed) last=info.Parser;
					//	Advance to next code point if parser
					//	failed to do so, to avoid possible
					//	infinite loop
					if (state.Current==start) ++state;
					
					break;
				
				}
				
				//	Otherwise rewind and continue
				state.Current=start;
			
			}
			
			//	If nothing was consumed, throw
			if (start==state.Current) state.Raise("No matching parser");
		
		}
		
		//	Complete the last pattern element
		if (last!=nullptr) last->Complete(*(pattern.back().get()));
		
		return pattern;
	
	}


}
