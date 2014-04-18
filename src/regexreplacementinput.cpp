#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexReplacementBeforeMatch : public RegexReplacementElement {
		
		
			public:
			
			
				virtual String operator () (RegexMatch & match, const CodePoint * begin, const CodePoint *) const override {
				
					return String(begin,match.begin());
				
				}
		
		
		};
		
		
		class RegexReplacementAfterMatch : public RegexReplacementElement {
		
		
			public:
			
			
				virtual String operator () (RegexMatch & match, const CodePoint *, const CodePoint * end) const override {
				
					return String(match.end(),end);
				
				}
		
		
		};
		
		
		class RegexReplacementEntireInput : public RegexReplacementElement {
		
		
			public:
			
			
				virtual String operator () (RegexMatch &, const CodePoint * begin, const CodePoint * end) const override {
				
					return String(begin,end);
				
				}
		
		
		};
		
		
		class RegexReplacementInputParser : public RegexReplacementParser {
		
		
			public:
			
			
				virtual bool operator () (RegexReplacementCompiler & compiler) const override {
				
					if (!(compiler.IsNext('$') && compiler)) return false;
					
					switch (*compiler) {
					
						case '`':
							compiler.Add<RegexReplacementBeforeMatch>();
							break;
						case '\'':
							compiler.Add<RegexReplacementAfterMatch>();
							break;
						case '_':
							compiler.Add<RegexReplacementEntireInput>();
							break;
						default:
							return false;
					
					}
					
					++compiler;
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexReplacementParserInstaller<RegexReplacementInputParser> installer;


}
