#include <unicode/codepoint.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>
#include <limits>


namespace Unicode {


	namespace {
	
	
		class RegexReplacementLiteral : public RegexReplacementElement {
		
		
			private:
			
			
				String str;
				
				
			public:
			
			
				virtual String operator () (RegexMatch &, const CodePoint *, const CodePoint *) const override {
				
					return str;
				
				}
				
				
				void Add (CodePoint cp) {
				
					str << cp;
				
				}
		
		
		};
		
		
		class RegexReplacementLiteralParser : public RegexReplacementParser {
		
		
			private:
			
			
				typedef RegexReplacementLiteral type;
				
				
				static CodePoint get (RegexReplacementCompiler & compiler) noexcept {
				
					if (*compiler!='$') return *compiler;
					
					if (++compiler && (*compiler=='$')) ++compiler;
					
					return '$';
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexReplacementCompiler & compiler) const {
				
					auto & element=compiler.Successive ? compiler.Back<type>() : compiler.Add<type>();
					element.Add(get(compiler));
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexReplacementParserInstaller<RegexReplacementLiteralParser> installer(std::numeric_limits<std::size_t>::max());


}
