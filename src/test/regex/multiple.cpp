#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to place a multiple qualifier at the beginning of the pattern results in an exception","[multiple][regex]") {

	WHEN("\"*\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("*"),RegexError);
	
	}

}


SCENARIO("* matches greedily at least zero times","[multiple][regex][greedy]") {

	GIVEN("a*") {
	
		Regex r("a*");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto b=s.begin();
			auto e=s.end();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and a zero-length substring at the end of the input") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
				CHECK(matches[1].Get().Size()==0);
				CHECK(matches[1].begin()==e);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaab\"") {
		
			String s("aaaaab");
			auto b=s.begin();
			auto e=s.end();
			auto matches=r.Matches(s);
			
			THEN("It matches \"aaaaa\", a zero-length substring immediately thereafter, and a zero-length substring at the end of the input") {
			
				REQUIRE(matches.size()==3);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()=="aaaaa");
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[1].begin()-b)==5);
				CHECK(matches[2].Get().Size()==0);
				CHECK(matches[2].begin()==e);
			
			}
		
		}
	
	}

}


SCENARIO("Repetition constructs backtrack when greedy behaviour causes over-matching","[multiple][regex][greedy]") {

	GIVEN("a*a") {
	
		Regex r("a*a");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It fails to match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("+ matches greedily at least once","[multiple][greedy][regex]") {

	GIVEN("a+") {
	
		Regex r("a+");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				REQUIRE(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaab\"") {
		
			String s("aaaaab");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches \"aaaaa\"") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()=="aaaaa");
			
			}
		
		}
	
	}

}


SCENARIO("? matches greedily either 0 or 1 times","[multiple][greedy][regex]") {

	GIVEN("a?") {
	
		Regex r("a?");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
			
			}
		
		}
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto b=s.begin();
			auto e=s.end();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and a zero-length substring at the end of the input") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
				CHECK(matches[1].Get().Size()==0);
				CHECK(matches[1].begin()==e);
			
			}
		
		}
		
		WHEN("It is matched against \"ab\"") {
		
			String s("ab");
			auto b=s.begin();
			auto e=s.end();
			auto matches=r.Matches(s);
			
			THEN("It matches \"a\", a zero-length substring immediately thereafter, and a zero-length substring at the end of the input") {
			
				REQUIRE(matches.size()==3);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()=="a");
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[1].begin()-b)==1);
				CHECK(matches[2].Get().Size()==0);
				CHECK(matches[2].begin()==e);
			
			}
		
		}
	
	}

}


SCENARIO("{x,y} matches greedily between x and y times (inclusive)","[multiple][greedy][regex]") {

	GIVEN("a{3,5}") {
	
		Regex r("a{3,5}");
	
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				REQUIRE(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				REQUIRE(matches[0].begin()==s.begin());
				REQUIRE(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				REQUIRE(matches[0].begin()==s.begin());
				REQUIRE(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaaa\"") {
		
			String s("aaaaaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the first five code points of the string") {
			
				REQUIRE(matches.size()==1);
				REQUIRE(matches[0].begin()==s.begin());
				REQUIRE(matches[0].Get()=="aaaaa");
			
			}
		
		}
	
	}

}


SCENARIO("{,y} matches up to y times (inclusive)","[multiple][greedy][regex]") {

	GIVEN("a{,3}") {
	
		Regex r("a{,3}");
	
		WHEN("It is matched against the empty string") {
		
			String s;
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==b);
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and the empty string at the end of the input") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
				CHECK(matches[1].begin()==s.end());
				CHECK(matches[1].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaa\"") {
		
			String s("aaaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches \"aaa\", \"a\", and the empty string at the end of the input") {
			
				REQUIRE(matches.size()==3);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()=="aaa");
				CHECK((matches[1].begin()-3)==b);
				CHECK(matches[1].Get()=="a");
				CHECK(matches[2].begin()==s.end());
				CHECK(matches[2].Get().Size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("{x,}  matches at least x times (inclusive)","[multiple][greedy][regex]") {

	GIVEN("a{3,}") {
	
		Regex r("a{3,}");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			auto matches=r.Matches("aa");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("{x} matches exactly x times","[multiple][greedy][regex]") {

	GIVEN("a{3}") {
	
		Regex r("a{3}");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aa\"") {
		
			auto matches=r.Matches("aa");
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaa\"") {
		
			String s("aaaaa");
			auto matches=r.Matches(s);
			
			THEN("It matches the first three code points") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()=="aaa");
			
			}
		
		}
		
		WHEN("It is matched against \"aaaaaa\"") {
		
			String s("aaaaaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("aaa");
			
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==3);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}


SCENARIO("Greedy repetition constructs may be qualified with + to make them possessive","[possessive][atomic][regex][multiple][greedy][regex]") {

	GIVEN("a*+") {
	
		Regex r("a*+");
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and the empty string at the end of the input") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==s);
				CHECK((matches[1].begin()-b)==3);
				CHECK(matches[1].Get().Size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("a*+a") {
	
		Regex r("a*+a");
		
		WHEN("It is matched against \"a\"") {
		
			auto matches=r.Matches("a");
			
			THEN("It does not match") {
			
				REQUIRE(matches.size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("Repetition constructs may be qualified with ? to make them lazy","[lazy][multiple][regex]") {

	GIVEN("a*?") {
	
		Regex r("a*?");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string at each position in the string, and at the end") {
			
				REQUIRE(matches.size()==4);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get().Size()==0);
				CHECK((matches[1].begin()-b)==1);
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[2].begin()-b)==2);
				CHECK(matches[2].Get().Size()==0);
				CHECK(matches[3].begin()==s.end());
				CHECK(matches[3].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"bbb\"") {
		
			String s("bbb");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string at each position in the string, and at the end") {
			
				REQUIRE(matches.size()==4);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get().Size()==0);
				CHECK((matches[1].begin()-b)==1);
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[2].begin()-b)==2);
				CHECK(matches[2].Get().Size()==0);
				CHECK(matches[3].begin()==s.end());
				CHECK(matches[3].Get().Size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("a*?b") {
	
		Regex r("a*?b");
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It does not match") {
			
				CHECK(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against \"aabaab\"") {
		
			String s("aabaab");
			auto b=s.begin();
			auto matches=r.Matches(s);
			String substr("aab");
			
			THEN("It matches twice") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==substr);
				CHECK((matches[1].begin()-b)==3);
				CHECK(matches[1].Get()==substr);
			
			}
		
		}
	
	}

}
