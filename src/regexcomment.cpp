#include <unicode/boundaries.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexCommentParser : public RegexParser {
		
		
			private:
			
			
				static bool skip_comment (RegexCompilerState & state) {
				
					if (!(
						++state && (*state=='?') &&
						++state && (*state=='#')
					)) return false;
					
					while (++state) if (*state==')') {
					
						++state;
						
						return true;
					
					}
					
					state.Raise("No end of comment");
				
				}
				
				
				static void skip_line_comment (RegexCompilerState & state) noexcept {
				
					for (;state && !IsLineBreak(state.Current,state.Begin,state.End,state.Locale);++state);
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompilerState & state) const override {
				
					switch (*state) {
					
						case '#':
							if (state.Check(RegexOptions::IgnorePatternWhiteSpace)) {
							
								skip_line_comment(state);
								
								return true;
								
							}
							return false;
						case '(':
							return skip_comment(state);
						default:
							break;
					
					}
				
					return false;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCommentParser> installer;


}
