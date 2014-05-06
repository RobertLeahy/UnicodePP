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
