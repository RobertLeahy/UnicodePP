#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>


namespace Unicode {


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
	
	
	template <typename Iterator>
	static bool backtrack (Iterator & begin, Iterator end, RegexState & state, Regex::States & states) {
	
		for (;;) {
		
			auto iter=states.end()-1;
			
			iter->Rewind(state);
			
			if (iter->CanBacktrack) {
			
				state.Backtracking=true;
				
				return true;
			
			}
			
			if (iter->PreventsBacktracking) return false;
			
			states.erase(iter);
			
			if (states.size()==0) break;
			
			--begin;
		
		}
		
		//	We ran out of states, fail
		return false;
	
	}
	
	
	template <typename Iterator>
	static bool execute (Iterator begin, Iterator end, RegexState & state, Regex::States & states) {
	
		//	If there are states, begin with some preparation
		if (states.size()!=0) {
		
			//	Advance begin iterator appropriately
			begin+=states.size();
			
			//	Attempt to backtrack.
			//
			//	If that fails, fail
			if (!backtrack(begin,end,state,states)) return false;
		
		}
		
		//	Loop over each pattern element and attempt
		//	to match it
		for (;begin!=end;state.Backtracking=false) {
		
			//	Create state for this pattern element
			states.emplace_back(state.begin());
			
			//	Evaluate pattern element
			if (!(*(begin->get()))(state,states.back())) {
			
				//	Backtrack -- fail if backtracking is not
				//	possible
				if (!backtrack(begin,end,state,states)) return false;
			
			} else {
			
				//	If match was a success, advance to the
				//	next pattern element
				++begin;
			
			}
		
		}
		
		//	Success -- all pattern elements matched
		return true;
	
	}
	
	
	bool Regex::Execute (const Pattern & pattern, RegexState & state) {
	
		States states;
		return Execute(pattern,state,states);
	
	}
	
	
	bool Regex::Execute (const Pattern & pattern, RegexState & state, States & states) {
	
		return state.Reversed() 
			?	execute(
					pattern.rbegin(),
					pattern.rend(),
					state,
					states
				)
			:	execute(
					pattern.begin(),
					pattern.end(),
					state,
					states
				);
	
	}
	
	
	Regex::Regex (const String & pattern, RegexOptions options, const Locale & locale)
		:	Regex(pattern.begin(),pattern.end(),options,locale)
	{	}
	
	
	Regex::Regex (const CodePoint * begin, const CodePoint * end, RegexOptions options, const Locale & locale)
		:	pattern(RegexCompiler{}(
				begin,
				end,
				options,
				locale
			)),
			rtl(Check(options,RegexOptions::RightToLeft))
	{	}
	
	
	std::vector<RegexToString> Regex::ToString () const {
	
		std::vector<RegexToString> retr;
		for (auto & element : pattern) retr.push_back(element->ToString());
		
		return retr;
	
	}


}
