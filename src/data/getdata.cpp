#include <unicode/data.hpp>
#include <iterator>


#include "data.cpp"


namespace Unicode {


	const CodePointInfo * DataBegin () noexcept {
	
		return std::begin(info);
	
	}
	
	
	const CodePointInfo * DataEnd () noexcept {
	
		return std::end(info);
	
	}


}
