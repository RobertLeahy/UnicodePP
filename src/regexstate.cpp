#include <unicode/regex.hpp>


namespace Unicode {


	void RegexState::Rewind (RegexEngine & engine) {
	
		engine.begin(loc);
		
		if (state) state->Rewind(engine);
	
	}


}
