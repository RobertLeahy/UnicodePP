#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {
	
	
	typedef std::vector<std::pair<std::size_t,const RegexParser *>> parsers_type;
	
	
	static parsers_type & get_parsers () {
	
		static parsers_type parsers;
		
		return parsers;
	
	}
	
	
	void RegexCompiler::Add (const RegexParser & parser, std::size_t priority) {
	
		auto & parsers=get_parsers();
	
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
	
		//	The pattern that is being compiled
		Pattern pattern;
		//	The last type of pattern element invoked
		const RegexParser * last=nullptr;
		//	The list of parsers
		auto & parsers=get_parsers();
		//	The locale that will be used.
		//
		//	If the culture invariant flag is set, this
		//	is always the default locale, otherwise it
		//	is whatever locale is provided
		auto & l=Check(options,RegexOptions::CultureInvariant) ? DefaultLocale : locale;
		
		//	Loop over each code point in the input string
		while (begin!=end) {
		
			//	Track the start point so we can detect the
			//	case where no parser can generate a pattern
			//	element
			auto start=begin;
			
			//	Loop over each parser
			for (auto & pair : parsers) {
			
				//	The current parser
				auto & parser=*(pair.second);
				
				//	Attempt to parse using the preceding
				//	pattern element if this is the same type
				//	of parser as was used previously
				bool parsed=(pair.second==last) && parser(*(pattern.back().get()),begin,end);
				//	If parsing above failed or was not performed,
				//	perform a regular parse -- i.e. attempt to create
				//	a new pattern element
				if (!parsed) {
				
					auto element=parser(begin,end,options,l);
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
				if (parsed) {
				
					last=pair.second;
					//	Advance to next code point if parser
					//	failed to do so, to avoid possible
					//	infinite loop
					if (begin==start) ++begin;
					
					break;
				
				}
				
				//	Otherwise rewind and continue
				begin=start;
			
			}
			
			//	If nothing was parsed, throw
			if (start==begin) {
			
				//	TODO: Throw
			
			}
		
		}
		
		//	Complete the last pattern element
		if (last!=nullptr) last->Complete(*(pattern.back().get()));
		
		return pattern;
	
	}


}
