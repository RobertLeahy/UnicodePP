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
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					if (!state || (T::Check(*state,Locale)==inverted)) return false;
					
					++state;
					
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
				static RegexCompiler::Element create (const RegexCompilerState & state, bool inverted) {
				
					return RegexCompiler::Element(
						new RegexCharacterClasses<T>(
							inverted,
							state.Options,
							state.Locale
						)
					);
				
				}
		
		
			public:
			
			
				virtual RegexCompiler::Element operator () (RegexCompilerState & state) const override {
				
					if (!((*state=='\\') && ++state)) return RegexCompiler::Element{};
					
					bool inverted=false;
					auto cp=*state;
					++state;
					switch (cp) {
					
						case 'W':
							inverted=true;
						case 'w':
							return create<RegexWord>(state,inverted);
						case 'D':
							inverted=true;
						case 'd':
							return create<RegexDigit>(state,inverted);
						case 'S':
							inverted=true;
						case 's':
							return create<RegexWhiteSpace>(state,inverted);
						default:
							return RegexCompiler::Element{};
					
					}
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCharacterClassesParser> installer;


}
