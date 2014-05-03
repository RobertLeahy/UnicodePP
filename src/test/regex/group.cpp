#include <unicode/regex.hpp>
#include <unicode/regexerror.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Attempting to compile an unterminated non-capturing group throws an exception","[regex][group][regexerror]") {

	WHEN("\"(?:\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?:"),RegexError);
	
	}
	
	WHEN("\"(\" is compiled in explicit capture mode, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(",RegexOptions::ExplicitCapture),RegexError);
	
	}
	
	WHEN("\"(?i:\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?i:"),RegexError);
	
	}

}


SCENARIO("Attempting to compile an unterminated automatically-numbered capturing group throws an exception","[regex][group][regexerror]") {

	WHEN("\"(\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("("),RegexError);
	
	}

}


SCENARIO("Attempting to compile an unterminated named capturing group throws an exception","[regex][group][regexerror]") {

	WHEN("\"(?'test'\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?'test'"),RegexError);
	
	}
	
	WHEN("\"(?<test>\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?<test>"),RegexError);
	
	}
	
	WHEN("\"(?P<test>\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?P<test>"),RegexError);
	
	}

}


SCENARIO("Attempting to compile a non-capturing group which changes options with an unknown option or invalid syntax throws an exception","[regex][group][regexoptions][regexerror]") {

	WHEN("\"(?u:ab)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?u:ab)"),RegexError);
	
	}
	
	WHEN("\"(?-i-m:ab)\" is compiled, an exception is thrown") {
	
		REQUIRE_THROWS_AS(Regex r("(?-i-m:ab)"),RegexError);
	
	}

}


SCENARIO("Non-capturing groups aggregate elements into a single construct, but do not capture","[regex][group]") {

	GIVEN("(?:)") {
	
		Regex r("(?:)");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input exactly once, but does not capture") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get().Size()==0);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("\"()\" in explicit capture mode") {
	
		Regex r("()",RegexOptions::ExplicitCapture);
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input exactly once, but does not capture") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get().Size()==0);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("(?:ab)+") {
	
		Regex r("(?:ab)+");
		
		WHEN("It is matched against \"abab\"") {
		
			String s("abab");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, but does not capture") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("\"(ab)+\" in explicit capture mode") {
	
		Regex r("(ab)+",RegexOptions::ExplicitCapture);
		
		WHEN("It is matched against \"abab\"") {
		
			String s("abab");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, but does not capture") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("Non-capturing groups may be compiled with different options than the rest of the pattern","[regex][group][regexoptions]") {

	GIVEN("(?i:a)") {
	
		Regex r("(?i:a)");
		
		THEN("It matches \"a\"") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It matches \"A\"") {
		
			CHECK(r.Match("A"));
		
		}
	
	}
	
	GIVEN("\"(?-i:a)\" in case insensitive mode") {
	
		Regex r("(?-i:a)",RegexOptions::IgnoreCase);
		
		THEN("It matches \"a\"") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It does not match \"A\"") {
		
			CHECK(!r.Match("A"));
		
		}
	
	}
	
	GIVEN("(?m:^)") {
	
		Regex r("(?m:^)");
	
		WHEN("It is matched against \"\\n\"") {
		
			String s("\n");
			auto matches=r.Matches(s);
			
			THEN("It matches at the beginning of each line") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get().Size()==0);
				CHECK(matches[1].begin()==s.end());
				CHECK(matches[1].Get().Size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("\"(?-m:^)\" in multiline mode") {
	
		Regex r("(?-m:^)",RegexOptions::Multiline);
		
		WHEN("It is matched against \"\\n\"") {
		
			String s("\n");
			auto matches=r.Matches(s);
			
			THEN("It matches at the beginning of the input") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get().Size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("(?n:(a))") {
	
		Regex r("(?n:(a))");
		
		WHEN("It is matched against \"a\"") {
		
			auto matches=r.Matches("a");
			
			THEN("It matches once, but does not capture") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("\"(?-n:(a))\" in explicit capture mode") {
	
		Regex r("(?-n:(a))",RegexOptions::ExplicitCapture);
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches once, and captures the entire input as capturing group 1") {
			
				REQUIRE(matches.size()==1);
				auto & captures=matches[0][1];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?s:.)") {
	
		Regex r("(?s:.)");
		
		THEN("It matches \"\\n\"") {
		
			CHECK(r.Match("\n"));
		
		}
	
	}
	
	GIVEN("\"(?-s:.) in single line mode") {
	
		Regex r("(?-s:.)",RegexOptions::Singleline);
		
		THEN("It does not match \"\\n\"") {
		
			CHECK(!r.Match("\n"));
		
		}
	
	}
	
	GIVEN("(?x:# this is a comment\\n)a") {
	
		Regex r("(?x:# this is a comment\n)a");
		
		THEN("It matches \"a\"") {
		
			CHECK(r.Match("a"));
		
		}
	
	}
	
	GIVEN("\"(?-x:#not a comment)\" in ignore pattern white space mode") {
	
		Regex r("(?-x:#not a comment)",RegexOptions::IgnorePatternWhiteSpace);
		
		THEN("It matches \"#not a comment\"") {
		
			CHECK(r.Match("#not a comment"));
		
		}
		
		THEN("It does not match \"a\"") {
		
			CHECK(!r.Match("a"));
		
		}
	
	}

}


SCENARIO("Substrings may be captured","[regex][group][capturing]") {

	GIVEN("(.)+") {
	
		Regex r("(.)+");
	
		WHEN("It is matched against a string consisting of 2 code points") {
		
			String s("ab");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures each code point") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==1);
				auto & captures=match[1];
				REQUIRE(captures.size()==2);
				CHECK(captures[0].begin()==b);
				CHECK(captures[0].Get()=="a");
				CHECK(captures[1].begin()==(b+1));
				CHECK(captures[1].Get()=="b");
			
			}
		
		}
	
	}

}


SCENARIO("Capturing groups are automatically numbered, starting from 1, ascending","[regex][group][capturing][numbered]") {

	GIVEN("(a)(b)(c)") {
	
		Regex r("(a)(b)(c)");
		
		WHEN("It is matched against \"abc\"") {
		
			String s("abc");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, and captures three different numbered groups") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==3);
				{
					auto & captures=match[1];
					REQUIRE(captures.size()==1);
					CHECK(captures[0].begin()==b);
					CHECK(captures[0].Get()=="a");
				}
				{
					auto & captures=match[2];
					REQUIRE(captures.size()==1);
					CHECK(captures[0].begin()==(b+1));
					CHECK(captures[0].Get()=="b");
				}
				{
				
					auto & captures=match[3];
					REQUIRE(captures.size()==1);
					CHECK(captures[0].begin()==(b+2));
					CHECK(captures[0].Get()=="c");
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Capturing groups may be named","[regex][group][capturing][named]") {

	GIVEN("(?<test>a)") {
	
		Regex r("(?<test>a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group \"test\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==1);
				CHECK(match.Numbered().size()==0);
				auto & captures=match["test"];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?P<test>a)") {
	
		Regex r("(?P<test>a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group \"test\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==1);
				CHECK(match.Numbered().size()==0);
				auto & captures=match["test"];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?'test'a)") {
	
		Regex r("(?'test'a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group \"test\"") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==1);
				CHECK(match.Numbered().size()==0);
				auto & captures=match["test"];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Capturing groups may be numbered","[regex][group][capturing][numbered]") {

	GIVEN("(?<5>a)") {
	
		Regex r("(?<5>a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group 5") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==1);
				auto & captures=match[5];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?P<5>a)") {
	
		Regex r("(?P<5>a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group 5") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==1);
				auto & captures=match[5];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("(?'5'a)") {
	
		Regex r("(?'5'a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input, and captures the entire input in capturing group 5") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==s.begin());
				CHECK(match.Get()==s);
				CHECK(match.Named().size()==0);
				CHECK(match.Numbered().size()==1);
				auto & captures=match[5];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
				CHECK(capture.Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Numbered capturing groups may be accessed with a numeric string","[regex][group][capturing][numbered]") {

	GIVEN("(?<5>a)") {
	
		Regex r("(?<5>a)");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto match=r.Match(s);
			
			THEN("Capturing group 5 may be retrieved as if it were capturing group \"5\"") {
			
				REQUIRE(match);
				auto & captures=(*match)["5"];
				REQUIRE(captures.size()==1);
				auto & capture=captures[0];
				CHECK(capture.begin()==s.begin());
			
			}
		
		}
	
	}

}


SCENARIO("Groups with the same name or number are captured left-to-right","[regex][group][capturing]") {

	GIVEN("(a)+") {
	
		Regex r("(a)+");
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input and capturing group 1 captures 3 times, from left-to-right") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				auto & captures=match[1];
				REQUIRE(captures.size()==3);
				CHECK(captures[0].begin()==b);
				CHECK(captures[1].begin()==(b+1));
				CHECK(captures[2].begin()==(b+2));
			
			}
		
		}
	
	}

}


SCENARIO("Groups with the same name or number are captured right-to-left in right-to-left mode","[regex][group][capturing][righttoleft]") {

	GIVEN("\"(a)+\" in right-to-left mode") {
	
		Regex r("(a)+",RegexOptions::RightToLeft);
		
		WHEN("It is matched against \"aaa\"") {
		
			String s("aaa");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the entire input and capturing group 1 captures 3 times, from right-to-left") {
			
				REQUIRE(matches.size()==1);
				auto & match=matches[0];
				CHECK(match.begin()==b);
				CHECK(match.Get()==s);
				auto & captures=match[1];
				REQUIRE(captures.size()==3);
				CHECK(captures[0].begin()==(b+2));
				CHECK(captures[1].begin()==(b+1));
				CHECK(captures[2].begin()==b);
			
			}
		
		}
	
	}

}
