#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("^ matches at the beginning of input and the beginning of lines (as appropriate)","[anchors]") {

	GIVEN("^") {
	
		GIVEN("RegexOptions::Multiline is not set") {
	
			Regex r("^");
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
		}
		
		GIVEN("RegexOptions::Multiline is set") {
		
			Regex r("^",RegexOptions::Multiline);
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN(
					"It matches a number of times equal to the number of lines, "
					"once at the beginning of each line, "
					"and each match is the empty string"
				) {
				
					REQUIRE(matches.size()==4);
					CHECK((matches[0].begin()-b)==0);
					CHECK(matches[0].Get().Size()==0);
					CHECK((matches[1].begin()-b)==2);
					CHECK(matches[1].Get().Size()==0);
					CHECK((matches[2].begin()-b)==4);
					CHECK(matches[2].Get().Size()==0);
					CHECK((matches[3].begin()-b)==6);
					CHECK(matches[3].Get().Size()==0);
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("$ matches at the end of input, before a final \\n at the end of input, at the end of lines (as appropriate), and before a final \\n at the end of a line (as appropriate","[anchors]") {

	GIVEN("$") {
	
		GIVEN("RegexOptions::Multiline is not set") {
		
			Regex r("$");
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line with a trailing \\n") {
			
				String s("hello world\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::Multiline is set") {
		
			Regex r("$",RegexOptions::Multiline);
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line with a trailing \\n") {
			
				String s("hello world\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before each \\n, at the end of each line, and at the end of the input") {
				
					REQUIRE(matches.size()==6);
					CHECK((matches[0].begin()-b)==1);
					CHECK(matches[0].Get().Size()==0);
					CHECK((matches[1].begin()-b)==2);
					CHECK(matches[1].Get().Size()==0);
					CHECK((matches[2].begin()-b)==3);
					CHECK(matches[2].Get().Size()==0);
					CHECK((matches[3].begin()-b)==4);
					CHECK(matches[3].Get().Size()==0);
					CHECK((matches[4].begin()-b)==5);
					CHECK(matches[4].Get().Size()==0);
					CHECK((matches[5].begin()-b)==6);
					CHECK(matches[5].Get().Size()==0);
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("\\A matches at the beginning of input","[anchors]") {

	GIVEN("\\A") {
	
		GIVEN("RegexOptions::Multiline is not set") {
		
			Regex r("\\A");
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
		
		}
	
		GIVEN("RegexOptions::Multiline is set") {
	
			Regex r("\\A",RegexOptions::Multiline);
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the beginning of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
		}
	
	}

}


SCENARIO("\\Z matches at the end of input, and before a final \\n at the end of input","[anchors]") {

	GIVEN("\\Z") {
	
		GIVEN("RegexOptions::Multiline is not set") {
		
			Regex r("\\Z");
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line with a trailing \\n") {
			
				String s("hello world\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::Multiline is set") {
		
			Regex r("\\Z",RegexOptions::Multiline);
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string before the trailing \\n, and at the end of the input") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==(e-1));
					CHECK(matches[0].Get().Size()==0);
					CHECK(matches[1].begin()==e);
					CHECK(matches[1].Get().Size()==0);
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("\\z matches at the end of input","[anchors]") {

	GIVEN("\\z") {
	
		GIVEN("RegexOptions::Multiline is not set") {
		
			Regex r("\\z");
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line") {
			
				String s("hello world");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing one line with a trailing \\n") {
			
				String s("hello world\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::Multiline is set") {
		
			Regex r("\\z",RegexOptions::Multiline);
			
			WHEN("It is matched against a string containing many lines") {
			
				String s("a\nb\nc\n");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string exactly once, at the end of the input") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("\\G matches at the point where the last match ended, or where the engine began attempting to match","[anchors]") {

	GIVEN("\\G") {
	
		String pattern("\\G");
	
		GIVEN("RegexOptions::RightToLeft is not set") {
		
			Regex r(pattern);
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string once, at the beginning of the string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a non-empty string") {
			
				String s("hello world");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string once, at the beginning of the string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::RightToLeft is set") {
		
			Regex r(pattern,RegexOptions::RightToLeft);
			
			WHEN("It is matched against the empty string") {
			
				String s;
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string once, at the end of the string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
			
			WHEN("It is matched against a non-empty string") {
			
				String s("hello world");
				auto e=s.end();
				auto matches=r.Matches(s);
				
				THEN("It matches the empty string once, at the end of the string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==e);
					CHECK(matches[0].Get().Size()==0);
				
				}
			
			}
		
		}
	
	}

	GIVEN("\\Ghello") {
	
		String pattern("\\Ghello");
	
		GIVEN("RegexOptions::RightToLeft is not set") {
		
			Regex r(pattern);
			
			WHEN("It is matched against the string \"hello\"") {
			
				String s("hello");
				auto matches=r.Matches(s);
				
				THEN("It matches the entire string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].Get()==s);
				
				}
			
			}
			
			WHEN("It is matched against the string \" hello\"") {
			
				String s(" hello");
				auto matches=r.Matches(s);
				
				THEN("It does not match") {
				
					REQUIRE(matches.size()==0);
				
				}
			
			}
			
			WHEN("It is matched against the string \"hello \"") {
			
				String s("hello ");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches \"hello\"") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get()=="hello");
				
				}
			
			}
			
			WHEN("It is matched against the string \"hellohello hello\"") {
			
				String s("hellohello hello");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the first two occurrences of \"hello\", but not the last one") {
				
					REQUIRE(matches.size()==2);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get()=="hello");
					CHECK((matches[1].begin()-b)==5);
					CHECK(matches[0].Get()=="hello");
				
				}
			
			}
			
			WHEN("It is matched against the string \"hello hellohello\"") {
			
				String s("hello hellohello");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the first occurrence of \"hello\", but not the last two") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].begin()==b);
					CHECK(matches[0].Get()=="hello");
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::RightToLeft is set") {
		
			Regex r(pattern,RegexOptions::RightToLeft);
			
			WHEN("It is matched against a string") {
			
				String s("hello");
				auto matches=r.Matches(s);
				
				THEN("It does not match") {
				
					CHECK(matches.size()==0);
				
				}
			
			}
		
		}
	
	}

	GIVEN("hello\\G") {
	
		String pattern("hello\\G");
		
		GIVEN("RegexOptions::RightToLeft is not set") {
		
			Regex r(pattern);
			
			WHEN("It is matched against a string") {
			
				String s("hello");
				auto matches=r.Matches(s);
				
				THEN("It does not match") {
				
					CHECK(matches.size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("RegexOptions::RightToLeft is set") {
		
			Regex r(pattern,RegexOptions::RightToLeft);
			
			WHEN("It is matched against the string \"hello\"") {
			
				String s("hello");
				auto matches=r.Matches(s);
				
				THEN("It matches the entire string") {
				
					REQUIRE(matches.size()==1);
					CHECK(matches[0].Get()==s);
				
				}
			
			}
			
			WHEN("It is matched against the string \" hello\"") {
			
				String s(" hello");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches \"hello\"") {
				
					REQUIRE(matches.size()==1);
					CHECK((matches[0].begin()-b)==1);
					CHECK(matches[0].Get()=="hello");
				
				}
			
			}
			
			WHEN("It is matched against the string \"hello \"") {
			
				String s("hello ");
				auto matches=r.Matches(s);
				
				THEN("It does not match") {
				
					REQUIRE(matches.size()==0);
				
				}
			
			}
			
			WHEN("It is matched against the string \"hellohello hello\"") {
			
				String s("hellohello hello");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the last occurence of \"hello\", but not the first two") {
				
					REQUIRE(matches.size()==1);
					CHECK((matches[0].begin()-b)==11);
					CHECK(matches[0].Get()=="hello");
				
				}
			
			}
			
			WHEN("It is matched against the string \"hello hellohello\"") {
			
				String s("hello hellohello");
				auto b=s.begin();
				auto matches=r.Matches(s);
				
				THEN("It matches the last two occurences of \"hello\", but not the first one") {
				
					REQUIRE(matches.size()==2);
					CHECK((matches[0].begin()-b)==11);
					CHECK(matches[0].Get()=="hello");
					CHECK((matches[1].begin()-b)==6);
					CHECK(matches[1].Get()=="hello");
				
				}
			
			}
		
		}
	
	}
	
}


SCENARIO("\\b matches between a word and non-word code point","[anchors]") {

	GIVEN("\\b") {
	
		Regex r("\\b");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto matches=r.Matches(s);
			
			THEN("It does not match") {
			
				REQUIRE(matches.size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing exactly one word") {
		
			String s("hello");
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string once at the beginning and once at the end") {
			
				REQUIRE(matches.size()==2);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get().Size()==0);
				CHECK(matches[1].begin()==s.end());
				CHECK(matches[1].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two words") {
		
			String s("hello world");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string once at the beginning of each word and once at the end of each word") {
			
				REQUIRE(matches.size()==4);
				CHECK(matches[0].begin()==b);
				CHECK(matches[0].Get().Size()==0);
				CHECK((matches[1].begin()-b)==5);
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[2].begin()-b)==6);
				CHECK(matches[2].Get().Size()==0);
				CHECK(matches[3].begin()==s.end());
				CHECK(matches[3].Get().Size()==0);
			
			}
		
		}
	
	}

}


SCENARIO("\\B matches everywhere but between a word and non-word code point","[anchors]") {

	GIVEN("\\B") {
	
		Regex r("\\B");
		
		WHEN("It is matched against the empty string") {
		
			String s;
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string once") {
			
				REQUIRE(matches.size()==1);
				CHECK(matches[0].begin()==s.begin());
				CHECK(matches[0].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing exactly one word") {
		
			String s("hello");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string everywhere but the beginning and the end") {
			
				REQUIRE(matches.size()==4);
				CHECK((matches[0].begin()-b)==1);
				CHECK(matches[0].Get().Size()==0);
				CHECK((matches[1].begin()-b)==2);
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[2].begin()-b)==3);
				CHECK(matches[2].Get().Size()==0);
				CHECK((matches[3].begin()-b)==4);
				CHECK(matches[3].Get().Size()==0);
			
			}
		
		}
		
		WHEN("It is matched against a string containing two words") {
		
			String s("hello world");
			auto b=s.begin();
			auto matches=r.Matches(s);
			
			THEN("It matches the empty string everywhere but the beginning and end of each word") {
			
				REQUIRE(matches.size()==8);
				CHECK((matches[0].begin()-b)==1);
				CHECK(matches[0].Get().Size()==0);
				CHECK((matches[1].begin()-b)==2);
				CHECK(matches[1].Get().Size()==0);
				CHECK((matches[2].begin()-b)==3);
				CHECK(matches[2].Get().Size()==0);
				CHECK((matches[3].begin()-b)==4);
				CHECK(matches[3].Get().Size()==0);
				CHECK((matches[4].begin()-b)==7);
				CHECK(matches[4].Get().Size()==0);
				CHECK((matches[5].begin()-b)==8);
				CHECK(matches[5].Get().Size()==0);
				CHECK((matches[6].begin()-b)==9);
				CHECK(matches[6].Get().Size()==0);
				CHECK((matches[7].begin()-b)==10);
				CHECK(matches[7].Get().Size()==0);
			
			}
		
		}
	
	}

}
