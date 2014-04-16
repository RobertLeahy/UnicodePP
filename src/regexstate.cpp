#include <unicode/regex.hpp>


namespace Unicode {


	RegexState::RegexState (const RegexEngine & engine) noexcept
		:	loc(engine.begin()),
			CanBacktrack(false),
			PreventsBacktracking(false)
	{	}
	
	
	RegexIterator RegexState::Begin () const noexcept {
	
		return loc;
	
	}


	void RegexState::Rewind (RegexEngine & engine) {
	
		if (state) state->Rewind(engine);
		
		engine.begin(loc);
	
	}
	
	
	void RegexState::Set (RegexState & state) const noexcept {
	
		state.CanBacktrack=CanBacktrack;
		state.PreventsBacktracking=PreventsBacktracking;
	
	}
	
	
	void RegexState::Clear () noexcept {
	
		CanBacktrack=false;
		PreventsBacktracking=false;
	
	}


}
