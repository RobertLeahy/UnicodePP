#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to compile a range with both end points equal results in an error","[regex][range][characterset][regexerror]") {

	WHEN("A range with both end points equal is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("[a-a]"),RegexError);
	
	}

}


SCENARIO("Attempting to compile a range with the higher code point first results in an error","[regex][range][characterset][regexerror]") {

	WHEN("A range with the higher code point first is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("[z-a]"),RegexError);
	
	}

}


SCENARIO("Ranges match any code points that lie between the start and end point (inclusive)","[regex][range][characterset]") {

	GIVEN("A pattern containing a range") {
	
		Regex r("[a-z]");
		
		WHEN("It is matched against the empty string") {
			
			THEN("It does not match") {
			
				CHECK(!r.Match(String{}));
			
			}
		
		}
		
		WHEN("It is matched against a string containing only code points outside that range") {
		
			String s("ZA./");
			
			THEN("It does not match") {
			
				CHECK(!r.Match(String{}));
			
			}
		
		}
		
		WHEN("It is matched against the beginning of the range") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against the end of the range") {
		
			String s("z");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point that is within the range, but is neither the beginning of the end thereof") {
		
			String s("h");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("In case insensitive mode, ranges match against simple case foldings","[range][regex][characterset][ignorecase]") {

	GIVEN("A pattern containing a range, both end points of which have only simple case foldings, compiled with RegexOptions::IgnoreCase") {
	
		Regex r("[a-z]",RegexOptions::IgnoreCase);
		
		WHEN("It is matched against a character from within the range") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a character not within the range, but whose case folding is within the range") {
		
			String s("A");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a character not within the range, and whose case folding is not within the range") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("1"));
			
			}
		
		}
	
	}

}


SCENARIO("An unterminated range behaves as if it were the first code point followed by a hyphen","[regex][range][characterset]") {

	GIVEN("An unterminated range") {
	
		Regex r("[a-]");
		
		WHEN("It is matched against the beginning of the range") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against the hyphen") {
		
			String s("-");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against some code point whose value is higher than the beginning's") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("i"));
			
			}
		
		}
	
	}

}


SCENARIO("Escapes may be the used for the end points of a range","[range][regex][characterset]") {

	GIVEN("A range which begins with an escape") {
	
		Regex r("[\\u0061-z]");
		
		WHEN("It is matched against a code point within the range") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point not within the range") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("A"));
			
			}
		
		}
	
	}
	
	GIVEN("A range which ends with an escape") {
	
		Regex r("[a-\\u007A]");
		
		WHEN("It is matched against a code point within the range") {
		
			String s("z");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point not within the range") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("A"));
			
			}
		
		}
	
	}
	
	GIVEN("A range which begins and ends with an escape") {
	
		Regex r("[\\u0061-\\u007A]");
		
		WHEN("It is matched against a code point within the range") {
		
			String s("z");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point not within the range") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("A"));
			
			}
		
		}
	
	}

}
