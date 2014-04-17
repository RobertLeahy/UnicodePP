#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/string.hpp>
#include <string>


namespace Catch {


	std::string toString (const Unicode::String & str);
	std::string toString (Unicode::CodePoint cp);
	
	
}
