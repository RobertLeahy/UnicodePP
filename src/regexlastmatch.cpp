#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexLastMatch : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
			
			
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					return engine.begin().Base()==engine.Last();
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="The location at which the last match ended";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexLastMatchParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("\\G")) return false;
					
					compiler.Add<RegexLastMatch>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexLastMatchParser> installer(false);


}
