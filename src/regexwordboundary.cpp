#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexWordBoundary : public RegexPatternElement {
		
		
			private:
			
			
				bool inverted;
				
				
				//	Checks to see if a certain character should be
				//	considered a word character
				bool check (CodePoint cp) const noexcept {
				
					auto cpi=cp.GetInfo(Locale);
					if (cpi==nullptr) return false;
					
					switch (cpi->GeneralCategory) {
					
						case GeneralCategory::Ll:
						case GeneralCategory::Lu:
						case GeneralCategory::Lt:
						case GeneralCategory::Lo:
						case GeneralCategory::Lm:
						case GeneralCategory::Nd:
						case GeneralCategory::Pc:
							return true;
						default:
							return false;
					
					}
				
				}
				
				
				bool check_prev (RegexEngine & engine) const noexcept {
				
					//	At the beginning, can't back up, so the previous
					//	code point can't be a word character
					if (engine.AtBeginning()) return false;
					
					--engine;
					auto retr=check(*engine);
					++engine;
					
					return retr;
				
				}
				
				
				bool check (const RegexEngine & engine) const noexcept {
				
					return engine ? check(*engine) : false;
				
				}
				
				
			public:
			
			
				RegexWordBoundary (bool inverted, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					return (check_prev(engine)!=check(engine))!=inverted;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent=inverted ? "Anywhere but b" : "B";
					retr.Parent << "etween a word and non-word code point";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexWordBoundaryParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!(compiler.IsNext('\\') && compiler)) return false;
					
					bool inverted;
					switch (*compiler) {
					
						case 'b':
							inverted=false;
							break;
						case 'B':
							inverted=true;
							break;
						default:
							return false;
					
					}
					
					++compiler;
					
					compiler.Add<RegexWordBoundary>(inverted);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexWordBoundaryParser> installer(false);


}
