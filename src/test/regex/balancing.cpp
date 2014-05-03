#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to compile an unterminated balancing construct throws an exception","[regex][balancing][regexerror]") {

	WHEN("\"(a)(?<-1>\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(a)(?<-1>"),RegexError);
	
	}

}


SCENARIO("Attempting to compile a balancing construct without a second group throws an exception","[regex][balancing][regexerror]") {

	WHEN("\"(?<->a)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?<->a)"),RegexError);
	
	}

}


SCENARIO("Balancing groups capture everything between itself and the target group, and delete the latest target capture","[regex][balancing][capturing]") {

	GIVEN("(a)b*(?'2-1')") {
	
		Regex r("(a)b*(?'2-1')");
		
		WHEN("It is matched against \"abbb\"") {
		
			String s("abbb");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, capturing group 1 contains nothing, and capturing group 2 contains \"bbb\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				CHECK(match[1].size()==0);
				auto & captures=match[2];
				REQUIRE(captures.size()==1);
				CHECK(captures[0].begin()==(b+1));
				CHECK(captures[0].Get()=="bbb");
			
			}
		
		}
	
	}

}


SCENARIO("Balancing groups not explicitly named or numbered are automatically numbered","[regex][balancing][capturing]") {

	GIVEN("(a)b*(?'-1')") {
	
		Regex r("(a)b*(?'-1')");
		
		WHEN("It is matched against \"abbb\"") {
		
			String s("abbb");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, capturing group 1 contains nothing, and capturing group 2 contains \"bbb\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				CHECK(match[1].size()==0);
				auto & captures=match[2];
				REQUIRE(captures.size()==1);
				CHECK(captures[0].begin()==(b+1));
				CHECK(captures[0].Get()=="bbb");
			
			}
		
		}
	
	}

}


SCENARIO("Balancing groups can match their own subpatterns","[regex][balancing][capturing]") {

	GIVEN("(a)+(?:a|b)+(?'-1'c)+") {
	
		Regex r("(a)+(?:a|b)+(?'-1'c)+");
		
		WHEN("It is matched against \"aabbbcc\"") {
		
			String s("aabbbcc");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, capturing group 1 does not capture, and capturing group 2 captures \"bbb\" and \"abbbc\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				CHECK(match[1].size()==0);
				auto & captures=match[2];
				REQUIRE(captures.size()==2);
				CHECK(captures[0].begin()==(b+2));
				CHECK(captures[0].Get()=="bbb");
				CHECK(captures[1].begin()==(b+1));
				CHECK(captures[1].Get()=="abbbc");
			
			}
		
		}
	
	}

}


SCENARIO("Balancing groups do not match unless the referenced group has a capture","[regex][balancing][capturing]") {

	GIVEN("(a)?b*(?'-1')") {
	
		Regex r("(a)?b*(?'-1')");
		
		THEN("It does not match \"bbbb\"") {
		
			CHECK(!r.Match("bbbb"));
		
		}
	
	}

}


SCENARIO("Balancing groups operate in right-to-left mode","[regex][balancing][capturing][righttoleft]") {

	GIVEN("\"(?'-2')b*(c)?\" in right-to-left mode") {
	
		Regex r("(?'-2')b*(c)?",RegexOptions::RightToLeft);
		
		THEN("It matches \"bbbc\"") {
		
			CHECK(r.Match("bbbc"));
		
		}
		
		THEN("It does not match \"bbb\"") {
		
			CHECK(!r.Match("bbb"));
		
		}
	
	}

}
