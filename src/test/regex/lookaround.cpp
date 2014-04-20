#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Positive lookahead asserts that a certain match proceeds from a point, without consuming any of the substring matched","[lookaround][lookahead][regex]") {

	GIVEN("a(?=a)") {
	
		Regex r("a(?=a)");
		
		WHEN("It is matched against \"a\"") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"ab\"") {
		
			auto matches=r.Matches("ab");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("a");
			
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==1);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}


SCENARIO("Capturing groups in lookarounds are released during backtracking","[lookaround][capturing][backtracking][regex]") {

	GIVEN("a*?(?=.(.))b") {
	
		Regex r("a*?(?=.(.))b");
		
		WHEN("It is matched against \"aabc\"") {
		
			String s("aabc");
			auto match=r.Match(s);
			
			THEN("It matches") {
			
				REQUIRE(match);
				
				auto capture=(*match)[1];
				
				THEN("The first capturing group captures exactly once, and captures \"c\"") {
				
					REQUIRE(capture.size()==1);
					CHECK((capture[0].begin()-s.begin())==3);
					CHECK(capture[0].Get()=="c");
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Attempting to compile an unterminated lookaround results in an exception","[lookaround][regex]") {

	WHEN("\"(?=a\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?=a"),RegexError);
	
	}

}


SCENARIO("Positive lookaheads may be variable width","[lookahead][lookaround][regex]") {

	GIVEN("a(?=.*?a)") {
	
		Regex r("a(?=.*?a)");
		
		WHEN("It is matched against \"aba\"") {
		
			String s("aba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"abba\"") {
		
			String s("abba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"ababba\"") {
		
			String s("ababba");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("a");
		
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==2);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}


SCENARIO("Negative lookahead asserts that a certain match does not proceed from a point, without consuming any of the substring matched","[lookaround][lookahead][regex]") {

	GIVEN("a(?!a)") {
	
		Regex r("a(?!a)");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
	
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"ab\"") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, from the second code point onwards") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-1)==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"abab\"") {
		
			String s("abab");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("a");
			
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==2);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}


SCENARIO("Negative lookaheads may be variable width","[lookahead][lookaround][regex]") {

	GIVEN("a(?!.*?a)") {
	
		Regex r("a(?!.*?a)");
		
		WHEN("It is matched against \"ab\"") {
		
			String s("ab");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"abbbba\"") {
		
			String s("abbbba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, immediately before the end of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-s.begin())==5);
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
	
	}

}


SCENARIO("Positive lookbehind asserts that a certain match precedes a point, without consuming any of the substring matched","[lookbehind][lookaround][regex]") {

	GIVEN("(?<=a)a") {
	
		Regex r("(?<=a)a");
	
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-s.begin())==1);
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"ba\"") {
		
			auto matches=r.Matches("ba");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"baaab\"") {
		
			String s("baaab");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("a");
			
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK((matches[0].begin()-b)==2);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==3);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}


SCENARIO("Positive lookbehinds may be variable width","[lookbehind][lookaround][regex]") {

	GIVEN("(?<=a.*?)a") {
	
		Regex r("(?<=a.*?)a");
	
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"abbbba\"") {
		
			String s("abbbba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-s.begin())==5);
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"bbbba\"") {
		
			auto matches=r.Matches("bbbba");
			
			THEN("It does not match") {
			
				REQUIRE(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("Negative lookbehind asserts that a certain match does not precede a point, without consuming any of the substring matched","[lookbehind][lookaround][regex]") {

	GIVEN("(?<!a)a") {
		
		Regex r("(?<!a)a");
	
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"ba\"") {
		
			String s("ba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-s.begin())==1);
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
	
	}

}


SCENARIO("Negative lookbehinds may be variable width","[lookbehind][lookaround][regex]") {

	GIVEN("(?<!a.*?)a") {
	
		Regex r("(?<!a.*?)a");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			String s("aa");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"ba\"") {
		
			String s("ba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK((matches[0].begin()-s.begin())==1);
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
		
		WHEN("It is matched against \"abbbbba\"") {
		
			String s("abbbbba");
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once, at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="a");
			
			}
		
		}
	
	}

}
