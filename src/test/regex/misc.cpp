#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("RFC 5322 e-mail regex successfully validates e-mails","[misc][regex][email]") {

	GIVEN("RFC 5322 e-mail regex which allows leading and trailing white space") {
	
		Regex r(
			R"~~(^\s*
			  (?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*
			  |  "(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]
				  |  \\[\x01-\x09\x0b\x0c\x0e-\x7f])*")
			@ (?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?
			  |  \[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}
				   (?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:
					  (?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]
					  |  \\[\x01-\x09\x0b\x0c\x0e-\x7f])+)
				 \])
			\s*$)~~",
			RegexOptions::IgnorePatternWhiteSpace|RegexOptions::IgnoreCase
		);
		
		WHEN("\"rleahy@rleahy.ca\" is tested") {
		
			auto match=r.Match("rleahy@rleahy.ca");
		
			THEN("It is found to be a valid e-mail") {
			
				CHECK(match);
			
			}
		
		}
		
		WHEN("\" rleahy@rleahy.ca \" is tested") {
		
			auto match=r.Match(" rleahy@rleahy.ca ");
		
			THEN("It is found to be a valid e-mail") {
			
				CHECK(match);
			
			}
		
		}
		
		WHEN("\"RLEAHY@RLEAHY.CA\" is tested") {
		
			auto match=r.Match("RLEAHY@RLEAHY.CA");
			
			THEN("It is found to be a valid e-mail") {
			
				CHECK(match);
			
			}
		
		}
		
		WHEN("\"rleahy.ca\" is tested") {
		
			auto match=r.Match("rleahy.ca");
			
			THEN("It is not found to be a valid e-mail") {
			
				CHECK(!match);
			
			}
		
		}
		
		WHEN("\"@rleahy.ca\" is tested") {
		
			auto match=r.Match("@rleahy.ca");
			
			THEN("It is not found to be a valid e-mail") {
			
				CHECK(!match);
			
			}
		
		}
		
		WHEN("\"John..Doe@example.com\" is tested") {
		
			auto match=r.Match("John..Doe@example.com");
			
			THEN("It is not found to be a valid e-mail") {
			
				CHECK(!match);
			
			}
		
		}
		
		WHEN("\"John.Doe@example.com\" is tested") {
		
			auto match=r.Match("John.Doe@example.com");
			
			THEN("It is found to be a valid e-mail") {
			
				CHECK(match);
			
			}
		
		}
		
		WHEN("\".John.Doe@example.com\" is tested") {
		
			auto match=r.Match(".John.Doe@example.com");
			
			THEN("It is not found to be a valid e-mail") {
			
				CHECK(!match);
			
			}
		
		}
		
		WHEN("\"John.Doe.@example.com\" is tested") {
		
			auto match=r.Match("John.Doe.@example.com");
			
			THEN("It is not found to be a valid e-mail") {
			
				CHECK(!match);
			
			}
		
		}
	
	}

}
