#include <unicode/data.hpp>


#include "collation.cpp"


namespace Unicode {


	Array<CollationTableEntry> CollationTable () noexcept {
	
		return MakeArray(table);
	
	}


}
