#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexWhiteSpace {
		
		
			public:
			
			
				static bool Check (CodePoint cp, const Locale & locale) noexcept {
				
					return cp.IsWhiteSpace(locale);
				
				}
				
				
				static String ToString (bool inverted) {
				
					String str("Any ");
					if (inverted) str << "non-";
					str << "white space code point";
					
					return str;
				
				}
		
		
		};
		
		
		class RegexDigit {
		
		
			public:
			
			
				static bool Check (CodePoint cp, const Locale & locale) noexcept {
				
					auto cpi=cp.GetInfo(locale);
					if (cpi==nullptr) return false;
					
					return cpi->GeneralCategory==GeneralCategory::Nd;
				
				}
				
				
				static String ToString (bool inverted) {
				
					String str("Any ");
					if (inverted) str << "non-";
					str << "digit";
					
					return str;
				
				}
		
		
		};
		
		
		class RegexWord {
		
		
			public:
			
			
				static bool Check (CodePoint cp, const Locale & locale) noexcept {
				
					auto cpi=cp.GetInfo(locale);
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
				
				
				static String ToString (bool inverted) {
				
					String str("Any ");
					if (inverted) str << "non-";
					str << "word code point";
					
					return str;
				
				}
		
		
		};
		
		
		template <typename T>
		class RegexCharacterClasses : public RegexPatternElement {
		
		
			private:
		
		
				bool inverted;
			
			
			public:
			
			
				RegexCharacterClasses (bool inverted, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					if (!engine || (T::Check(*engine,Locale)==inverted)) return false;
					
					++engine;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent=T::ToString(inverted);
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexCharacterClassesParser : public RegexParser {
		
		
			private:
			
			
				template <typename T>
				static void create (bool inverted, RegexCompiler & compiler) {
				
					compiler.Add<RegexCharacterClasses<T>>(inverted);
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!((*compiler=='\\') && ++compiler)) return false;
					
					bool inverted=false;
					auto cp=*compiler;
					++compiler;
					switch (cp) {
					
						case 'W':
							inverted=true;
						case 'w':
							create<RegexWord>(inverted,compiler);
							break;
						case 'D':
							inverted=true;
						case 'd':
							create<RegexDigit>(inverted,compiler);
							break;
						case 'S':
							inverted=true;
						case 's':
							create<RegexWhiteSpace>(inverted,compiler);
							break;
						default:
							return false;
					
					}
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCharacterClassesParser> installer;


}
