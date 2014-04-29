#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("\\K resets the beginning of the match","[resetmatch][regex]") {

	GIVEN("(a\\Kb)") {
	
		Regex r("(a\\Kb)");
		
		WHEN("It is matched against \"b\"") {
		
			auto matches=r.Matches("b");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
		WHEN("It is matched against \"ab\"") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches the last code point, but the first capturing group captures the entire input") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==(s.begin()+1));
				CHECK(match.Get()=="b");
				auto & captures=match[1];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("\\K resets the end of the match in right-to-left mode","[resetmatch][regex][righttoleft]") {

	GIVEN("(a\\Kb) in right-to-left mode") {
	
		Regex r("(a\\Kb)",RegexOptions::RightToLeft);
		
		WHEN("It is matched against \"a\"") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
		WHEN("It is matched against \"ab\"") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches the first code point, but the first capturing group captures the entire input") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()=="a");
				auto & captures=match[1];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("\\K does not reset the beginning of the match if it is backtracked over","[resetmatch][regex][backtracking]") {

	GIVEN("a\\Kb|ac") {
	
		Regex r("a\\Kb|ac");
		
		WHEN("It is matched against \"ac\"") {
		
			String s("ac");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}
