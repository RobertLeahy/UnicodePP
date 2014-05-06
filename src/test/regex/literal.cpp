#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Sequences not specially interpreted by the regular expression engine match themselves","[literal][regex]") {

	GIVEN("A regular expression pattern which contains a literal sequence which does not change under NFD") {
	
		Regex r("abc");
		
		WHEN("It is matched against the same sequence") {
		
			String s("abc");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against the same sequence, twice") {
		
			String s("abcabc");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches twice") {
			
				String ss("abc");
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get()==ss);
				CHECK(matches[1].begin()==(b+3));
				CHECK(matches[1].Get()==ss);
			
			}
		
		}
		
		WHEN("It is matched against a different sequence, which does not contain that literal sequence as a substring") {
			
			String s("def");
			
			THEN("It does not match") {
			
				CHECK(!r.Match(s));
			
			}
		
		}
		
		WHEN("It is matched against the empty string") {
		
			String s;
			
			THEN("It does not match") {
			
				CHECK(!r.Match(s));
			
			}
		
		}
	
	}
	
	GIVEN("A regular expression pattern which contains a literal sequence which changes under NFD") {
	
		Regex r("\u00E1");
		
		WHEN("It is matched against the same sequence") {
		
			String s("\u00E1");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, but due to normalization the begin iterator is not the string's begin iterator") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()!=s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against an equivalent sequence in NFD") {
		
			String s("a\u0301");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("A regular expression which contains a literal sequence which is in NFD, but changes under NFC") {
	
		Regex r("a\u0301");
		
		WHEN("It is matched against an equivalent sequence in NFC") {
		
			String s("\u00E1");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string, but due to normalization the begin iterator is not the string's begin iterator") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()!=s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Sequences not specially interpreted by the regular expression engine match case insensitively when case is ignored","[literal][ignorecase][regex]") {

	GIVEN("A pattern consisting only of literals with simple case foldings with RegexOptions::IgnoreCase set") {
	
		Regex r("aBc",RegexOptions::IgnoreCase);
		
		WHEN("It is matched against itself") {
		
			String s("aBc");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against itself, converted to lower case") {
		
			String s("abc");
			auto matches=r.Matches(s);
		
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against itself, converted to upper case") {
		
			String s("ABC");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}
	
	GIVEN("A pattern consisting of literals with full case foldings with RegexOptions::IgnoreCase set") {
	
		//	LATIN SMALL LETTER SHARP S (U+00DF)
		Regex r("ß",RegexOptions::IgnoreCase);
		
		WHEN("It is matched against itself") {
		
			String s("ß");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against an equivalent same case representation of itself") {
		
			String s("ss");
			auto matches=r.Matches(s);
		
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against itself, converted to upper case") {
		
			String s("SS");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("White space in the pattern is ignored when the appropriate option is set","[literal][ignorepatternwhitespace][regex]") {

	GIVEN("A pattern consisting entirely of white space with RegexOptions::IgnorePatternWhiteSpace set") {
	
		Regex r(" ",RegexOptions::IgnorePatternWhiteSpace);
		
		WHEN("It is matched against the empty string") {
		
			auto matches=r.Matches(String{});
			
			THEN("It matches exactly once") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].Get().Size()==0);
			
			}
		
		}
	
	}
	
	GIVEN("A pattern consisting partially of white space with RegexOptions::IgnorePatternWhiteSpace set") {
	
		Regex r(" a   b    c   ",RegexOptions::IgnorePatternWhiteSpace);
		
		WHEN("It is matched against itself without the white space") {
		
			String s("abc");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("White space in the pattern will not be ignored when RegexOptions::IgnorePatternWhiteSpace is set if escaped with a reverse solidus","[regex][literal][ignorepatternwhitespace]") {

	GIVEN("A pattern consisting entirely of white space escaped by the reverse solidus with RegexOptions::IgnorePatternWhiteSpace set") {
	
		Regex r("\\ ",RegexOptions::IgnorePatternWhiteSpace);
		
		WHEN("It is matched against the empty string") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match(String{}));
			
			}
		
		}
		
		WHEN("it is matched against the white space sequence") {
		
			String s(" ");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("White space in a character set is not ignored, even when RegexOptions::IgnorePatternWhiteSpace is set","[literal][ignorepatternwhitespace][regex][characterset]") {

	GIVEN("A pattern containing a character set containing white space with RegexOptions::IgnorePatternWhiteSpace set") {
	
		Regex r("[ a]",RegexOptions::IgnorePatternWhiteSpace);
		
		WHEN("It is matched against the white space code point") {
		
			String s(" ");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
	
	}

}


SCENARIO("Code points may be given as octal","[literal][regex]") {

	GIVEN("\\141") {
	
		Regex r("\\141");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than 'a'") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("Code points may be given as hexadecimal","[literal][regex]") {

	GIVEN("\\x61") {
	
		Regex r("\\x61");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than 'a'") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}
	
	GIVEN("\\u0061") {
	
		Regex r("\\u0061");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than 'a'") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}
	
	GIVEN("\\U00000061") {
	
		Regex r("\\U00000061");
		
		WHEN("It is matched against \"a\"") {
		
			String s("a");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than 'a'") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\b\" indicates BACKSPACE (U+0008), but only within a character set","[literal][regex][characterset]") {

	GIVEN("[\\b]") {
	
		Regex r("[\\b]");
		
		WHEN("It is matched against BACKSPACE (U+0008)") {
		
			String s("\u0008");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than BACKSPACE (U+0008)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\r\" indicates CARRIAGE RETURN (CR) (U+000D)","[literal][regex]") {

	GIVEN("\\r") {
	
		Regex r("\\r");
		
		WHEN("It is matched against CARRIAGE RETURN (CR) (U+000D)") {
		
			String s("\u000D");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than CARRIAGE RETURN (CR) (U+000D)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\n\" indicates LINE FEED (LF) (U+000A)","[literal][regex]") {

	GIVEN("\\n") {
	
		Regex r("\\n");
		
		WHEN("It is matched against LINE FEED (LF) (U+000A)") {
		
			String s("\u000A");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than LINE FEED (LF) (U+000A)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\t\" indicates CHARACTER TABULATION (U+0009)","[literal][regex]") {

	GIVEN("\\t") {
	
		Regex r("\\t");
		
		WHEN("It is matched against CHARACTER TABULATION (U+0009)") {
		
			String s("\u0009");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than CHARACTER TABULATION (U+0009)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\f\" indicates FORM FEED (FF) (U+000C)","[literal][regex]") {

	GIVEN("\\f") {
	
		Regex r("\\f");
		
		WHEN("It is matched against FORM FEED (FF) (U+000C)") {
		
			String s("\u000C");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than FORM FEED (FF) (U+000C)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\e\" indicates ESCAPE (U+001B)","[literal][regex]") {

	GIVEN("\\e") {
	
		Regex r("\\e");
		
		WHEN("It is matched against ESCAPE (U+001B)") {
		
			String s("\u001B");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than ESCAPE (U+001B)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\v\" indicates LINE TABULATION (U+000B)","[literal][regex]") {

	GIVEN("\\v") {
	
		Regex r("\\v");
		
		WHEN("It is matched against LINE TABULATION (U+000B)") {
		
			String s("\u000B");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than LINE TABULATION (U+001B)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}


SCENARIO("\"\\a\" indicates BELL (U+0007)","[literal][regex]") {

	GIVEN("\\a") {
	
		Regex r("\\a");
		
		WHEN("It is matched against BELL (U+0007)") {
		
			String s("\u0007");
			auto matches=r.Matches(s);
			
			THEN("It matches the entire string") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get()==s);
			
			}
		
		}
		
		WHEN("It is matched against a code point other than BELL (U+0007)") {
		
			THEN("It does not match") {
			
				CHECK(!r.Match("b"));
			
			}
		
		}
	
	}

}
