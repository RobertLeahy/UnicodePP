/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <utility>


namespace Unicode {


	Array<CodePointInfo> Data () noexcept;
	Array<Composition> Compositions () noexcept;
	Array<CollationTableEntry> CollationTable () noexcept;


}
