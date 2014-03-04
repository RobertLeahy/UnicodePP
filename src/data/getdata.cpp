#include <unicode/data.hpp>


#include "data.cpp"


namespace Unicode {


	Array<CodePointInfo> Data () noexcept {
	
		return MakeArray(info);
	
	}
	
	
	Array<Composition> Compositions () noexcept {
	
		return MakeArray(compositions);
	
	}


}
