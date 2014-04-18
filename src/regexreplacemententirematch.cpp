#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexReplacementEntireMatch : public RegexReplacementElement {
		
		
			public:
			
			
				virtual String operator () (RegexMatch & match, const CodePoint *, const CodePoint *) const override {
				
					return match.Get();
				
				}
		
		
		};
		
		
		class RegexReplacementEntireMatchParser : public RegexReplacementParser {
		
		
			public:
			
			
				virtual bool operator () (RegexReplacementCompiler & compiler) const override {
				
					if (!compiler.IsNext("$&")) return false;
					
					compiler.Add<RegexReplacementEntireMatch>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexReplacementParserInstaller<RegexReplacementEntireMatchParser> installer;


}
