#define CATCH_CONFIG_MAIN


#include <catch.hpp>


#include <unicode/caseconverter.hpp>
#include <unicode/codepoint.hpp>
#include <unicode/comparer.hpp>
#include <unicode/locale.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/string.hpp>
#include <algorithm>
#include <cstring>
#include <vector>


using namespace Unicode;


//	RAII wrapper which changes the locale, and then
//	resets it to default when it goes out of scope
class LocaleChange {


	public:
	
	
		LocaleChange (const Locale & locale) noexcept {
		
			Locale::Set(locale);
		
		}
		
		
		~LocaleChange () noexcept {
		
			Locale::Set(DefaultLocale);
		
		}


};


//	Matches strings exactly
bool IsEqual (const std::vector<CodePoint> & a, const std::vector<CodePoint> & b) noexcept {

	if (a.size()!=b.size()) return false;
	
	return std::equal(
		a.begin(),
		a.end(),
		b.begin()
	);

}


bool IsEqual (const std::vector<CodePoint> & a, const String & b) noexcept {

	return IsEqual(a,b.CodePoints());

}


bool IsEqual (const String & a, const std::vector<CodePoint> & b) noexcept {

	return IsEqual(a.CodePoints(),b);

}


bool IsEqual (const String & a, const String & b) noexcept {

	return IsEqual(a.CodePoints(),b.CodePoints());

}


//
//	CASE CONVERTER
//


SCENARIO("Strings may be converted to lowercase","[caseconverter]") {

	GIVEN("A case converter") {
	
		CaseConverter c;
		
		GIVEN("A lowercase string") {
		
			String s("hello world");
			
			THEN("The result of lowercasing that string is the string itself") {
			
				REQUIRE(String(c.ToLower(s.begin(),s.end())).Equals(s));
			
			}
			
			GIVEN("The same string uppercased") {
			
				String s2("HELLO WORLD");
				
				THEN("The result of lowercasing the latter is the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
			
			GIVEN("The same string with mixed casing") {
			
				String s2("Hello World");
				
				THEN("The result of lowercasing the latter is the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
		
		}
		
		GIVEN("An uppercased string containing GREEK CAPITAL LETTER SIGMA both in word final position and elsewhere") {
		
			String s("ὈΔΥΣΣΕΎΣ");
			
			THEN(
				"Lowercasing that string converts GREEK CAPITAL LETTER SIGMA to GREEK SMALL LETTER FINAL SIGMA "
				"in word final position, GREEK SMALL LETTER SIGMA elsewhere"
			) {
			
				auto cps=c.ToLower(s.begin(),s.end());
				REQUIRE(cps.size()==8);
				CHECK(cps[3]==0x3C3);
				REQUIRE(cps[7]==0x3C2);
			
			}
		
		}
	
	}
	
	GIVEN("A case converter in Lithuanian locale") {
	
		Locale l(DefaultLocale);
		l.Language="lt";
		CaseConverter c(l);
		
		GIVEN("A string containing LATIN SMALL LETTER I, COMBINING DOT ABOVE, COMBINING ACUTE ACCENT") {
		
			String s("i̇́");
			
			GIVEN("A string containing LATIN CAPITAL LETTER I WITH ACUTE") {
			
				String s2("Í");
			
				THEN("Lowercasing the latter results in the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
			
			GIVEN("A string containing LATIN CAPITAL LETTER I, COMBINING ACUTE ACCENT") {
			
				String s2("Í");
				
				THEN("Lowercasing the latter results in the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A case converter in Turkic locale") {
	
		Locale l(DefaultLocale);
		l.Language="tr";
		CaseConverter c(l);
		
		GIVEN("A string containing LATIN SMALL LETTER I") {
		
			String s("i");
		
			GIVEN("A string containing LATIN CAPITAL LETTER I, COMBINING DOT ABOVE") {
			
				String s2("İ");
				
				THEN("Lowercasing the latter results in the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
			
			GIVEN("A string containing LATIN CAPITAL LETTER I WITH DOT ABOVE") {
			
				String s2("İ");
				
				THEN("Lowercasing the latter results in the former") {
				
					REQUIRE(String(c.ToLower(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing LATIN CAPITAL LETTER I") {
		
			String s("I");
			
			GIVEN("A string containing LATIN SMALL LETTER DOTLESS I") {
			
				String s2("ı");
				
				THEN("Lowercasing the former results in the latter") {
				
					REQUIRE(String(c.ToLower(s.begin(),s.end())).Equals(s2));
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be converted to uppercase","[caseconverter]") {

	GIVEN("A case converter") {
	
		CaseConverter c;
		
		GIVEN("An uppercase string") {
		
			String s("HELLO WORLD");
			
			THEN("The result of uppercasing that string is the string itself") {
			
				REQUIRE(String(c.ToUpper(s.begin(),s.end())).Equals(s));
			
			}
			
			GIVEN("The same string lowercased") {
			
				String s2("hello world");
				
				THEN("The result of uppercasing the latter is the former") {
				
					REQUIRE(String(c.ToUpper(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
			
			GIVEN("The same string with mixed casing") {
			
				String s2("Hello world");
				
				THEN("The result of uppercasing the latter is the former") {
				
					REQUIRE(String(c.ToUpper(s2.begin(),s2.end())).Equals(s));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING GREEK YPOGEGRAMMENI, COMBINING ACUTE ACCENT") {
		
			String s("ᾴ");
			
			GIVEN("A string containing GREEK CAPITAL LETTER ALPHA, COMBINING ACUTE ACCENT, GREEK CAPITAL LETTER IOTA") {
			
				String s2("ΆΙ");
				
				THEN("The result of uppercasing the former is the latter") {
				
					REQUIRE(String(c.ToUpper(s.begin(),s.end())).Equals(s2));
				
				}
				
			}
		
		}
		
		GIVEN("A string containing LATIN SMALL LETTER SHARP S") {
		
			String s("ß");
			
			GIVEN("A string containing LATIN CAPITAL LETTER S, LATIN CAPITAL LETTER S") {
			
				String s2("SS");
				
				THEN("The result of uppercasing the former is the latter") {
				
					REQUIRE(String(c.ToUpper(s.begin(),s.end())).Equals(s2));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A case converter in Lithuanian locale") {
	
		Locale l(DefaultLocale);
		l.Language="lt";
		CaseConverter c(l);
		
		GIVEN("A string containing LATIN SMALL LETTER I, COMBINING DOT ABOVE, COMBINING ACUTE ACCENT") {
		
			String s("i̇́");
			
			GIVEN("A string containing LATIN CAPITAL LETTER I, COMBINING ACUTE ACCENT") {
			
				String s2("Í");
				
				THEN("Uppercasing the latter results in the former") {
				
					REQUIRE(String(c.ToUpper(s.begin(),s.end())).Equals(s2));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A case converter in Turkic locale") {
	
		Locale l(DefaultLocale);
		l.Language="tr";
		CaseConverter c(l);
		
		GIVEN("A string containing LATIN SMALL LETTER I") {
		
			String s("i");
			
			GIVEN("A string containing LATIN CAPITAL LETTER I WITH DOT ABOVE") {
			
				String s2("İ");
				
				THEN("Uppercasing the former results in the latter") {
				
					REQUIRE(String(c.ToUpper(s.begin(),s.end())).Equals(s2));
				
				}
			
			}
		
		}
	
	}

}


//
//	COMPARER
//


SCENARIO("Strings may be compared for canonical equivalence","[comparer]") {

	GIVEN("A comparer in the default locale") {
	
		Comparer c;
		
		THEN("It finds empty strings represented by nullptrs to be equivalent") {
		
			REQUIRE(c.Compare(nullptr,nullptr,nullptr,nullptr));
			REQUIRE(c.Compare(nullptr,nullptr,nullptr));
		
		}
		
		GIVEN("An empty string") {
		
			String s;
			
			GIVEN("A C style string") {
			
				const char * str="Hello world";
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),str));
				
				}
			
			}
			
			GIVEN("An empty C style string") {
			
				const char * str="";
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),str));
				
				}
			
			}
		
		}
		
		GIVEN("A string that doesn't change under NFD or NFC") {
		
			const char * str="Hello world";
			
			String s(str);
			
			GIVEN("An identical string") {
			
				String s2(str);
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			GIVEN("A different string") {
			
				String s2("Good-bye world");
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			GIVEN("An identical C style string") {
			
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),str));
				
				}
			
			}
			
			GIVEN("A different C style string") {
			
				const char * str2="Good-bye world";
			
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),str2));
				
				}
			
			}
			
			THEN("It is found not to be equivalent to nullptr") {
			
				REQUIRE(!c.Compare(s.begin(),s.end(),nullptr));
			
			}
			
			GIVEN("An empty C style string") {
			
				const char * str2="";
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),str2));
				
				}
			
			}
		
		}
		
		GIVEN("A string in NFD which changes under NFC") {
		
			String s("naïveté");
			REQUIRE(s.Size()==9);
			
			GIVEN("A canonically equivalent string in NFC") {
			
				String s2("naïveté");
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			THEN("It is found to be equivalent to itself") {
			
				REQUIRE(c.Compare(s.begin(),s.end(),s.begin(),s.end()));
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be compared case insensitively","[comparer]") {

	GIVEN("A comparer in the default locale in case insensitive mode") {
	
		Comparer c(false);
		
		GIVEN("A string with only simple case foldings") {
		
			String s("Hello world");
			
			THEN("It is found to be equivalent with itself") {
			
				REQUIRE(c.Compare(s.begin(),s.end(),s.begin(),s.end()));
			
			}
			
			GIVEN("A string with only simple case foldings, which represents the same string when case is ignored") {
			
				String s2("hello world");
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			GIVEN("A string with only simple case foldings, which represents a different string when case is ignored") {
			
				String s2("good-bye world");
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
		
		}
		
		GIVEN("A string with full case foldings") {
		
			String s("ß");
			
			THEN("It is found to be equivalent with itself") {
			
				REQUIRE(c.Compare(s.begin(),s.end(),s.begin(),s.end()));
			
			}
			
			GIVEN("A string with only simple case foldings, which represents the same string when case is ignored") {
			
				String s2("SS");
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			GIVEN("A string that does not represent the same string when case is ignored") {
			
				String s2("a");
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A comparer in the default locale in case insensitive mode with full case foldings disabled") {
	
		Comparer c(false);
		c.SetFullMappings(false);
		
		GIVEN("A string with full case foldings") {
		
			String s("ß");
			
			THEN("It is found to be equivalent with itself") {
			
				REQUIRE(c.Compare(s.begin(),s.end(),s.begin(),s.end()));
			
			}
			
			GIVEN(
				"A string with only simple case foldings, which represents the same string when case is ignored "
				"under full foldings, but not under simple foldings"
			) {
			
				String s2("SS");
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
			
			GIVEN("A string that does not represent the same string when case is ignored") {
			
				String s2("a");
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2.begin(),s2.end()));
				
				}
			
			}
		
		}
	
	}

}


//
//	LOCALE
//


SCENARIO("The global locale is initialized properly","[locale]") {

	GIVEN("The program has started") {
	
		THEN("The global locale is the default locale") {
		
			REQUIRE(&(Locale::Get())==&DefaultLocale);
		
		}
	
	}

}


SCENARIO("The global locale can be changed","[locale]") {
	

	GIVEN("A non-default locale") {
	
		Locale l(DefaultLocale);
		const char * language="tr";
		l.Language=language;
		
		WHEN("The locale is changed") {
		
			LocaleChange c(l);
			
			THEN("The new locale is the global locale") {
			
				REQUIRE(std::strcmp(language,Locale::Get().Language)==0);
			
			}
		
		}
	
	}
	

}


SCENARIO("Information about code points can be retrieved from the global locale","[locale]") {

	GIVEN("The program has started") {
	
		THEN("Code points for which information exists may be looked up") {
		
			auto cpi=DefaultLocale.GetInfo('A');
			
			REQUIRE(cpi!=nullptr);
			REQUIRE(cpi->Name!=nullptr);
			REQUIRE(cpi->CodePoint=='A');
			REQUIRE(std::strcmp(cpi->Name,"LATIN CAPITAL LETTER A")==0);
		
		}
		
		THEN("Code points for which information does not exist cannot be looked up") {
		
			auto cpi=DefaultLocale.GetInfo(static_cast<CodePoint::Type>(0x10FFFE));
			REQUIRE(cpi==nullptr);
		
		}
	
	}

}


//
//	NORMALIZER
//


SCENARIO("Strings may be placed in Normal Form Canonical Composition","[normalizer]") {

	GIVEN("A normalizer") {
	
		Normalizer n;
		
		GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING ACUTE ACCENT, COMBINING GREEK YPOGEGRAMMENI") {
		
			String s("ᾴ");
			
			THEN("It is not considered to be in Normal Form Canonical Composition") {
			
				REQUIRE(!n.IsNFC(s.begin(),s.end()));
			
			}
			
			GIVEN("A string containing GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI") {
			
				String s2("ᾴ");
				
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s2));
				
				}
			
			}
		
		}
	
	}

}