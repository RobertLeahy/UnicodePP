#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to compile a recursion specification with an empty group specification results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?)"),RegexError);
	
	}

}


SCENARIO("Attempting to compile an unterminated recursion specification results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?"),RegexError);
	
	}

}


SCENARIO("Attempting to reference a group that does not exist in a recursion specification results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?1)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?1)"),RegexError);
	
	}

}


SCENARIO("Attempting to reference a relative group that would lie before the beginning of the pattern results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?-1)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?-1)"),RegexError);
	
	}

}


SCENARIO("Attempting to reference a 0th relative group results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?-0)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?-0)"),RegexError);
	
	}
	
	WHEN("\"(?+0)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?+0)"),RegexError);
	
	}

}


SCENARIO("Attempting to reference a relative group that will never be found results in an exception","[regex][recursion][regexerror][error]") {

	WHEN("\"(?+1)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?+1)"),RegexError);
	
	}

}


SCENARIO("Numbered groups may be recursed","[regex][recursion]") {

	GIVEN("(.)(?1)") {
	
		Regex r("(.)(?1)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string one code point in length") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two different code points") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Named groups may be recursed","[regex][recursion]") {

	GIVEN("(?<test>.)(?&test)") {
	
		Regex r("(?<test>.)(?&test)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string one code point in length") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two different code points") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?<test>.)(?P>test)") {
	
		Regex r("(?<test>.)(?P>test)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string one code point in length") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two different code points") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Relative groups may be recursed","[regex][recursion]") {

	GIVEN("(.)(?-1)") {
	
		Regex r("(.)(?1)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string one code point in length") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two different code points") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?+1)(.)") {
	
		Regex r("(.)(?1)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string one code point in length") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing the same code point twice") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two different code points") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("The entire pattern may be recursed","[regex][recursion]") {

	GIVEN("a(?:b|(?R))") {
	
		Regex r("a(?:b|(?R))");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"a\"") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaab\"") {
		
			String s("aaaab");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}
