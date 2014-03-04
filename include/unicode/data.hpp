/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <utility>


namespace Unicode {


	Array<CodePointInfo> Data () noexcept;
	Array<Composition> Compositions () noexcept;
	Array<CollationElementTableEntry> Collation () noexcept;


}
