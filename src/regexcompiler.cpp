#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {


	static std::vector<
		std::pair<
			std::size_t,
			const RegexParser *
		>
	> parsers;
	
	
	void RegexCompiler::Add (const RegexParser & parser, std::size_t priority) {
	
		//	Find insertion point
		auto begin=parsers.begin();
		for (auto end=parsers.end();begin!=end;++begin) if (begin->first>priority) break;
		
		parsers.insert(begin,{priority,&parser});
	
	}
	
	
	RegexCompiler::Pattern RegexCompiler::operator () (
		const CodePoint * begin,
		const CodePoint * end,
		RegexOptions options,
		const Locale & locale
	) const {
	
		Pattern pattern;
		const RegexParser * last=nullptr;
		
		//	Loop over each code point in the input string
		while (begin!=end) {
		
			//	Track the start point so we can detect the
			//	case where no parser can generate a pattern
			//	element
			auto start=begin;
			
			//	Loop over each pattern element
			for (auto & pair : parsers) {
			
				//	Attempt to parse
				if ((*(pair.second))(
					pattern,
					last==pair.second,
					begin,
					end,
					options,
					locale
				)) {
				
					//	Parse successful
					
					//	Advance iterator if the parser didn't do it
					if (start==begin) ++begin;
					
					//	Set last parser to this parser
					last=pair.second;
					
					//	Advance to next iteration
					break;
				
				}
				
				//	Parse failed
				
				//	Rewind to start point
				begin=start;
			
			}
			
			//	Was anything parsed?
			//
			//	If not, throw
			if (start==begin) {
			
				//	TODO: Throw
			
			}
		
		}
		
		return pattern;
	
	}


}
