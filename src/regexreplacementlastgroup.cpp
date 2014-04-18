#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexReplacementLastGroup : public RegexReplacementElement {
		
		
			private:
			
			
				static void process (
					const CodePoint * begin_a,
					const CodePoint * end_a,
					const CodePoint * & begin_b,
					const CodePoint * & end_b
				) noexcept {
				
					if (begin_b>begin_a) return;
				
					if (begin_b<begin_a) {
					
						begin_b=begin_a;
						end_b=end_a;
						
						return;
					
					}
					
					if (end_b>end_a) end_b=end_a;
				
				}
			
			
				static void scan (const RegexMatch::Type & vec, const CodePoint * & begin, const CodePoint * & end) noexcept {
				
					for (auto & capture : vec) process(capture.begin(),capture.end(),begin,end);
				
				}
			
			
				template <typename T>
				static void scan (const RegexMatch::Map<T> & map, const CodePoint * & begin, const CodePoint * & end) noexcept {
				
					for (auto & pair : map) scan(pair.second,begin,end);
				
				}
		
		
			public:
			
			
				virtual String operator () (RegexMatch & match, const CodePoint *, const CodePoint *) const override {
				
					const CodePoint * begin=nullptr;
					const CodePoint * end=nullptr;
					scan(match.Numbered(),begin,end);
					scan(match.Named(),begin,end);
					
					return String(begin,end);
				
				}
		
		
		};
		
		
		class RegexReplacementLastGroupParser : public RegexReplacementParser {
		
		
			public:
			
			
				virtual bool operator () (RegexReplacementCompiler & compiler) const override {
				
					if (!compiler.IsNext("$+")) return false;
					
					compiler.Add<RegexReplacementLastGroup>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexReplacementParserInstaller<RegexReplacementLastGroupParser> installer;


}
