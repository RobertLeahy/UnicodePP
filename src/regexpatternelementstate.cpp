#include <unicode/regex.hpp>


namespace Unicode {


	void RegexPatternElementState::Rewind (RegexState & state) {
	
		state.begin(loc);
		
		if (this->state) this->state->Rewind(state);
	
	}


}
