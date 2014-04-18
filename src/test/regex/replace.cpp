#include <unicode/regex.hpp>
#include "../catch.hpp"
#include <catch.hpp>


using namespace Unicode;


SCENARIO("Custom callbacks may be used to replace matches in a string","[replace]") {

	GIVEN("A regular expression which matches each word") {
	
		String pattern("\\w+");
		
		GIVEN("A callback which surrounds each match with quotes") {
		
			auto callback=[] (RegexMatch & match, const CodePoint *, const CodePoint *) {
				
				String retr;
				retr << "\"" << match.Get() << "\"";
				
				return retr;
			
			};
			
			GIVEN("RegexOptions::RightToLeft is not set") {
		
				Regex r(pattern);
				
				WHEN("The empty string is processed") {
				
					auto s=r.Replace(String{},callback);
					
					THEN("The result is the empty string") {
					
						CHECK(s.Size()==0);
					
					}
				
				}
				
				WHEN("A string containing only white space is processed") {
				
					auto str="   ";
					auto s=r.Replace(str,callback);
					
					THEN("It is unchanged") {
					
						CHECK(s==str);
					
					}
				
				}
				
				WHEN("A string containing one word is processed") {
				
					auto s=r.Replace("hello",callback);
					
					THEN("The result is that word, quoted") {
					
						CHECK(s=="\"hello\"");
					
					}
				
				}
				
				WHEN("A string containing many words is processed") {
				
					auto s=r.Replace("hello world",callback);
					
					THEN("The result is each of those words, quoted") {
					
						CHECK(s=="\"hello\" \"world\"");
					
					}
				
				}
				
				WHEN("A string containing many words, with trailing and leading white space is processed") {
				
					auto s=r.Replace(" hello world ",callback);
					
					THEN("The result is each of those words, quoted, and the trailing and leading white space is preserved") {
					
						CHECK(s==" \"hello\" \"world\" ");
					
					}
				
				}
				
			}
			
			GIVEN("RegexOptions::RightToLeft is set") {
		
				Regex r(pattern,RegexOptions::RightToLeft);
				
				WHEN("The empty string is processed") {
				
					auto s=r.Replace(String{},callback);
					
					THEN("The result is the empty string") {
					
						CHECK(s.Size()==0);
					
					}
				
				}
				
				WHEN("A string containing only white space is processed") {
				
					auto str="   ";
					auto s=r.Replace(str,callback);
					
					THEN("It is unchanged") {
					
						CHECK(s==str);
					
					}
				
				}
				
				WHEN("A string containing one word is processed") {
				
					auto s=r.Replace("hello",callback);
					
					THEN("The result is that word, quoted") {
					
						CHECK(s=="\"hello\"");
					
					}
				
				}
				
				WHEN("A string containing many words is processed") {
				
					auto s=r.Replace("hello world",callback);
					
					THEN("The result is each of those words, quoted") {
					
						CHECK(s=="\"hello\" \"world\"");
					
					}
				
				}
				
				WHEN("A string containing many words, with trailing and leading white space is processed") {
				
					auto s=r.Replace(" hello world ",callback);
					
					THEN("The result is each of those words, quoted, and the trailing and leading white space is preserved") {
					
						CHECK(s==" \"hello\" \"world\" ");
					
					}
				
				}
				
			}
		
		}
	
	}

}


SCENARIO("Replacement strings containing only literals may be used to replace matches in a string","[replace]") {

	GIVEN("A regular expression which matches each word") {
	
		Regex r("\\w+");
		
		GIVEN("A string containing many words") {
		
			String str("hello world");
			
			GIVEN("A replacement which replaces each match with a string that does not contain '$'") {
			
				RegexReplacement repl("hello");
				
				WHEN("The string is processed") {
				
					auto s=r.Replace(str,repl);
				
					THEN("The result is each word replaced by the replacement") {
					
						CHECK(s=="hello hello");
					
					}
				
				}
			
			}
			
			GIVEN("A replacement which contains only '$'") {
			
				RegexReplacement repl("$");
				
				WHEN("The string is processed") {
				
					auto s=r.Replace(str,repl);
					
					THEN("The result is each word replaced by '$'") {
					
						CHECK(s=="$ $");
					
					}
				
				}
			
			}
			
			GIVEN("A replacement which contains '$', but not in a position to be interpreted as a metacharacter") {
			
				RegexReplacement repl("$hello$ $good-bye$");
				
				WHEN("The string is processed") {
				
					auto s=r.Replace(str,repl);
					
					THEN("The result is each word replaced by the literal replacement") {
					
						CHECK(s=="$hello$ $good-bye$ $hello$ $good-bye$");
					
					}
				
				}
			
			}
			
			GIVEN("A replacement which contains '$' escaped with another '$'") {
			
				RegexReplacement repl("$$");
				
				WHEN("The string is processed") {
				
					auto s=r.Replace(str,repl);
					
					THEN("\"$$\" is treated as \"$\"") {
					
						CHECK(s=="$ $");
					
					}
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("Numbered or named capturing groups may be substituted by a replacement string using backreferences","[replace]") {

	GIVEN("A regular expression which does not capture") {
	
		Regex r("hello");
		
		GIVEN("A replacement which refers to some capturing group") {
		
			RegexReplacement repl("\"$1\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The empty string is substituted") {
				
					CHECK(s=="\"\"");
				
				}
			
			}
		
		}
	
	}

	GIVEN("A regular expression which captures a numbered group whose number is 1 decimal digit") {
	
		Regex r("(hello)");
		
		GIVEN("A replacement which attempts to refer to this capturing group using \"$n\" syntax with a leading zero") {
		
			RegexReplacement repl("\"$01\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The literal replacement string is substituted") {
				
					CHECK(s=="\"$01\"");
				
				}
			
			}
		
		}
		
		GIVEN("A replacement which refers to this capturing group using \"$n\" syntax") {
		
			RegexReplacement repl("\"$1\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The captured group is substituted") {
				
					CHECK(s=="\"hello\"");
				
				}
			
			}
		
		}
		
		GIVEN("A replacement which refers to this capturing group using \"${n}\" syntax") {
		
			RegexReplacement repl("\"${1}\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The captured group is substituted") {
				
					CHECK(s=="\"hello\"");
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A regular expression which captures a numbered group whose number is 2 decimal digits") {
	
		Regex r("(?<20>hello)");
		
		GIVEN("A replacement which refers to this capturing group using \"$n\" syntax") {
		
			RegexReplacement repl("\"$20\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The captured group is substituted") {
				
					CHECK(s=="\"hello\"");
				
				}
			
			}
		
		}
		
		GIVEN("A replacement which refers to this capturing group using \"${n}\" syntax") {
		
			RegexReplacement repl("\"${20}\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The captured group is substituted") {
				
					CHECK(s=="\"hello\"");
				
				}
			
			}
		
		}
	
	}
	
	GIVEN("A regular expression which captures a named group") {
	
		Regex r("(?<hello>hello)");
		
		GIVEN("A replacement which refers to this capturing group") {
		
			RegexReplacement repl("\"${hello}\"");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The captured group is substituted") {
				
					CHECK(s=="\"hello\"");
				
				}
			
			}
		
		}
	
	}

	GIVEN("A regular expression which can capture the same group multiple times") {
	
		Regex r("(\\w)+");
		
		GIVEN("A replacement which refers to this capturing group") {
		
			RegexReplacement repl("$1");
			
			WHEN("A string which matches the regular expression and results in many captures is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The first capture is substituted") {
				
					CHECK(s=="h");
				
				}
			
			}
		
		}
	
	}
	
}


SCENARIO("The last group captured may be substituted","[replace]") {

	GIVEN("A replacement which substitutes the last group captured") {
	
		RegexReplacement repl("$+");
		
		GIVEN("A regular expression which captures once") {
		
			Regex r("(hello)");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The result is the capture") {
				
					CHECK(s=="hello");
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which can capture the same group many times") {
		
			Regex r("(\\w)+");
			
			WHEN("A string which matches the regular expression and results in many captures is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The last capture is substituted") {
				
					CHECK(s=="o");
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which captures multiple groups") {
		
			Regex r("(hello) (world)");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello world",repl);
				
				THEN("The result of the last capture is substituted") {
				
					CHECK(s=="world");
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which captures multiple, nested groups") {
		
			Regex r("(he(llo))");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The result of the innermost group is substituted") {
				
					CHECK(s=="llo");
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which captures multiple, nested groups, which all begin at the same place") {
		
			Regex r("((he)llo)");
			
			WHEN("A string which matches the regular expression is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The result of the innermost group is substituted") {
				
					CHECK(s=="he");
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("The entire match may be substituted","[replace]") {

	GIVEN("A replacement which substitutes the entire match") {
	
		RegexReplacement repl("$&");
		
		GIVEN("A regular expression which matches the entire input") {
		
			Regex r(".+");
			
			WHEN("Any string is processed") {
			
				String str("hello");
				auto s=r.Replace(str,repl);
				
				THEN("The result is the string") {
				
					CHECK(s==str);
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which matches some subset of the input") {
		
			Regex r("\\S+");
			
			WHEN("A string which matches twice is processed") {
			
				String str("hello good-bye");
				auto s=r.Replace(str,repl);
				
				THEN("The result is the input") {
				
					CHECK(s==str);
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("The entire input may be substituted","[replace]") {

	GIVEN("A replacement which substitutes the entire input") {
	
		RegexReplacement repl("$_");
		
		GIVEN("A regular expression which matches the entire input") {
		
			Regex r(".+");
			
			WHEN("Any string is processed") {
			
				String str("hello");
				auto s=r.Replace(str,repl);
				
				THEN("The result is the string") {
				
					CHECK(s==str);
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which matches some subset of the input") {
		
			Regex r("\\S+");
			
			WHEN("A string which matches twice is processed") {
			
				auto s=r.Replace("hello good-bye",repl);
				
				THEN("The result is the input, twice") {
				
					CHECK(s=="hello good-bye hello good-bye");
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("The input before each match may be substituted","[replace]") {

	GIVEN("A replacement which substitutes the input preceding the match") {
	
		RegexReplacement repl("$`");
		
		GIVEN("A regular expression which matches the entire input") {
		
			Regex r(".+");
			
			WHEN("Any string is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The result is the empty string") {
				
					CHECK(s.Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which matches some subset of the input") {
		
			Regex r("\\S+");
			
			WHEN("A string which matches twice is processed") {
			
				auto s=r.Replace("hello good-bye",repl);
				
				THEN("The result is the input before the second match, and everything which was not matched") {
				
					CHECK(s==" hello ");
				
				}
			
			}
		
		}
	
	}

}


SCENARIO("The input after each match may be substituted","[replace]") {

	GIVEN("A replacement which substitutes the input following the match") {
	
		RegexReplacement repl("$'");
		
		GIVEN("A regular expression which matches the entire input") {
		
			Regex r(".+");
			
			WHEN("Any string is processed") {
			
				auto s=r.Replace("hello",repl);
				
				THEN("The result is the empty string") {
				
					CHECK(s.Size()==0);
				
				}
			
			}
		
		}
		
		GIVEN("A regular expression which matches some subset of the input") {
		
			Regex r("\\S+");
			
			WHEN("A string which matches twice is processed") {
			
				auto s=r.Replace("hello good-bye",repl);
				
				THEN("The result is the input after the first match, and everything which was not matched") {
				
					CHECK(s==" good-bye ");
				
				}
			
			}
		
		}
	
	}

}
