#include <unicode/codepoint.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexLiteral : public RegexPatternElement {
		
		
			private:
			
			
				std::vector<CodePoint> cps;
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual RegexToString ToString () const override {
				
					return RegexToString{};
				
				}
				
				
				void Add (CodePoint cp) {
				
					cps.push_back(cp);
				
				}
		
		
		};
	
	
		class RegexLiteralParser : public RegexParser {
		
		
			public:
		
		
				virtual bool operator () (
					RegexCompiler::Pattern & pattern,
					bool successive,
					const CodePoint * & begin,
					const CodePoint *,
					RegexOptions options,
					const Locale & locale
				) const override {
				
					if (!successive) pattern.push_back(
						RegexCompiler::Element(
							new RegexLiteral(options,locale)
						)
					);
					
					reinterpret_cast<RegexLiteral *>(
						pattern.back().get()
					)->Add(*begin);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static RegexParserInstaller<RegexLiteralParser> installer;


}
