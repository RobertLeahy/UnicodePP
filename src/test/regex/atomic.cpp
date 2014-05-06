#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to compile an unterminated atomic group results in an exception","[regexerror][regex][atomic][group]") {

	WHEN("An unterminated atomic group is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?>a"),RegexError);
	
	}

}


SCENARIO("Atomic groups match everything within","[regex][atomic][group]") {

	GIVEN("(?>a)") {
	
		Regex r("(?>a)");
		
		WHEN("It is matched against the empty string") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match(String{}));
			
			}
		
		}
		
		WHEN("It is matched against \"b\"") {

			THEN("It does not match") {
			
				CHECK(!r.Match(String{}));
			
			}
		
		}
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
		
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto b=s.begin();
			String ss("a");
			auto matches=r.Matches(s);
			
			THEN("It matches \"a\" twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==ss);
				CHECK(matches[1].begin()==(b+1));
				CHECK(matches[1].Get()==ss);
			
			}
		
		}
	
	}

}


SCENARIO("Atomic groups may backtrack internally","[regex][backtracking][atomic][group]") {

	GIVEN("(?>a*a)") {
	
		Regex r("(?>a*a)");
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Atomic groups may be backtracked over if they do not successfully match","[regex][backtracking][atomic][group]") {

	GIVEN("a*(?>a)") {
	
		Regex r("a*(?>a)");
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Once an atomic group has matched, it may not be backtracked over","[regex][backtracking][atomic][group]") {

	GIVEN("(?>a*)a") {
	
		Regex r("(?>a*)a");
		
		WHEN("It is matched against \"aaa\"") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("aaa"));
			
			}
		
		}
	
	}

}
