#include <unicode/regex.hpp>
#include <new>


namespace Unicode {


	template <typename Iterator>
	bool RegexEngine::backtrack (Iterator & begin, Iterator end) {
	
		for (;;) {
		
			auto iter=states.end()-1;
		
			iter->Rewind(*this);
			
			if (iter->CanBacktrack) return true;
			
			if (iter->PreventsBacktracking) return false;
			
			states.erase(iter);
			
			if (states.size()==0) break;
			
			--begin;
		
		}
		
		//	Ran out of states, fail
		return false;
	
	}
	
	
	template <typename Iterator>
	bool RegexEngine::execute (Iterator begin, Iterator end) {
	
		//	If there are states, that means that this
		//	engine has executed previously, and that
		//	backtracking may be possible
		if (states.size()!=0) {
		
			//	Advance to last state
			begin+=states.size();
			
			//	Attempt to backtrack, if that fails,
			//	fail, there are no other substrings that
			//	we can match
			if (!backtrack(begin,end)) return false;
		
		}
		
		//	Loop over each pattern element and attempt to
		//	match it
		while (begin!=end) {
		
			//	Create state for this pattern element
			states.emplace_back(b);
			
			//	Evaluate pattern element
			if ((*(begin->get()))(*this,states.back())) {
			
				//	Success
				++begin;
				
			} else {
			
				//	Failed, attempt to backtrack
				if (!backtrack(begin,end)) return false;
			
			}
		
		}
		
		//	All pattern elements matched, success
		return true;
	
	}
	
	
	RegexEngine::RegexEngine (const CodePoint * begin, const CodePoint * end, bool reversed, const Regex::Pattern & pattern) noexcept
		:	b(reversed ? end : begin,reversed),
			l(b),
			e(reversed ? begin : end,reversed),
			pattern(pattern)
	{	}
	
	
	RegexEngine::operator bool () const noexcept {
	
		return l!=e;
	
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
	
		return Reversed() ? execute(
			pattern.rbegin(),
			pattern.rend()
		) : execute(
			pattern.begin(),
			pattern.end()
		);
	
	}
	

}
