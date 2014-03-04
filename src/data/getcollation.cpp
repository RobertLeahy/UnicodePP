#include <unicode/data.hpp>


#include "collation.cpp"


namespace Unicode {


	Array<CollationElementTableEntry> Collation () noexcept {
	
		return MakeArray(table);
	
	}


}
