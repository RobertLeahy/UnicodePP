#include <unicode/boundaries.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexCommentParser : public RegexParser {
		
		
			private:
			
			
				static void skip_comment (RegexCompilerState & state) {
				
					if (!(
						++state && (*state=='?') &&
						++state && (*state=='#')
					)) return;
					
					state.Fail=false;
					
					while (++state) if (*state==')') {
					
						++state;
						
						return;
					
					}
					
					state.Raise("No end of comment");
				
				}
				
				
				static void skip_line_comment (RegexCompilerState & state) noexcept {
				
					state.Fail=false;
				
					for (;state && !IsLineBreak(state.Current,state.Begin,state.End,state.Locale);++state);
				
				}
		
		
			public:
			
			
				virtual RegexCompiler::Element operator () (RegexCompilerState & state) const override {
				
					switch (*state) {
					
						case '#':
							if (state.Check(RegexOptions::IgnorePatternWhiteSpace)) skip_line_comment(state);
							break;
						case '(':
							skip_comment(state);
							break;
						default:
							break;
					
					}
				
					//	Comments never turn into a pattern
					//	element
					return RegexCompiler::Element{};
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCommentParser> installer;


}
