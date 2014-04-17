#include "catch.hpp"
#include <unicode/codepoint.hpp>
#include <unicode/iostream.hpp>
#include <unicode/string.hpp>
#include <iomanip>
#include <sstream>


namespace Catch {


	std::string toString (const Unicode::String & str) {

		std::ostringstream ss;
		ss << "\"" << str << "\"";
		
		return ss.str();

	}
	
	
	std::string toString (Unicode::CodePoint cp) {
	
		std::ostringstream ss;
		ss	<<	"U+"
			<<	std::hex
			<<	std::setfill('0')
			<<	std::setw(4)
			<<	std::uppercase
			<<	static_cast<Unicode::CodePoint::Type>(cp);
		
		return ss.str();
	
	}
	
	
}
