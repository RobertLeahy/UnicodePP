#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexValidUnicode : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
			
			
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					if (engine && engine->IsValid()) {
					
						++engine;
						
						return true;
					
					}
					
					return false;
				
				}
			
			
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="Any valid Unicode code point";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexValidUnicodeParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("\\X")) return false;
					
					compiler.Add<RegexValidUnicode>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexValidUnicodeParser> installer;


}
