#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Strings may be split","[split]") {

	GIVEN("A regular expression which matches between each word") {
	
		String pattern("\\s+");
	
		GIVEN("RegexOptions::RightToLeft is not set") {
	
			Regex r(pattern);
			
			WHEN("It is used to split the empty string") {
			
				auto split=r.Split(String{});
				
				THEN("The empty string is the only result") {
				
					REQUIRE(split.size()==1);
					CHECK(split[0].Size()==0);
				
				}
			
			}
			
			WHEN("It is used to split a string containing only one word") {
			
				String str("hello");
				auto split=r.Split(str);
				
				THEN("The only result is the word itself") {
				
					REQUIRE(split.size()==1);
					CHECK(split[0]==str);
				
				}
			
			}
			
			WHEN("It is used to split a string containing trailing and leading whitespace") {
			
				auto split=r.Split(" hello ");
				
				THEN("It is split into the empty string, the word, and the empty string, respectively") {
				
					REQUIRE(split.size()==3);
					CHECK(split[0].Size()==0);
					CHECK(split[1]=="hello");
					CHECK(split[2].Size()==0);
				
				}
			
			}
			
			WHEN("It is used to split a string containing trailing and leading whitespace, and empty splits are ignored") {
			
				auto split=r.Split(" hello ",true);
				
				THEN("The only result is the word") {
				
					REQUIRE(split.size()==1);
					CHECK(split[0]=="hello");
				
				}
			
			}
			
			WHEN("It is used to split a string containing many words") {
			
				auto split=r.Split("hello good-bye");
				
				THEN("The result is each word") {
				
					REQUIRE(split.size()==2);
					CHECK(split[0]=="hello");
					CHECK(split[1]=="good-bye");
				
				}
			
			}
			
		}
		
		GIVEN("RegexOptions::RightToLeft is set") {
		
			Regex r(pattern,RegexOptions::RightToLeft);
			
			WHEN("It is used to split a string containing many words") {
			
				auto split=r.Split("hello good-bye");
				
				THEN("The result is each word in reverse order") {
				
					REQUIRE(split.size()==2);
					CHECK(split[0]=="good-bye");
					CHECK(split[1]=="hello");
				
				}
			
			}
		
		}
	
	}

}
