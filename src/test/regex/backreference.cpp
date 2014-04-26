#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Not terminating a \\k backreference results in an exception","[regex][backreference][regexerror]") {

	WHEN("\"\\k<1\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("\\k<1"),RegexError);
	
	}

}


SCENARIO("Referring to an empty group fails unconditionally","[regex][backreference]") {

	GIVEN("(?:(a)|(b))\\k<1>") {
	
		Regex r("(?:(a)|(b))\\k<1>");
		
		WHEN("It is matched against \"b\"") {
		
			auto matches=r.Matches("b");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("Backreferences always refer to the most recent capture made by the capturing group-in-question","[regex][backreference]") {

	GIVEN("(?'1'.)(?'1'.)\\k<1>") {
	
		Regex r("(?'1'.)(?'1'.)\\k<1>");
		
		WHEN("It is matched against \"abb\"") {
		
			String s("abb");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"bab\"") {
		
			auto matches=r.Matches("bab");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("\\k backreferences may refer to numbered capturing groups","[regex][backreference]") {

	GIVEN("(.)\\k<1>") {
	
		Regex r("(.)\\k<1>");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against one code point") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against two different code points") {
		
			auto matches=r.Matches("ab");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("\\k backreferences may refer to named capturing groups","[regex][backreference]") {

	GIVEN("(?'test'.)\\k<test>") {
	
		Regex r("(?'test'.)\\k<test>");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against one code point") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against two different code points") {
		
			auto matches=r.Matches("ab");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("\\ backreferences may refer to numbered capturing groups","[regex][backreference]") {

	GIVEN("(.)\\1") {
	
		Regex r("(.)\\1");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against one code point") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against two different code points") {
		
			auto matches=r.Matches("ab");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("Backreferences ignore case when RegexOptions::IgnoreCase is set","[regex][backreference][ignorecase]") {

	GIVEN("(.)\\1 with RegexOptions::IgnoreCase") {
	
		Regex r("(.)\\1",RegexOptions::IgnoreCase);
		
		WHEN("It is matched against \"aA\"") {
		
			String s("aA");
			auto matches=r.Matches(s);
		
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}
