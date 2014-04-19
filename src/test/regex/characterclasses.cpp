#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("\\w matches letters, digits, and underscores","[characterclasses][word][regex]") {

	GIVEN("\\w") {
	
		Regex r("\\w");
		
		THEN("It matches letters") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It matches underscores") {
		
			CHECK(r.Match("_"));
		
		}
		
		THEN("It matches digits") {
		
			CHECK(r.Match("1"));
		
		}
		
		THEN("It does not match white space") {
		
			CHECK(!r.Match(" "));
		
		}
		
		THEN("It does not match newlines") {
		
			CHECK(!r.Match("\n"));
		
		}
		
		THEN("It does not match hyphens") {
		
			CHECK(!r.Match("-"));
		
		}
		
		THEN("It does not match symbols") {
		
			CHECK(!r.Match("\\"));
		
		}
	
	}

}


SCENARIO("\\W matches anything but letters, digits, and underscores","[characterclasses][word][regex]") {

	GIVEN("\\W") {
	
		Regex r("\\W");
		
		THEN("It does not match letters") {
		
			CHECK(!r.Match("a"));
		
		}
		
		THEN("It does not match underscores") {
		
			CHECK(!r.Match("_"));
		
		}
		
		THEN("It does not match digits") {
		
			CHECK(!r.Match("1"));
		
		}
		
		THEN("It matches white space") {
		
			CHECK(r.Match(" "));
		
		}
		
		THEN("It matches newlines") {
		
			CHECK(r.Match("\n"));
		
		}
		
		THEN("It matches hyphens") {
		
			CHECK(r.Match("-"));
		
		}
		
		THEN("It matches symbols") {
		
			CHECK(r.Match("\\"));
		
		}
	
	}

}


SCENARIO("\\s matches white space","[characterclasses][whitespace][regex]") {

	GIVEN("\\s") {
	
		Regex r("\\s");
		
		THEN("It matches spaces") {
		
			CHECK(r.Match(" "));
		
		}
		
		THEN("It matches newlines") {
		
			CHECK(r.Match("\n"));
		
		}
		
		THEN("It does not match letters") {
		
			CHECK(!r.Match("a"));
		
		}
		
		THEN("It does not match digits") {
		
			CHECK(!r.Match("1"));
		
		}
		
		THEN("It does not match symbols") {
		
			CHECK(!r.Match("\\"));
		
		}
	
	}

}


SCENARIO("\\S matches anything but white space","[characterclasses][whitespace][regex]") {

	GIVEN("\\S") {
	
		Regex r("\\S");
		
		THEN("It does not match spaces") {
		
			CHECK(!r.Match(" "));
		
		}
		
		THEN("It does not match newlines") {
		
			CHECK(!r.Match("\n"));
		
		}
		
		THEN("It matches letters") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It matches digits") {
		
			CHECK(r.Match("1"));
		
		}
		
		THEN("It matches symbols") {
		
			CHECK(r.Match("\\"));
		
		}
	
	}

}


SCENARIO("\\d matches digits","[characterclasses][digit][regex]") {

	GIVEN("\\d") {
	
		Regex r("\\d");
		
		THEN("It matches digits") {
		
			CHECK(r.Match("1"));
		
		}
		
		THEN("It does not match letters") {
		
			CHECK(!r.Match("a"));
		
		}
		
		THEN("It does not match symbols") {
		
			CHECK(!r.Match("\\"));
		
		}
		
		THEN("It does not match white space") {
		
			CHECK(!r.Match(" "));
		
		}
		
		THEN("It does not match newlines") {
		
			CHECK(!r.Match("\n"));
		
		}
	
	}

}


SCENARIO("\\D matches anything but digits","[characterclasses][digit][regex]") {

	GIVEN("\\D") {
	
		Regex r("\\D");
		
		THEN("It does not match digits") {
		
			CHECK(!r.Match("1"));
		
		}
		
		THEN("It matches letters") {
		
			CHECK(r.Match("a"));
		
		}
		
		THEN("It matches symbols") {
		
			CHECK(r.Match("\\"));
		
		}
		
		THEN("It matches white space") {
		
			CHECK(r.Match(" "));
		
		}
		
		THEN("It matches newlines") {
		
			CHECK(r.Match("\n"));
		
		}
	
	}

}
