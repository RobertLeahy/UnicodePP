#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO(". matches any code point except LINE FEED (LF) (U+000A)","[wildcard][regex]") {

	GIVEN(".") {
	
		Regex r(".");
		
		THEN("It matches letters") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It matches digits") {
		
			CHECK(r.Match("1"));
		
		}
		
		THEN("It matches symbols") {
		
			CHECK(r.Match("\\"));
		
		}
		
		THEN("It does not match newlines") {
		
			CHECK(!r.Match("\n"));
		
		}
		
		THEN("It matches carriage returns") {
		
			CHECK(r.Match("\r"));
		
		}
	
	}

}


SCENARIO(". matches any code point in single line mode","[wildcard][singleline][regex]") {

	GIVEN(".") {
	
		GIVEN("RegexOptions::Singleline is set") {
		
			Regex r(".",RegexOptions::Singleline);
			
			THEN("It matches newlines") {
			
				CHECK(r.Match("\n"));
			
			}
		
		}
	
	}

}
