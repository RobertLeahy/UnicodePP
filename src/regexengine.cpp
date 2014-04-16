#include <unicode/regex.hpp>
#include <utility>


namespace Unicode {


	template <typename Iterator>
	bool RegexEngine::backtrack (Iterator & begin, Iterator end) {
	
		for (;;) {
		
			auto iter=states.end()-1;
			
			if (iter->PreventsBacktracking) return false;
			
			if (iter->CanBacktrack) return true;
			
			iter->Rewind(*this);
			
			states.erase(iter);
			
			if (states.size()==0) break;
			
			--begin;
		
		}
		
		//	Ran out of states, fail
		return false;
	
	}
	
	
	template <typename Iterator>
	bool RegexEngine::execute (Iterator begin, Iterator end) {
	
		//	Whether the engine has just backtracked,
		//	causes creation of a RegexState to be skipped
		//	so that the RegexPatternElement being invoked
		//	has access to its backtracking information.
		//
		//	The engine initially backtracks if there are
		//	states, as this means that it has executed
		//	previously.
		bool backtracked=states.size()!=0;
		if (backtracked) {
		
			//	Advance to last state
			begin+=states.size()-1;
			
			//	Attempt to backtrack, if that fails,
			//	fail, there are no other substrings that
			//	we can match
			if (!backtrack(begin,end)) return false;
		
		}
		
		//	Loop over each pattern element and attempt to
		//	match it
		while (begin!=end) {
		
			//	Create state for this pattern element if
			//	appropriate
			if (backtracked) backtracked=false;
			else states.emplace_back(*this);
			
			//	Evaluate pattern element
			if ((*(begin->get()))(*this,states.back())) {
			
				//	Success
				++begin;
				
			} else {
			
				//	Failed, attempt to backtrack
				if (!backtrack(begin,end)) return false;
				
				backtracked=true;
			
			}
		
		}
		
		//	All pattern elements matched, success
		return true;
	
	}
	
	
	RegexEngine::RegexEngine (
		const CodePoint * loc,
		const CodePoint * begin,
		const CodePoint * end,
		bool reversed,
		const CodePoint * last,
		const Regex::Pattern & pattern,
		RegexMatch & match
	) noexcept
		:	b(reversed ? end : begin,reversed),
			l(loc,reversed),
			e(reversed ? begin : end,reversed),
			last(last),
			pattern(pattern),
			CanBacktrack(false),
			PreventsBacktracking(false),
			Match(match)
	{	}
	
	
	RegexEngine::RegexEngine (const RegexEngine & other, const Regex::Pattern & pattern)
		:	b(other.b),
			l(other.l),
			e(other.e),
			pattern(pattern),
			CanBacktrack(false),
			PreventsBacktracking(false),
			Match(other.Match)
	{	}
	
	
	RegexEngine::RegexEngine (const RegexEngine & other, const Regex::Pattern & pattern, bool reversed)
		:	RegexEngine(other,pattern)
	{
	
		if (reversed==Reversed()) return;
	
		std::swap(b,e);
		b.Reverse();
		l.Reverse();
		e.Reverse();
	
	}
	
	
	RegexEngine::operator bool () const noexcept {
	
		return l!=e;
	
	}
	
	
	bool RegexEngine::AtBeginning () const noexcept {
	
		return b==l;
	
	}
	
	
	RegexIterator RegexEngine::begin () const noexcept {
	
		return l;
	
	}
	
	
	void RegexEngine::begin (RegexIterator i) noexcept {
	
		l=i;
	
	}
	
	
	const CodePoint * RegexEngine::Begin () const noexcept {
	
		return Reversed() ? e.Base() : b.Base();
	
	}
	
	
	RegexIterator RegexEngine::end () const noexcept {
	
		return e;
	
	}
	
	
	const CodePoint * RegexEngine::End () const noexcept {
	
		return Reversed() ? b.Base() : e.Base();
	
	}
	
	
	const CodePoint * RegexEngine::Last () const noexcept {
	
		return last;
	
	}
	
	
	const CodePoint & RegexEngine::operator * () const noexcept {
	
		return *l;
	
	}
	
	
	const CodePoint * RegexEngine::operator -> () const noexcept {
	
		return &(*l);
	
	}
	
	
	RegexEngine & RegexEngine::operator ++ () noexcept {
	
		return *this+=1;
	
	}
	
	
	RegexEngine & RegexEngine::operator -- () noexcept {
	
		return *this-=1;
	
	}
	
	
	RegexEngine & RegexEngine::operator += (std::ptrdiff_t dist) noexcept {
	
		l+=dist;
		
		return *this;
	
	}
	
	
	RegexEngine & RegexEngine::operator -= (std::ptrdiff_t dist) noexcept {
	
		l-=dist;
		
		return *this;
	
	}
	
	
	std::size_t RegexEngine::Remaining () const noexcept {
	
		return static_cast<std::size_t>(e-l);
	
	}
	
	
	bool RegexEngine::Reversed () const noexcept {
	
		return b.Reversed();
	
	}
	
	
	bool RegexEngine::operator () () {
	
		CanBacktrack=false;
		PreventsBacktracking=false;
		
		auto retr=Reversed() ? execute(
			pattern.rbegin(),
			pattern.rend()
		) : execute(
			pattern.begin(),
			pattern.end()
		);
		
		//	Set flags as appropriate
		//
		//	We move through the collection of states backwards
		//
		//	If a state prevents backtracking, and it occurs before
		//	a state which can backtrack, that state will never get
		//	an opportunity to backtrack, and therefore the entire
		//	pattern should be treated as preventing backtracking
		//
		//	Similarly, if there's a state that can backtrack before
		//	a state that prevents backtracking, it's possible that
		//	backtracking to that state will yield a new match, and
		//	that the engine won't have to backtrack and fail at the
		//	pattern element which can't be backtracked over.
		//
		//	Therefore, we find the last state with one of these flags
		//	set, set that flag, and end.
		for (auto begin=states.rbegin(),end=states.rend();begin!=end;++begin) {
		
			if (begin->PreventsBacktracking) {
			
				PreventsBacktracking=true;
				
				break;
			
			}
			
			if (begin->CanBacktrack) {
			
				CanBacktrack=true;
				
				break;
			
			}
		
		}
		
		return retr;
	
	}
	
	
	void RegexEngine::Set (RegexState & state) const noexcept {
	
		state.CanBacktrack=CanBacktrack;
		state.PreventsBacktracking=PreventsBacktracking;
	
	}
	
	
	void RegexEngine::Set (RegexEngine & engine) const noexcept {
	
		engine.l=l;
	
	}
	
	
	void RegexEngine::Rewind () {
	
		while (states.size()!=0) {
		
			auto iter=states.end()-1;
			
			iter->Rewind(*this);
			
			states.erase(iter);
		
		}
	
	}
	

}
