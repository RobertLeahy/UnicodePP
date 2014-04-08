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
	
	
	RegexCompiler::Pattern RegexCompiler::operator () (
		const CodePoint * begin,
		const CodePoint * end,
		RegexOptions options,
		const Locale & locale
	) const {
	
		RegexCompilerState state(
			begin,
			end,
			options,
			locale
		);
		(*this)(state);
		
		return std::move(state.Pattern);
	
	}
	
	
	static bool should_invoke (const RegexParserInfo & info, const RegexCompilerState & state) noexcept {
	
		return !info.CharacterClass || (*(info.CharacterClass)==state.CharacterClass);
	
	}
	
	
	static void complete (const RegexParser * last, std::size_t last_loc, RegexCompilerState & state) {
	
		if (last!=nullptr) last->Complete(*(state.Pattern[last_loc].get()));
	
	}
	
	
	void RegexCompiler::operator () (RegexCompilerState & state) const {
	
		//	The last type of parser successfully
		//	invoked
		const RegexParser * last=nullptr;
		//	Index of the pattern element the last
		//	successfully invoked parser generated
		std::size_t last_loc;
		//	The list of parsers
		auto & parsers=get_parsers();
		
		//	Loop until done
		while (!state.Done()) {
		
			//	Track the start point so:
			//
			//	A.	We can return here if a parse fails.
			//	B.	We can detect the case where no parser
			//		can generate a pattern element/consume
			//		code points
			auto start=state.Current;
			
			//	Loop for each parser
			for (auto & info : parsers) {
			
				//	Skip parsers that shouldn't be invoked in
				//	this context
				if (!should_invoke(info,state)) continue;
				
				//	Make sure the state is clean/consistent
				state.Reset();
				state.Successive=info.Parser==last;
				
				if ((*(info.Parser))(state)) {
				
					//	Parse successful
					
					//	Update last parser if pattern elements were
					//	actually generated
					if (state.Parsed) {
					
						//	Complete last pattern element
						complete(last,last_loc,state);
					
						last=info.Parser;
						last_loc=state.Pattern.size()-1;
						
					}
					
					//	If parser failed to advance state, do it
					//	for them
					if (state.Current==start) ++state;
					
					//	Proceeds to next iteration of outer loop
					goto success;
				
				}
				
				//	Parse failed
				
				//	Rewind
				state.Current=start;
			
			}
			
			//	Nothing was consumed, throw
			state.Raise("No matching parser");
			
			//	Control comes here after a successful parse
			success:;
		
		}
		
		//	Complete last pattern element
		complete(last,state.Pattern.size()-1,state);
	
	}


}
