#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("\\X matches any valid Unicode code point","[regex][validunicode]") {

	GIVEN("\\X") {
	
		Regex r("\\X");
		
		WHEN("It is matched against a valid Unicode code point") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a UTF-16 surrogate") {
		
			String s;
			s << CodePoint(0xD800);
			auto matches=r.Matches(s);
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a code point larger than U+10FFFF") {
		
			String s;
			s << CodePoint(0x10FFFF+1);
			auto matches=r.Matches(s);
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against one of the last two code points in a plane") {
		
			String s;
			s << CodePoint(0x10FFFF);
			auto matches=r.Matches(s);
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}
