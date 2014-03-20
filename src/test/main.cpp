#include <unicode/ascii.hpp>
#include <unicode/binarysearch.hpp>
#include <unicode/caseconverter.hpp>
#include <unicode/codepoint.hpp>
#include <unicode/comparer.hpp>
#include <unicode/converter.hpp>
#include <unicode/iostream.hpp>
#include <unicode/latin1.hpp>
#include <unicode/locale.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/string.hpp>
#include <unicode/utf8.hpp>
#include <unicode/utf16.hpp>
#include <unicode/vector.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace Catch {


	//	Formats Unicode strings for output by Catch
	std::string toString (const Unicode::String & str) {

		std::ostringstream ss;
		ss << "\"" << str << "\"";
		
		return ss.str();

	}
	
	
}


#define CATCH_CONFIG_MAIN


#include <catch.hpp>


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


//	Determines if an iterator is bounded above/below
//	by other iterators
template <typename Iter>
bool InRange (const Iter & begin, const Iter & end, const Iter & iter) noexcept {

	return (iter>=begin) && (iter<end);

}


template <typename T>
std::size_t StringLength (const T * str) noexcept {

	if (str==nullptr) return 0;

	std::size_t retr=0;
	for (;*str!=0;++str,++retr);
	
	return retr;

}


template <typename T>
auto normalize (const T & obj) noexcept -> std::vector<typename std::decay<decltype(*(obj.begin()))>::type> {

	return std::vector<typename std::decay<decltype(*(obj.begin()))>::type>(obj.begin(),obj.end());

}


template <typename T>
std::vector<T> normalize (const T * str) {

	return std::vector<T>(str,str+StringLength(str));

}


template <typename T>
class remove_signed_helper {


	public:
	
	
		typedef typename std::make_unsigned<T>::type type;


};


template <>
class remove_signed_helper<CodePoint> {


	public:
	
	
		typedef CodePoint type;


};


template <typename T>
typename remove_signed_helper<T>::type remove_signed (T i) noexcept {

	union {
		typename std::make_unsigned<T>::type out;
		T in;
	};
	in=i;
	
	return out;

}


CodePoint remove_signed (CodePoint cp) noexcept {

	return cp;

}


template <typename T1, typename T2>
bool IsEqual (const T1 & a, const T2 & b) {

	auto n_a=normalize(a);
	auto n_b=normalize(b);
	
	auto begin_a=n_a.begin();
	auto end_a=n_a.end();
	auto begin_b=n_b.begin();
	auto end_b=n_b.end();
	
	if ((end_a-begin_a)!=(end_b-begin_b)) return false;
	
	typedef typename std::decay<decltype(*begin_a)>::type a_type;
	typedef typename std::decay<decltype(*begin_b)>::type b_type;
	
	return std::equal(
		begin_a,
		end_a,
		begin_b,
		[] (a_type a, b_type b) noexcept {
		
			return remove_signed(a)==remove_signed(b);
		
		}
	);

}


template <typename T1, typename T2>
bool IsByteWiseEqual (const T1 & a, const T2 & b) {

	auto n_a=normalize(a);
	auto n_b=normalize(b);
	
	auto begin_a=reinterpret_cast<const unsigned char *>(Begin(n_a));
	auto end_a=reinterpret_cast<const unsigned char *>(End(n_a));
	auto begin_b=reinterpret_cast<const unsigned char *>(Begin(n_b));
	auto end_b=reinterpret_cast<const unsigned char *>(End(n_b));
	
	if ((end_a-begin_a)!=(end_b-begin_b)) return false;
	
	return std::equal(
		begin_a,
		end_a,
		begin_b
	);

}


//	Gets a std::string containing some type
template <typename T>
std::string Get (const T & t) {

	std::ostringstream ss;
	ss << t;
	
	return ss.str();

}


//
//	ASCII
//


SCENARIO("ASCII characters are the correct width","[ascii]") {

	GIVEN("ASCII::CodeUnit") {
	
		THEN("It is one byte wide") {
		
			REQUIRE(sizeof(ASCII::CodeUnit)==1);
		
		}
	
	}

}


SCENARIO("ASCII characters are unsigned","[ascii]") {

	GIVEN("ASCII::CodeUnit") {
	
		THEN("It is unsigned") {
		
			REQUIRE(std::is_unsigned<ASCII::CodeUnit>::value);
		
		}
	
	}

}


SCENARIO("The ASCII BOM is empty") {

	GIVEN("An ASCII encoder/decoder") {
	
		ASCII encoder;
		
		THEN("Its BOM is empty") {
		
			std::vector<unsigned char> bom;
			encoder.BOM().Get(bom);
			REQUIRE(bom.size()==0);
		
		}
	
	}

}


SCENARIO("Information about the representation of code points in ASCII is determined properly","[ascii]") {

	GIVEN("An ASCII encoder/decoder") {
	
		ASCII encoder;
		
		GIVEN("A code point representable in ASCII") {
		
			CodePoint cp='a';
			
			THEN("It is identified as being representable in ASCII") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires one ASCII character to represent") {
			
				REQUIRE(encoder.Count(cp)==1);
			
			}
		
		}
		
		GIVEN("A code point unrepresentable in ASCII") {
		
			CodePoint cp=127U+1;
			
			THEN("It is identified as being unrepresentable in ASCII") {
			
				CHECK(!encoder.CanRepresent(cp));
				REQUIRE(encoder.Count(cp)==0);
			
			}
		
		}
	
	}

}


SCENARIO("ASCII strings may be decoded","[ascii]") {

	GIVEN("An ASCII encoder/decoder") {
	
		ASCII encoder;
		
		THEN("Decoding an empty buffer represented as two null iterators results in the empty string") {
		
			REQUIRE(encoder.Decode(nullptr,nullptr).size()==0);
		
		}
		
		GIVEN("An empty buffer of bytes") {
		
			std::vector<unsigned char> buffer;
			
			THEN("Attempting to decode the buffer results in the empty string") {
			
				REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
			
			}
		
		}
		
		GIVEN("An ASCII string") {
		
			const char * begin="Hello world";
			auto end=begin+StringLength(begin);
			
			THEN("Decoding it recovers the ASCII string") {
			
				REQUIRE(IsEqual(encoder.Decode(begin,end),begin));
			
			}
			
		}
		
		GIVEN("A buffer containing a Latin-1 character") {
		
			std::vector<unsigned char> buffer={0xFF};
			
			THEN("Attempting to decode it raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer recovers the character") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0xFF);
				
				}
			
			}
			
			GIVEN("Strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
			
				THEN("Attempting to decode the buffer results in the replacement") {
			
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
					
				}
			
			}
			
			GIVEN("Strict errors result in no action") {
			
				encoder.Strict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
			
		}
	
	}

}


SCENARIO("Strings may be encoded to ASCII","[ascii]") {

	GIVEN("An ASCII encoder/decoder") {
	
		ASCII encoder;
		
		GIVEN("A string containing only ASCII") {
		
			String s("Hello world");
			
			THEN("It may be encoded") {
			
				REQUIRE(IsEqual(encoder.Encode(s),s));
			
			}
		
		}
		
		GIVEN("An ASCII encoder which will output the BOM") {
		
			ASCII encoder2;
			encoder2.OutputBOM=true;
			
			GIVEN("A string representable in ASCII") {
			
				String s("Hello world");
				
				THEN("Both encoders generate identical representations") {
				
					REQUIRE(IsEqual(encoder.Encode(s),encoder2.Encode(s)));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing Unicode") {
		
			String s(u8"м");
			
			THEN("Encoding the string results in an exception") {
			
				REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
			
			}
			
			GIVEN("Lossy errors are being ignored") {
			
				encoder.Lossy.Ignore();
				
				THEN("Encoding the string results in an empty buffer") {
				
					REQUIRE(encoder.Encode(s).size()==0);
				
				}
			
			}
			
			GIVEN("Lossy errors result in no action") {
			
				encoder.Lossy.Nothing();
				
				THEN("Encoding the string results in an empty buffer") {
				
					REQUIRE(encoder.Encode(s).size()==0);
				
				}
			
			}
			
			GIVEN("Lossy errors result in a replacement representable in ASCII") {
			
				CodePoint replacement='?';
				encoder.Lossy.Replace(replacement);
				
				THEN("Encoding the string results in the replacement") {
				
					auto encoded=encoder.Encode(s);
					REQUIRE(encoded.size()==1);
					REQUIRE(encoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Lossy errors result in a replacement not representable in ASCII") {
			
				//	REPLACEMENT CHARACTER (U+FFFD)
				CodePoint replacement=0xFFFDU;
				encoder.Lossy.Replace(replacement);
				
				THEN("Encoding the string results in an exception") {
				
					REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
				
				}
			
			}
		
		}
	
	}

}


//
//	BINARY SEARCH
//


SCENARIO("Elements in a sorted collection may be efficiently located","[binarysearch]") {

	GIVEN("A sorted collection") {
	
		int arr []={
			1,
			17,
			93,
			672,
			3486,
			90042
		};
		auto begin=std::begin(arr);
		auto end=std::end(arr);
		
		THEN("An element at the beginning of the collection is found successfully") {
		
			auto iter=BinarySearch(begin,end,1);
			REQUIRE(iter==begin);
			REQUIRE(*iter==1);
		
		}
		
		THEN("An element at the end of the collection is found successfully") {
		
			auto iter=BinarySearch(begin,end,90042);
			REQUIRE(iter==(end-1));
			REQUIRE(*iter==90042);
		
		}
		
		THEN("An element at neither the beginning or the end of the collection is found successfully") {
		
			auto iter=BinarySearch(begin,end,672);
			REQUIRE(InRange(begin,end,iter));
			REQUIRE(*iter==672);
		
		}
		
		THEN("An element which would be before the beginning is not found") {
		
			REQUIRE(BinarySearch(begin,end,0)==end);
		
		}
		
		THEN("An element which would be past the end is not found") {
		
			REQUIRE(BinarySearch(begin,end,std::numeric_limits<int>::max())==end);
		
		}
		
		THEN("An element which would be neither before the beginning or past the end is not found") {
		
			REQUIRE(BinarySearch(begin,end,400)==end);
		
		}
	
	}

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
//	CODE POINT
//


SCENARIO("Code points may be checked for validity","[codepoint]") {

	GIVEN("A valid code point") {
	
		CodePoint cp='a';
		
		THEN("It is found to be valid") {
		
			REQUIRE(cp.IsValid());
		
		}
	
	}
	
	GIVEN("A code point beyond U+10FFFF") {
	
		CodePoint cp=0x10FFFFU+1;
		
		THEN("It is found to be invalid") {
		
			REQUIRE(!cp.IsValid());
		
		}
	
	}
	
	GIVEN("A UTF-16 surrogate") {
	
		CodePoint cp=0xD800U;
		
		THEN("It is found to be invalid") {
		
			REQUIRE(!cp.IsValid());
		
		}
	
	}
	
	GIVEN("The reversed byte order mark (i.e. U+FFFE)") {
	
		CodePoint cp=0xFFFEU;
		
		THEN("It is found to be invalid") {
		
			REQUIRE(!cp.IsValid());
		
		}
	
	}
	
	GIVEN("A non-character") {
	
		CodePoint cp=0x1FFFFU;
		
		THEN("It is found to be invalid") {
		
			REQUIRE(!cp.IsValid());
		
		}
	
	}
	
	GIVEN("One of the code points in the range U+FDD0..U+FDEF") {
	
		CodePoint cp=0xFDD0U;
		
		THEN("It is found to be invalid") {
		
			REQUIRE(!cp.IsValid());
		
		}
	
	}
	
	GIVEN("The code points immediately before and after U+FDD0..U+FDEF") {
	
		CodePoint before=0xFDD0U-1;
		CodePoint after=0xFDEFU+1;
		
		THEN("They are found to be valid") {
		
			CHECK(before.IsValid());
			REQUIRE(after.IsValid());
		
		}
	
	}

}


SCENARIO("Code points may be tested to see if they are white space or not","[codepoint]") {

	GIVEN("A code point that is in ASCII and is not white space") {
	
		CodePoint cp='a';
		
		THEN("It is not identified as white space") {
		
			REQUIRE(!cp.IsWhiteSpace());
		
		}
	
	}
	
	GIVEN("A code point that is not ASCII and is not white space") {
	
		//	GREEK CAPITAL LETTER SIGMA (U+03A3)
		CodePoint cp=0x03A3U;
		
		THEN("It is not identified as white space") {
		
			REQUIRE(!cp.IsWhiteSpace());
		
		}
	
	}
	
	GIVEN("A code point that is in ASCII and is white space") {
	
		CodePoint cp=' ';
		
		THEN("It is identified as white space") {
		
			REQUIRE(cp.IsWhiteSpace());
		
		}
	
	}
	
	GIVEN("A code point that is not ASCII and is white space") {
	
		//	PARAGRAPH SEPARATOR (U+2029)
		CodePoint cp=0x2029U;
		
		THEN("It is identified as white space") {
		
			REQUIRE(cp.IsWhiteSpace());
		
		}
	
	}
	
	GIVEN("A code point for which information is not available") {
	
		//	Outside Unicode, therefore by definition no information
		//	available
		CodePoint cp=CodePoint::Max+1;
		
		THEN("It is not identified as white space") {
		
			REQUIRE(!cp.IsWhiteSpace());
		
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
		
			CHECK(c.Compare(nullptr,nullptr,nullptr,nullptr));
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
			
			GIVEN("A C style string containing the same string") {
			
				const char * s2="Hello world";
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2));
				
				}
			
			}
			
			GIVEN("A C style string containing the same string when case is ignored") {
			
				const char * s2="HELLO WORLD";
				
				THEN("They are found to be equivalent") {
				
					REQUIRE(c.Compare(s.begin(),s.end(),s2));
				
				}
			
			}
			
			GIVEN("A C style string containing a different string when case is ignored") {
			
				const char * s2="good-bye world";
				
				THEN("They are not found to be equivalent") {
				
					REQUIRE(!c.Compare(s.begin(),s.end(),s2));
				
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
//	INTEGER CONVERSION
//


SCENARIO("Integers may be converted to strings","[converter]") {

	GIVEN("An integer converter") {
	
		Converter<int> c;
		
		GIVEN("The largest integer") {
		
			auto i=std::numeric_limits<int>::max();
			
			GIVEN("A string containing this integer") {
			
				auto s=Get(i);
				
				THEN("Converting the former to a Unicode string results in the latter") {
				
					REQUIRE(IsEqual(c(i),s));
				
				}
			
			}
		
		}
		
		GIVEN("The smallest integer") {
		
			auto i=std::numeric_limits<int>::min();
			
			GIVEN("A string containing this integer") {
			
				auto s=Get(i);
				
				THEN("Converting the former to a Unicode string results in the latter") {
				
					REQUIRE(IsEqual(c(i),s));
				
				}
			
			}
		
		}
		
		GIVEN("Zero") {
		
			int i=0;
			
			GIVEN("A string containing zero") {
			
				std::string s="0";
				
				THEN("Converting the former to a Unicode string results in the latter") {
				
					REQUIRE(IsEqual(c(i),s));
				
				}
			
			}
		
		}
		
		int dummy=0;
		
		THEN("Attempting to convert using a base higher than 16 results in an exception") {
		
			REQUIRE_THROWS_AS(c(dummy,17),ConversionError);
		
		}
		
		THEN("Attempting to convert using a base of 0 results in an exception") {
		
			REQUIRE_THROWS_AS(c(dummy,0),ConversionError);
		
		}
	
	}

}


SCENARIO("Strings may be converted to integers","[converter]") {

	GIVEN("An integer converter") {
	
		Converter<int> c;
		
		THEN("The empty string represented as two null pointers is not found to represent an integer") {
		
			REQUIRE_THROWS_AS(c(nullptr,nullptr),ConversionError);
		
		}
		
		GIVEN("The empty string") {
		
			String s;
			
			THEN("It is not found to represent an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end()),ConversionError);
			
			}
		
		}
		
		GIVEN("A string containing the largest integer") {
		
			String s(std::numeric_limits<int>::max());
			
			THEN("It is found to contain the largest integer") {
			
				REQUIRE(c(s.begin(),s.end())==std::numeric_limits<int>::max());
			
			}
		
		}
		
		GIVEN("A string containing the smallest integer") {
		
			String s(std::numeric_limits<int>::min());
			
			THEN("It is found to contain the smallest integer") {
			
				REQUIRE(c(s.begin(),s.end())==std::numeric_limits<int>::min());
			
			}
		
		}
		
		GIVEN("A string containing an integer interspersed with white space") {
		
			String s("  - 1 2    3 ");
			
			GIVEN("The integer that string represents") {
			
				int i=-123;
				
				THEN("The former converts to the latter") {
				
					REQUIRE(c(s.begin(),s.end())==i);
				
				}
			
			}
		
		}
		
		GIVEN("A string which is partially numeric") {
		
			String s("hello123");
			
			THEN("It is not found to contain an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end()),ConversionError);
			
			}
		
		}
		
		GIVEN("A string which is not numeric") {
		
			String s("hello world");
			
			THEN("It is not found to contain an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end()),ConversionError);
			
			}
		
		}
		
		GIVEN("A string which is numeric, but which has digits disallowed by the specified base") {
		
			String s("19");
			
			THEN("It is not found to contain an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end(),9),ConversionError);
			
			}
		
		}
		
		GIVEN("A string containing an integer larger than the largest integer") {
		
			String s(std::numeric_limits<int>::max());
			s << "1";
			
			THEN("It cannot be converted to an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end()),std::overflow_error);
			
			}
		
		}
		
		GIVEN("A string containing an integer smaller than the smallest integer") {
		
			String s(std::numeric_limits<int>::min());
			s << "1";
			
			THEN("It cannot be converted to an integer") {
			
				REQUIRE_THROWS_AS(c(s.begin(),s.end()),std::underflow_error);
			
			}
		
		}
		
		THEN("Attempting to convert using a base higher than 16 results in an exception") {
		
			REQUIRE_THROWS_AS(c(nullptr,nullptr,17),ConversionError);
		
		}
		
		THEN("Attempting to convert using a base of 0 results in an exception") {
		
			REQUIRE_THROWS_AS(c(nullptr,nullptr,0),ConversionError);
		
		}
	
	}

}


//
//	LATIN-1
//


SCENARIO("Latin-1 characters are the correct width","[latin1]") {

	GIVEN("Latin1::CodeUnit") {
	
		THEN("It is one byte wide") {
		
			REQUIRE(sizeof(Latin1::CodeUnit)==1);
		
		}
	
	}

}


SCENARIO("Latin-1 characters are unsigned","[latin1]") {

	GIVEN("Latin1::CodeUnit") {
	
		THEN("It is unsigned") {
		
			REQUIRE(std::is_unsigned<Latin1::CodeUnit>::value);
		
		}
	
	}

}


SCENARIO("The Latin-1 BOM is empty","[latin1]") {

	GIVEN("A Latin-1 encoder/decoder") {
	
		Latin1 encoder;
		
		THEN("Its BOM is empty") {
		
			std::vector<unsigned char> bom;
			encoder.BOM().Get(bom);
			REQUIRE(bom.size()==0);
		
		}
	
	}

}


SCENARIO("Information about the representation of code points in Latin-1 is determined properly","[latin1]") {

	GIVEN("A Latin-1 encoder/decoder") {
	
		Latin1 encoder;
		
		GIVEN("A code point representable in ASCII") {
		
			CodePoint cp='a';
			
			THEN("It is identified as being representable in Latin-1") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires one Latin-1 character to represent") {
			
				REQUIRE(encoder.Count(cp)==1);
			
			}
		
		}
		
		GIVEN("A code point representable in Latin-1 but not ASCII") {
		
			CodePoint cp=128U;
			
			THEN("It is identified as being representable in Latin-1") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires one Latin-1 character to represent") {
			
				REQUIRE(encoder.Count(cp)==1);
			
			}
		
		}
		
		GIVEN("A code point unrepresentable in Latin-1") {
		
			CodePoint cp=256U;
			
			THEN("It is identified as being unrepresentable in Latin-1") {
			
				CHECK(!encoder.CanRepresent(cp));
				REQUIRE(encoder.Count(cp)==0);
			
			}
		
		}
	
	}

}


SCENARIO("Latin-1 strings may be decoded","[latin1]") {

	GIVEN("A Latin-1 encoder/decoder") {
	
		Latin1 encoder;
		
		THEN("Decoding an empty buffer represented as two null iterators results in the empty string") {
		
			REQUIRE(encoder.Decode(nullptr,nullptr).size()==0);
		
		}
		
		GIVEN("An empty buffer of bytes") {
		
			std::vector<unsigned char> buffer;
			
			THEN("Attempting to decode the buffer results in the empty string") {
			
				REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
			
			}
		
		}
		
		GIVEN("An ASCII string") {
		
			const char * begin="Hello world";
			auto end=begin+StringLength(begin);
			
			THEN("Decoding it recovers the ASCII string") {
			
				REQUIRE(IsEqual(encoder.Decode(begin,end),begin));
			
			}
		
		}
		
		GIVEN("A Latin-1 string") {
		
			std::vector<unsigned char> str={0xFF};
			
			THEN("Decoding it recovers the Latin-1 string") {
			
				REQUIRE(IsEqual(encoder.Decode(Begin(str),End(str)),str));
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be encoded to Latin-1","[latin1]") {

	GIVEN("A Latin-1 encoder/decoder") {
	
		Latin1 encoder;
		
		GIVEN("A string containing only ASCII") {
		
			String s("Hello world");
			
			THEN("It may be encoded") {
			
				REQUIRE(IsEqual(encoder.Encode(s),s));
			
			}
		
		}
		
		GIVEN("A string containing ASCII and Latin-1 characters") {
		
			String s(u8"façade");
			
			THEN("It may be encoded") {
			
				REQUIRE(IsEqual(encoder.Encode(s),s));
			
			}
		
		}
		
		GIVEN("A Latin-1 encoder/decoder which will output the BOM") {
		
			Latin1 encoder2;
			encoder2.OutputBOM=true;
			
			GIVEN("A string representable in Latin-1") {
			
				String s("Hello world");
				
				THEN("Both encoders generate identical representations") {
			
					REQUIRE(IsEqual(encoder.Encode(s),encoder2.Encode(s)));
			
				}
				
			}
		
		}
		
		GIVEN("A string containing Unicode") {
		
			String s(u8"м");
			
			THEN("Encoding the string results in an exception") {
			
				REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
			
			}
			
			GIVEN("Lossy errors are being ignored") {
			
				encoder.Lossy.Ignore();
				
				THEN("Encoding the string results in an empty buffer") {
				
					REQUIRE(encoder.Encode(s).size()==0);
				
				}
			
			}
			
			GIVEN("Lossy errors result in no action") {
			
				encoder.Lossy.Nothing();
				
				THEN("Encoding the string results in an empty buffer") {
				
					REQUIRE(encoder.Encode(s).size()==0);
				
				}
			
			}
			
			GIVEN("Lossy errors result in a replacement representable in Latin-1") {
			
				CodePoint replacement='?';
				encoder.Lossy.Replace(replacement);
				
				THEN("Encoding the string results in the replacement") {
				
					auto encoded=encoder.Encode(s);
					REQUIRE(encoded.size()==1);
					REQUIRE(encoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Lossy errors result in a replacement not representable in Latin-1") {
			
				//	REPLACEMENT CHARACTER (U+FFFD)
				CodePoint replacement=0xFFFDU;
				encoder.Lossy.Replace(replacement);
				
				THEN("Encoding the string results in an exception") {
				
					REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
				
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
		
		GIVEN("A string containing LATIN SMALL LETTER A") {
		
			String s("a");
			
			THEN("It is considered to be in Normal Form Canonical Composition") {
			
				REQUIRE(n.IsNFC(s.begin(),s.end()));
			
			}
			
			THEN("It transforms to itself under Normal Form Canonical Composition") {
			
				REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s));
			
			}
		
		}
		
		GIVEN("A string containing LATIN SMALL LETTER A, COMBINING ACUTE ACCENT") {
		
			String s("á");
			
			THEN("It is not considered to be in Normal Form Canonical Composition") {
			
				REQUIRE(!n.IsNFC(s.begin(),s.end()));
			
			}
			
			GIVEN("A string containing LATIN SMALL LETTER A WITH ACUTE") {
			
				String s2("á");
				
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s2));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing LATIN SMALL LETTER A, LATIN SMALL LETTER I, COMBINING ACUTE ACCENT") {
		
			String s("aí");
			
			GIVEN("A string containing LATIN SMALL LETTER A, LATIN SMALL LETTER I WITH ACUTE") {
			
				String s2("aí");
			
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s2));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing LATIN SMALL LETTER I, COMBINING RING ABOVE, COMBINING ACUTE ACCENT") {
		
			String s("i̊́");
			
			THEN(
				"COMBINING ACUTE ACCENT is blocked from LATIN SMALL LETTER I and the string "
				"does not change under Normal Form Canonical Composition"
			) {
			
				REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s));
			
			}
		
		}
		
		GIVEN("A string containing LATIN SMALL LETTER C, COMBINING GREEK YPOGEGRAMMENI, COMBINING CEDILLA, COMBINING DIAERESIS") {
		
			String s("ç̈ͅ");
			
			GIVEN("A string containing LATIN SMALL LETTER C WITH CEDILLA, COMBINING DIAERESIS, COMBINING GREEK YPOGEGRAMMENI") {
			
				String s2("ç̈ͅ");
				
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s.begin(),s.end()),s2));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI") {
		
			String s("ᾴ");
			
			THEN("It is considered to be in Normal Form Canonical Composition") {
			
				REQUIRE(n.IsNFC(s.begin(),s.end()));
			
			}
		
			GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING ACUTE ACCENT, COMBINING GREEK YPOGEGRAMMENI") {
			
				String s2("ᾴ");
				
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s2.begin(),s2.end()),s));
				
				}
				
			}
			
			GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING GREEK YPOGEGRAMMENI, COMBINING ACUTE ACCENT") {
			
				String s2("ᾴ");
				
				THEN("They are congruent in Normal Form Canonical Composition") {
				
					REQUIRE(IsEqual(n.ToNFC(s2.begin(),s2.end()),s));
				
				}
			
			}
		
		}
		
		GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING ACUTE ACCENT, COMBINING GREEK YPOGEGRAMMENI") {
		
			String s("ᾴ");
			
			THEN("It is not considered to be in Normal Form Canonical Composition") {
		
				REQUIRE(!n.IsNFC(s.begin(),s.end()));
		
			}
		
		}
		
		GIVEN("A string containing GREEK SMALL LETTER ALPHA, COMBINING GREEK YPOGEGRAMMENI, COMBINING ACUTE ACCENT") {
		
			String s("ᾴ");
			
			THEN("It is not considered to be in Normal Form Canonical Composition") {
		
				REQUIRE(!n.IsNFC(s.begin(),s.end()));
		
			}
			
		}
	
	}

}


//
//	OUTPUT STREAMS
//


SCENARIO("Unicode strings may be output to C++ streams","[iostream]") {

	GIVEN("An 8-bit character output stream") {
	
		std::basic_ostringstream<char> ss;
		
		GIVEN("A UTF-8 string literal") {
		
			auto literal=u8"Привет мир";
			
			GIVEN("A string constructed from that literal") {
			
				String s(literal);
				
				THEN("Outputting that string to the stream results in the literal") {
				
					ss << s;
					REQUIRE(IsByteWiseEqual(ss.str(),literal));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A 16-bit character output stream") {
	
		std::basic_ostringstream<char16_t> ss;
		
		GIVEN("A UTF-16 string literal") {
		
			auto literal=u"Привет мир";
			
			GIVEN("A string constructed from that literal") {
			
				String s(literal);
				
				THEN("Outputting that string to the stream results in the literal") {
				
					ss << s;
					REQUIRE(IsByteWiseEqual(ss.str(),literal));
				
				}
			
			}
		
		}
	
	}
	
	
	GIVEN("A 32-bit character output stream") {
	
		std::basic_ostringstream<char32_t> ss;
		
		GIVEN("A UTF-32 string literal") {
		
			auto literal=U"Привет мир";
			
			GIVEN("A string constructed from that literal") {
			
				String s(literal);
				
				THEN("Outputting that string to the stream results in the literal") {
				
					ss << s;
					REQUIRE(IsByteWiseEqual(ss.str(),literal));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A 64-bit character output stream") {
	
		std::basic_ostringstream<std::uint64_t> ss;
		
		GIVEN("A string") {
		
			String s("Hello world");
			
			THEN("Outputting that string to the stream results in an exception") {
			
				REQUIRE_THROWS_AS(ss << s,std::logic_error);
			
			}
		
		}
	
	}

}


//
//	STRING
//


SCENARIO("Strings may be constructed","[string]") {

	GIVEN("A default constructed string") {
	
		String s;
		
		THEN("It is the empty string") {
		
			REQUIRE(s.Size()==0);
		
		}
		
		THEN("It is in the default locale") {
		
			REQUIRE(&(s.GetLocale())==&DefaultLocale);
		
		}
	
	}
	
	GIVEN("A custom locale") {
	
		Locale l;
		
		GIVEN("A string constructed with that locale") {
		
			String s(l);
			
			THEN("It is the empty string") {
			
				REQUIRE(s.Size()==0);
			
			}
			
			THEN("The string's locale is the custom locale") {
			
				REQUIRE(&(s.GetLocale())==&l);
			
			}
		
		}
	
	}
	
	GIVEN("An empty vector of code points") {
	
		std::vector<CodePoint> cps;
		
		GIVEN("A string constructed from that vector") {
		
			String s(std::move(cps));
			
			THEN("It is the empty string") {
			
				REQUIRE(s.Size()==0);
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
		
		GIVEN("A custom locale") {
		
			Locale l;
			
			GIVEN("A string constructed from that vector and locale") {
			
				String s(std::move(cps),l);
				
				THEN("It is the empty string") {
				
					REQUIRE(s.Size()==0);
				
				}
				
				THEN("The string's locale is the custom locale") {
				
					REQUIRE(&(s.GetLocale())==&l);
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A vector of code points") {
	
		std::vector<CodePoint> cps={'h','e','l','l','o'};
		
		GIVEN("A string constructed from that vector") {
		
			String s(cps);
			
			THEN("They are identical") {
			
				REQUIRE(IsEqual(cps,s));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
		
		GIVEN("A custom locale") {
		
			Locale l;
			
			GIVEN("A string constructed from that vector and locale") {
			
				String s(cps,l);
				
				THEN("The string and vector are identical") {
				
					REQUIRE(IsEqual(cps,s));
				
				}
				
				THEN("The string's locale is the custom locale") {
				
					REQUIRE(&(s.GetLocale())==&l);
				
				}
			
			}
		
		}
	
	}

	GIVEN("A C string containing only ASCII characters") {
	
		auto c_str="hello world";
		
		GIVEN("A string constructed from that string") {
		
			String s(c_str);
			
			THEN("They are identical") {
			
				REQUIRE(IsEqual(s,c_str));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
	
	}
	
	GIVEN("A wide C string containing only ASCII characters") {
	
		auto w_c_str=L"hello world";
		
		GIVEN("A string constructed from that string") {
		
			String s(w_c_str);
			
			THEN("They are identical") {
			
				REQUIRE(IsEqual(s,w_c_str));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
	
	}
	
	GIVEN("A UTF-8 string literal") {
	
		auto u8_str=u8"Привет мир";
		
		GIVEN("A string constructed from that string") {
		
			String s(u8_str);
			
			THEN("The UTF-8 encoding of that string is identical to the literal") {
			
				REQUIRE(IsByteWiseEqual(UTF8{}.Encode(s.begin(),s.end()),u8_str));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
	
	}
	
	GIVEN("A UTF-16 string literal") {
	
		auto u16_str=u"\U0001D11E";
		
		GIVEN("A string constructed from that string") {
		
			String s(u16_str);
			
			THEN("The UTF-16 encoding of that string is identical to the literal") {
			
				auto order=EndianEncoding::Detect();
				UTF16 encoder(order,order);
				encoder.OutputBOM=false;
				REQUIRE(IsByteWiseEqual(encoder.Encode(s.begin(),s.end()),u16_str));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
	
	}
	
	GIVEN("A UTF-32 string literal") {
	
		auto u32_str=U"hello world";
		
		GIVEN("A string constructed from that string") {
		
			String s(u32_str);
			
			THEN("They are identical") {
			
				REQUIRE(IsEqual(s,u32_str));
			
			}
			
			THEN("It is in the default locale") {
			
				REQUIRE(&(s.GetLocale())==&DefaultLocale);
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be copied","[string]") {

	GIVEN("A string") {
	
		String s("hello world");
		
		GIVEN("A copy of that string") {
		
			String s2(s);
			
			THEN("They are identical") {
			
				CHECK(&(s.GetLocale())==&(s2.GetLocale()));
				REQUIRE(IsEqual(s,s2));
			
			}
			
			THEN("When the string is copy assigned to itself, it is still identical to the copy (i.e. it is unchanged)") {
			
				s=s;
				CHECK(&(s.GetLocale())==&(s2.GetLocale()));
				REQUIRE(IsEqual(s,s2));
			
			}
		
		}
		
		GIVEN("A custom locale set on that string") {
		
			Locale l;
			s.SetLocale(l);
			
			GIVEN("A copy of that string") {
			
				String s2(s);
				
				THEN("They are identical") {
				
					CHECK(&(s.GetLocale())==&(s2.GetLocale()));
					REQUIRE(IsEqual(s,s2));
				
				}
			
			}
			
			GIVEN("Another string") {
			
				String s2("good-bye world");
				
				THEN("When the former is copy assigned to the latter, they become identical") {
				
					s2=s;
					
					CHECK(&(s.GetLocale())==&(s2.GetLocale()));
					REQUIRE(IsEqual(s,s2));
				
				}
			
			}
		
		}
		
		GIVEN("Another string") {
		
			String s2("good-bye world");
			
			THEN("When the former is copy assigned to the latter, they become identical") {
			
				s2=s;
				
				CHECK(&(s.GetLocale())==&(s2.GetLocale()));
				REQUIRE(IsEqual(s,s2));
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be moved","[string]") {

	GIVEN("A string") {
	
		String s("hello world");
		
		GIVEN("A copy of that string") {
		
			String s2(s);
			
			GIVEN("A string that is the result of moving the copy") {
			
				String s3(std::move(s2));
				
				THEN("The copy is empty") {
				
					REQUIRE(s2.Size()==0);
				
				}
				
				THEN("The moved string is identical to the original") {
				
					CHECK(&(s.GetLocale())==&(s3.GetLocale()));
					REQUIRE(IsEqual(s,s3));
				
				}
			
			}
			
			GIVEN("Another string") {
			
				String s3("good-bye world");
				
				GIVEN("The copy is move assigned to the last string") {
				
					s3=std::move(s2);
					
					THEN("The copy is empty") {
					
						REQUIRE(s2.Size()==0);
					
					}
					
					THEN("The moved string is identical to the original") {
					
						CHECK(&(s.GetLocale())==&(s3.GetLocale()));
						REQUIRE(IsEqual(s,s3));
					
					}
				
				}
			
			}
		
		}
		
		GIVEN("A custom locale set on that string") {
		
			Locale l;
			s.SetLocale(l);
			
			GIVEN("A copy of that string") {
			
				String s2(s);
				
				GIVEN("A string that is the result of moving the copy") {
				
					String s3(std::move(s2));
					
					THEN("The copy is empty") {
					
						REQUIRE(s2.Size()==0);
					
					}
					
					THEN("The moved string is identical to the original") {
					
						CHECK(&(s.GetLocale())==&(s3.GetLocale()));
						REQUIRE(IsEqual(s,s3));
					
					}
				
				}
				
				GIVEN("Another string") {
				
					String s3("good-bye world");
					
					GIVEN("The copy is move assigned to the last string") {
					
						s3=std::move(s2);
						
						THEN("The copy is empty") {
						
							REQUIRE(s2.Size()==0);
						
						}
						
						THEN("The moved string is identical to the original") {
						
							CHECK(&(s.GetLocale())==&(s3.GetLocale()));
							REQUIRE(IsEqual(s,s3));
						
						}
					
					}
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be converted to C style strings","[string]") {

	GIVEN("A UTF-8 literal") {
	
		auto literal=u8"Привет мир";
		
		GIVEN("A string constructed from that literal") {
		
			String s(literal);
			
			GIVEN("That string converted to a C-style string") {
			
				auto c_str=s.ToCString();
				
				THEN("It is of the same size as the literal") {
				
					REQUIRE(StringLength(literal)==c_str.Size());
				
				}
				
				THEN("It is identical to the literal") {
				
					REQUIRE(IsByteWiseEqual(literal,c_str));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A wide string literal") {
	
		auto literal=L"Hello world";
		
		GIVEN("A string constructed from that literal") {
		
			String s(literal);
			
			GIVEN("That string converted to a C-style shtring") {
			
				auto c_str=s.ToCString<wchar_t>();
				
				THEN("It is of the same size as the literal") {
				
					REQUIRE(StringLength(literal)==c_str.Size());
				
				}
				
				THEN("It is identical to the literal") {
				
					REQUIRE(IsByteWiseEqual(literal,c_str));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A UTF-16 literal") {
	
		auto literal=u"Привет мир";
		
		GIVEN("A string constructed from that literal") {
		
			String s(literal);
			
			GIVEN("That string converted to a C-style shtring") {
			
				auto c_str=s.ToCString<char16_t>();
				
				THEN("It is of the same size as the literal") {
				
					REQUIRE(StringLength(literal)==c_str.Size());
				
				}
				
				THEN("It is identical to the literal") {
				
					REQUIRE(IsByteWiseEqual(literal,c_str));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A UTF-32 literal") {
	
		auto literal=U"Привет мир";
		
		GIVEN("A string constructed from that literal") {
		
			String s(literal);
			
			GIVEN("That string converted to a C-style shtring") {
			
				auto c_str=s.ToCString<char32_t>();
				
				THEN("It is of the same size as the literal") {
				
					REQUIRE(StringLength(literal)==c_str.Size());
				
				}
				
				THEN("It is identical to the literal") {
				
					REQUIRE(IsByteWiseEqual(literal,c_str));
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A string") {
	
		String s("Hello world");
	
		THEN(
			"Attempting to convert it to a C-style string that doesn't have a character size of "
			"8, 16, or 32 bits results in an exception"
		) {
		
			REQUIRE_THROWS_AS(s.ToCString<uint64_t>(),std::logic_error);
		
		}
	
	}

}


SCENARIO("Strings may be converted to strings suitable for consumption by operating system APIs","[string]") {

	#ifdef _WIN32
	GIVEN("The operating system is Microsoft Windows")
	#else
	GIVEN("The operating system is not Microsoft Windows")
	#endif
	{
	
		#ifdef _WIN32
		GIVEN("A UTF-16 literal")
		#else
		GIVEN("A UTF-8 literal")
		#endif
		{
		
			auto literal=
			#ifdef _WIN32
			u"Hello world"
			#else
			u8"Hello world"
			#endif
			;
			
			GIVEN("A string constructed from that literal") {
			
				String s(literal);
				
				GIVEN("That string converted to an operating system string") {
				
					auto os_str=s.ToOSString();
					
					THEN("The literal and the operating system string are identical") {
					
						REQUIRE(IsEqual(s,os_str));
					
					}
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be trimmed","[string]") {

	GIVEN("The empty string") {
	
		const String s;
		
		THEN("It does not change when trimmed") {
		
			CHECK(s.TrimFront()==s);
			CHECK(s.TrimRear()==s);
			REQUIRE(s.Trim()==s);
		
		}
	
	}
	
	GIVEN("A string with no trailing or leading white space") {
	
		const String s("hello world");
		
		THEN("It does not change when trimmed") {
		
			CHECK(s.TrimFront()==s);
			CHECK(s.TrimRear()==s);
			REQUIRE(s.Trim()==s);
		
		}
	
	}
	
	GIVEN("An immutable string") {
	
		const String s("    right     ");
		
		THEN("It may be trimmed, yielding a new string, but it remains unchanged") {
		
			REQUIRE(s.Trim()!=s);
		
		}
	
	}
	
	GIVEN("A string rvalue") {
	
		String s("    right     ");
		
		THEN("Trimming it moves it") {
		
			std::move(s).Trim();
			
			REQUIRE(s.Size()==0);
		
		}
	
	}
	
	GIVEN("A string with leading and trailing white space") {
	
		String s("     hello      ");
		
		GIVEN("The same string, with all leading white space removed") {
		
			String s2("hello      ");
			
			THEN("They are equivalent after having leading white space trimmed") {
			
				REQUIRE(s.TrimFront()==s2);
			
			}
		
		}
		
		GIVEN("The same string, with all trailing white space removed") {
		
			String s2("     hello");
			
			THEN("They are equivalent after having trailing white space removed") {
			
				REQUIRE(s.TrimRear()==s2);
			
			}
		
		}
		
		GIVEN("The same string, with all leading and trailing white space removed") {
		
			String s2("hello");
			
			THEN("They are equivalent after being trimmed") {
			
				REQUIRE(s.Trim()==s2);
			
			}
		
		}
	
	}
	
	GIVEN("A string with non-ASCII leading and trailing white space") {
	
		//	This string has a leading and trailing
		//	NO-BREAK SPACE (U+00A0)
		String s(" hello ");
		
		GIVEN("The same string, with all leading and trailing white space removed") {
		
			String s2("hello");
			
			THEN("They are equivalent after being trimmed") {
			
				REQUIRE(s.Trim()==s2);
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be swapped","[string]") {

	GIVEN("Two strings, one of which has a custom locale") {
	
		Locale l=DefaultLocale;
		l.Language="tr";
		String s_orig("hello");
		s_orig.SetLocale(l);
		String s2_orig("world");
		String s=s_orig;
		String s2=s2_orig;
		
		THEN("Swapping them causes each to become the other") {
		
			std::swap(s,s2);
			
			CHECK(s==s2_orig);
			CHECK(&(s.GetLocale())==&(s2_orig.GetLocale()));
			CHECK(s2==s_orig);
			REQUIRE(&(s2.GetLocale())==&(s_orig.GetLocale()));
		
		}
	
	}

}


SCENARIO("Strings may be concatenated","[string]") {

	GIVEN("Two strings") {
	
		String s("hello");
		String s2("world");
		
		GIVEN("A third string, which is the concatenation of those strings") {
		
			String s3("helloworld");
			
			THEN("Concatenating the first two results in the third") {
			
				REQUIRE((s+s2)==s3);
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be appended to other strings","[string]") {

	GIVEN("Two strings") {
	
		String s("hello");
		String s2("world");
		
		GIVEN("A third string, which is the concatenation of those strings") {
		
			String s3("helloworld");
		
			THEN("Appending the second to the first causes the first to be equivalent to the third") {
			
				s << s2;
				
				REQUIRE(s==s3);
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be hashed","[string]") {
	
	GIVEN("A string") {
	
		String s("naïveté");
		
		THEN("Hashing the string twice results in the same hash") {
		
			REQUIRE(std::hash<String>{}(s)==std::hash<String>{}(s));
		
		}
		
		GIVEN("An equivalent string") {
		
			String s2("naïveté");
		
			THEN("Hashing them results in the same hash") {
			
				REQUIRE(std::hash<String>{}(s)==std::hash<String>{}(s2));
			
			}
		
		}
		
		GIVEN("A string with the same characters in a different order") {
		
			String s2("néatveï");
			
			THEN("Hashing them results in a different hash") {
			
				REQUIRE(std::hash<String>{}(s)!=std::hash<String>{}(s2));
			
			}
		
		}
		
		GIVEN("A different string") {
		
			String s2("hello world");
			
			THEN("Hashing them results in a different hash") {
			
				REQUIRE(std::hash<String>{}(s)!=std::hash<String>{}(s2));
			
			}
		
		}
		
		GIVEN("A string which has that string as a substring") {
		
			String s2("Jonathan's naïveté");
			
			THEN("Hashing them results in a different hash") {
			
				REQUIRE(std::hash<String>{}(s)!=std::hash<String>{}(s2));
			
			}
		
		}
	
	}

}


//
//	UTF-8
//


SCENARIO("UTF-8 code units are the correct width","[utf8]") {

	GIVEN("UTF8::CodeUnit") {
	
		THEN("It is one byte wide") {
		
			REQUIRE(sizeof(UTF8::CodeUnit)==1);
		
		}
	
	}

}


SCENARIO("UTF-8 code units are unsigned","[utf8]") {

	GIVEN("UTF8::CodeUnit") {
	
		THEN("It is unsigned") {
		
			REQUIRE(std::is_unsigned<UTF8::CodeUnit>::value);
		
		}
	
	}

}


SCENARIO("The UTF-8 BOM may be obtained from a UTF-8 encoder/decoder","[utf8]") {

	GIVEN("A UTF-8 encoder/decoder") {
	
		UTF8 encoder;
		
		THEN("Its BOM is correct") {
		
			std::vector<unsigned char> bom;
			encoder.BOM().Get(bom);
			REQUIRE(bom.size()==3);
			CHECK(bom[0]==0xEF);
			CHECK(bom[1]==0xBB);
			REQUIRE(bom[2]==0xBF);
		
		}
	
	}

}


SCENARIO("Information about the representation of code points in UTF-8 is determined properly","[utf8]") {

	GIVEN("A UTF-8 encoder/decoder") {
	
		UTF8 encoder;
		
		GIVEN("U+0000") {
		
			CodePoint cp=0U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires one byte to represent") {
			
				REQUIRE(encoder.Count(cp)==1);
			
			}
		
		}
		
		GIVEN("U+007F") {
		
			CodePoint cp=0x7FU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires one byte to represent") {
			
				REQUIRE(encoder.Count(cp)==1);
			
			}
		
		}
		
		GIVEN("U+0080") {
		
			CodePoint cp=0x80U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires two bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==2);
			
			}
		
		}
		
		GIVEN("U+07FF") {
		
			CodePoint cp=0x7FFU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires two bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==2);
			
			}
		
		}
		
		GIVEN("U+0800") {
		
			CodePoint cp=0x800U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires three bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==3);
			
			}
		
		}
		
		GIVEN("U+FFFF") {
		
			CodePoint cp=0xFFFFU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires three bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==3);
			
			}
		
		}
		
		GIVEN("U+10000") {
		
			CodePoint cp=0x10000U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires four bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==4);
			
			}
		
		}
		
		GIVEN("U+1FFFFF") {
		
			CodePoint cp=0x1FFFFFU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires four bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==4);
			
			}
		
		}
		
		GIVEN("U+200000") {
		
			CodePoint cp=0x200000U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires five bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==5);
			
			}
		
		}
		
		GIVEN("U+3FFFFFF") {
		
			CodePoint cp=0x3FFFFFFU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires five bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==5);
			
			}
		
		}
		
		GIVEN("U+4000000") {
		
			CodePoint cp=0x4000000U;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires six bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==6);
			
			}
		
		}
		
		GIVEN("U+7FFFFFFF") {
		
			CodePoint cp=0x7FFFFFFFU;
			
			THEN("It can be represented") {
			
				REQUIRE(encoder.CanRepresent(cp));
			
			}
			
			THEN("It requires six bytes to represent") {
			
				REQUIRE(encoder.Count(cp)==6);
			
			}
		
		}
		
		GIVEN("A code point unrepresentable by UTF-8") {
		
			CodePoint cp=0x7FFFFFFFU+1;
			
			THEN("It cannot be represented") {
			
				CHECK(!encoder.CanRepresent(cp));
				REQUIRE(encoder.Count(cp)==0);
			
			}
		
		}
	
	}

}


SCENARIO("UTF-8 strings may be decoded","[utf8]") {

	GIVEN("A UTF-8 encoder/decoder") {
	
		UTF8 encoder;
		
		THEN("Decoding an empty buffer represented as two null iterators results in the empty string") {
		
			REQUIRE(encoder.Decode(nullptr,nullptr).size()==0);
		
		}
		
		GIVEN("An empty buffer of bytes") {
		
			std::vector<unsigned char> buffer;
			
			THEN("Attempting to decode the buffer results in the empty string") {
			
				REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
			
			}
		
		}
		
		GIVEN("A buffer containing ASCII") {
		
			auto str="Hello world";
			auto end=str+StringLength(str);
			
			THEN("Decoding the buffer recovers the ASCII") {
			
				REQUIRE(IsEqual(encoder.Decode(str,end),str));
			
			}
		
		}
		
		GIVEN("A buffer containing a two byte sequence") {
		
			std::vector<unsigned char> buffer={0xC3,0xA5};
			
			THEN("Decoding the buffer recovers the code point") {
			
				auto decoded=encoder.Decode(Begin(buffer),End(buffer));
				REQUIRE(decoded.size()==1);
				REQUIRE(decoded[0]==0xE5);
			
			}
		
		}
		
		GIVEN("A buffer containing a three byte sequence") {
		
			std::vector<unsigned char> buffer={0xE2,0x80,0x8B};
			
			THEN("Decoding the buffer recovers the code point") {
			
				auto decoded=encoder.Decode(Begin(buffer),End(buffer));
				REQUIRE(decoded.size()==1);
				REQUIRE(decoded[0]==0x200B);
			
			}
		
		}
		
		GIVEN("A buffer containing a four byte sequence") {
		
			std::vector<unsigned char> buffer={0xF0,0x9D,0x84,0x9E};
			
			THEN("Decoding the buffer recovers the code point") {
			
				auto decoded=encoder.Decode(Begin(buffer),End(buffer));
				REQUIRE(decoded.size()==1);
				REQUIRE(decoded[0]==0x1D11E);
			
			}
		
		}
		
		GIVEN("A buffer containing a four byte sequence which represents a code point larger than U+10FFFF") {
		
			std::vector<unsigned char> buffer={0xF4,0x90,0x80,0x80};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer recovers the code point") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0x110000);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer containing a five byte sequence") {
		
			std::vector<unsigned char> buffer={0xF8,0xBF,0xBF,0xBF,0xBF};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer recovers the code point") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0xFFFFFF);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
			
				THEN("Attempting to decode the buffer results in the replacement") {
			
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
					
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer containing a six byte sequence") {
		
			std::vector<unsigned char> buffer={0xFC,0xBF,0xBF,0xBF,0xBF,0xBF};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer recovers the code point") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0x3FFFFFFF);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
			
				THEN("Attempting to decode the buffer results in the replacement") {
			
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
					
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer containing a seven byte sequence") {
		
			std::vector<unsigned char> buffer={0xFE,0xBF,0xBF,0xBF,0xBF,0xBF,0xBF};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==7);
					CHECK(decoded[0]==replacement);
					CHECK(decoded[1]==replacement);
					CHECK(decoded[2]==replacement);
					CHECK(decoded[3]==replacement);
					CHECK(decoded[4]==replacement);
					CHECK(decoded[5]==replacement);
					REQUIRE(decoded[6]==replacement);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer containing an eight byte sequence") {
		
			std::vector<unsigned char> buffer={0xFF,0xBF,0xBF,0xBF,0xBF,0xBF,0xBF,0xBF};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==8);
					CHECK(decoded[0]==replacement);
					CHECK(decoded[1]==replacement);
					CHECK(decoded[2]==replacement);
					CHECK(decoded[3]==replacement);
					CHECK(decoded[4]==replacement);
					CHECK(decoded[5]==replacement);
					CHECK(decoded[6]==replacement);
					REQUIRE(decoded[7]==replacement);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer of bytes representing a UTF-16 surrogate") {
		
			UTF8 temp;
			temp.UnicodeStrict.Ignore();
			std::vector<CodePoint> cps={static_cast<CodePoint::Type>(0xDC00)};
			auto buffer=temp.Encode(Begin(cps),End(cps));
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer results in the UTF-16 surrogate") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0xDC00);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer of bytes representing U+FFFE") {
		
			UTF8 temp;
			temp.UnicodeStrict.Ignore();
			std::vector<CodePoint> cps={static_cast<CodePoint::Type>(0xFFFE)};
			auto buffer=temp.Encode(Begin(cps),End(cps));
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer results in the UTF-16 surrogate") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0xFFFE);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer of bytes representing U+FFFF") {
		
			UTF8 temp;
			temp.UnicodeStrict.Ignore();
			std::vector<CodePoint> cps={static_cast<CodePoint::Type>(0xFFFF)};
			auto buffer=temp.Encode(Begin(cps),End(cps));
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are being ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to decode the buffer results in the UTF-16 surrogate") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0xFFFF);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnicodeStrict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A buffer of bytes containing an invalid UTF-8 sequence") {
		
			std::vector<unsigned char> buffer={0x87,'a'};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer results in everything except the problematic area being decoded") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]=='a');
				
				}
			
			}
			
			GIVEN("Strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the invalid sequence being replaced by the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==2);
					CHECK(decoded[0]==replacement);
					REQUIRE(decoded[1]=='a');
				
				}
			
			}
			
			GIVEN("Strict errors result in no action") {
			
				encoder.Strict.Nothing();
				
				THEN("Attempting to decode the buffer results in everything except the problematic area being decoded") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]=='a');
				
				}
			
			}
		
		}
	
		GIVEN("A buffer of bytes containing an overlong UTF-8 sequence") {
		
			std::vector<unsigned char> buffer={0xF0,0x82,0x82,0xAC};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer results in the code point being recovered") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==0x20AC);
				
				}
			
			}
			
			GIVEN("Strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Strict errors result in no action") {
			
				encoder.Strict.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					REQUIRE(encoder.Decode(Begin(buffer),End(buffer)).size()==0);
				
				}
			
			}
		
		}
	
		GIVEN("A buffer of bytes containing an incomplete multi-byte sequence followed by a single byte sequence") {
		
			//	This is MUSICAL SYMBOL G CLEF minus the last byte
			//	followed by LATIN SMALL LETTER A
			std::vector<unsigned char> buffer={0xF0,0x9D,0x84,'a'};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Strict errors are being ignored") {
			
				encoder.Strict.Ignore();
				
				THEN("Attempting to decode the buffer recovers the code point corresponding to the single byte sequence") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]=='a');
				
				}
			
			}
			
			GIVEN("Strict errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.Strict.Replace(replacement);
				
				THEN(
					"Attempting to decode the buffer results in the replacement "
					"followed by the code point corresponding to the single byte sequence"
				) {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==2);
					CHECK(decoded[0]==replacement);
					REQUIRE(decoded[1]=='a');
				
				}
			
			}
			
			GIVEN("Strict errors result in no action") {
			
				encoder.Strict.Nothing();
				
				THEN("Attempting to decode the buffer recovers the code point corresponding to the single byte sequence") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]=='a');
				
				}
			
			}
		
		}
	
		GIVEN("A buffer of bytes containing an incomplete multi-byte sequence") {
		
			//	This is MUSICAL SYMBOL G CLEF minus the last byte
			std::vector<unsigned char> buffer={0xF0,0x9D,0x84};
			
			THEN("Attempting to decode the buffer raises an exception") {
			
				REQUIRE_THROWS_AS(encoder.Decode(Begin(buffer),End(buffer)),EncodingError);
			
			}
			
			GIVEN("Unexpected end errors are being ignored") {
			
				encoder.UnexpectedEnd.Ignore();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==0);
				
				}
			
			}
			
			GIVEN("Unexpected end errors result in a replacement") {
			
				CodePoint replacement='?';
				encoder.UnexpectedEnd.Replace(replacement);
				
				THEN("Attempting to decode the buffer results in the replacement") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					CHECK(decoded[0]==replacement);
				
				}
			
			}
			
			GIVEN("Unexpected end errors result in no action") {
			
				encoder.UnexpectedEnd.Nothing();
				
				THEN("Attempting to decode the buffer results in the empty string") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==0);
				
				}
			
			}
		
		}
	
		GIVEN("A buffer of bytes beginning with the BOM") {
		
			std::vector<unsigned char> buffer={0xEF,0xBB,0xBF,'a'};
			
			THEN("Decoding the buffer recovers the BOM as one of the code points") {
			
				auto decoded=encoder.Decode(Begin(buffer),End(buffer));
				REQUIRE(decoded.size()==2);
				CHECK(decoded[0]==0xFEFF);
				REQUIRE(decoded[1]=='a');
			
			}
			
			GIVEN("The decoder should attempt to detect the BOM") {
			
				encoder.DetectBOM=true;
				
				THEN("Decoding the buffer does not recover the BOM") {
				
					auto decoded=encoder.Decode(Begin(buffer),End(buffer));
					REQUIRE(decoded.size()==1);
					REQUIRE(decoded[0]=='a');
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Strings may be encoded to UTF-8","[utf8]") {

	GIVEN("A UTF-8 encoder/decoder") {
	
		UTF8 encoder;
		
		GIVEN("A string containing only code points which may be represented with one code unit") {
		
			String s("Hello world");
			
			THEN("It may be encoded") {
			
				REQUIRE(IsEqual(encoder.Encode(s),s));
			
			}
		
		}
		
		GIVEN("A string containing code points which require multiple code units to represent") {
		
			String s(u8"м");
			
			THEN("It may be encoded") {
			
				auto encoded=encoder.Encode(s);
				REQUIRE(encoded.size()==2);
				CHECK(encoded[0]==0xD0);
				REQUIRE(encoded[1]==0xBC);
			
			}
		
		}
	
		GIVEN("A string containing illegal Unicode") {
		
			String s;
			s << CodePoint(0x110000U);
			
			THEN("Attempting to encode it results in an exception") {
			
				REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
			
			}
			
			GIVEN("Unicode strict errors are ignored") {
			
				encoder.UnicodeStrict.Ignore();
				
				THEN("Attempting to encode it succeeds") {
				
					REQUIRE(IsEqual(encoder.Encode(s),std::vector<unsigned char>({0xF4,0x90,0x80,0x80})));
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in no action") {
			
				encoder.UnicodeStrict.Nothing();
				
				THEN("Attempting to encode it results in the empty string") {
				
					REQUIRE(encoder.Encode(s).size()==0);
				
				}
			
			}
			
			GIVEN("Unicode strict errors result in a replacement") {
			
				GIVEN("The replacement is representable in UTF-8") {
				
					CodePoint replacement='?';
					encoder.UnicodeStrict.Replace(replacement);
					
					THEN("Attempting to encode the string results in the replacement") {
					
						auto encoded=encoder.Encode(s);
						REQUIRE(encoded.size()==1);
						REQUIRE(encoded[0]==replacement);
					
					}
				
				}
				
				GIVEN("The replacement is not representable in UTF-8") {
				
					CodePoint replacement=std::numeric_limits<CodePoint::Type>::max();
					encoder.UnicodeStrict.Replace(replacement);
					
					THEN("Attempting to encode the string results in an exception") {
					
						REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
					
					}
				
				}
			
			}
		
		}
	
		GIVEN("A string containing code points which UTF-8 cannot represent") {
		
			String s;
			s << CodePoint(std::numeric_limits<CodePoint::Type>::max());
			
			GIVEN("Unicode strict errors are ignored") {
			
				encoder.UnicodeStrict.Ignore();
			
				THEN("Attempting to encode it results in an exception") {
				
					REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
				
				}
				
				GIVEN("Lossy errors are ignored") {
				
					encoder.Lossy.Ignore();
					
					THEN("Attempting to encode the string results in an empty buffer") {
					
						REQUIRE(encoder.Encode(s).size()==0);
					
					}
				
				}
				
				GIVEN("Lossy errors result in a replacement") {
				
					GIVEN("The replacement is representable in UTF-8") {
					
						CodePoint replacement='?';
						encoder.Lossy.Replace(replacement);
						
						THEN("Attempting to encode the string results in the replacement") {
						
							auto encoded=encoder.Encode(s);
							REQUIRE(encoded.size()==1);
							REQUIRE(encoded[0]==replacement);
						
						}
					
					}
					
					GIVEN("The replacement is not representable in UTF-8") {
					
						CodePoint replacement=std::numeric_limits<CodePoint::Type>::max();
						encoder.Lossy.Replace(replacement);
						
						THEN("Attempting to encode the string results in an exception") {
						
							REQUIRE_THROWS_AS(encoder.Encode(s),EncodingError);
						
						}
					
					}
				
				}
				
				GIVEN("Lossy errors result in no action") {
				
					encoder.Lossy.Nothing();
					
					THEN("Attempting to encode the string results in an empty buffer") {
					
						REQUIRE(encoder.Encode(s).size()==0);
					
					}
				
				}
			
			}
			
			GIVEN("A string representable in UTF-8") {
			
				String s("Hello world");
				
				GIVEN("The encoder/decoder is set to output the BOM") {
				
					encoder.OutputBOM=true;
					
					THEN("Encoding the string outputs the BOM") {
					
						auto encoded=encoder.Encode(s);
						REQUIRE(encoded.size()==(s.Size()+3));
						CHECK(encoded[0]==0xEF);
						CHECK(encoded[1]==0xBB);
						REQUIRE(encoded[2]==0xBF);
					
					}
				
				}
			
			}
			
		}
		
	}

}
