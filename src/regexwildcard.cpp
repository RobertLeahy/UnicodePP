#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexWildcard : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					if (
						!state ||
						(
							!Check(RegexOptions::Singleline) &&
							(*state=='\n')
						)
					) return false;
					
					++state;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Any code point";
					if (!Check(RegexOptions::Singleline)) retr.Parent << " except LINE FEED (LF) (U+000A)";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexWildcardParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & state) const override {
				
					if (*state!='.') return false;
					
					state.Add<RegexWildcard>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexWildcardParser> installer;


}
