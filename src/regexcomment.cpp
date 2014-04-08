#include <unicode/boundaries.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexCommentParser : public RegexParser {
		
		
			private:
			
			
				static bool skip_comment (RegexCompiler & compiler) {
				
					if (!(
						++compiler && (*compiler=='?') &&
						++compiler && (*compiler=='#')
					)) return false;
					
					while (++compiler) if (*compiler==')') {
					
						++compiler;
						
						return true;
					
					}
					
					compiler.Raise("No end of comment");
				
				}
				
				
				static void skip_line_comment (RegexCompiler & compiler) noexcept {
				
					for (;compiler && !IsLineBreak(compiler.Current,compiler.Begin,compiler.End,compiler.Locale);++compiler);
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					switch (*compiler) {
					
						case '#':
							if (compiler.Check(RegexOptions::IgnorePatternWhiteSpace)) {
							
								skip_line_comment(compiler);
								
								return true;
								
							}
							return false;
						case '(':
							return skip_comment(compiler);
						default:
							break;
					
					}
				
					return false;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCommentParser> installer;


}
