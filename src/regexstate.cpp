#include <unicode/regex.hpp>


namespace Unicode {


	RegexState::RegexState (const RegexEngine & engine) noexcept
		:	loc(engine.begin()),
			CanBacktrack(false),
			PreventsBacktracking(false)
	{	}


	void RegexState::Rewind (RegexEngine & engine) {
	
		if (state) state->Rewind(engine);
		
		engine.begin(loc);
	
	}


}
