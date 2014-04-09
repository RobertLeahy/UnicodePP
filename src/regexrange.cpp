#include <unicode/caseconverter.hpp>
#include <unicode/converter.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexRange : public RegexPatternElement {
		
		
			private:
			
			
				CodePoint a;
				CodePoint b;
				
				
				static CodePoint fold (CodePoint cp, const Unicode::Locale & locale) noexcept {
				
					return CaseConverter(locale,false).Fold(&cp,&cp,&cp+1)[0];
				
				}
				
				
				static CodePoint get (CodePoint cp, RegexOptions options, const Unicode::Locale & locale) noexcept {
				
					return Unicode::Check(options,RegexOptions::IgnoreCase) ? fold(cp,locale) : cp;
				
				}
				
				
				static String format (CodePoint cp) {
				
					String retr("U+");
					String str(Converter<CodePoint::Type>{}(cp));
					str.ToUpper();
					for (auto i=str.Size();i<4;++i) retr << "0";
					retr << str;
					
					return retr;
				
				}
				
				
			public:
			
			
				RegexRange (CodePoint a, CodePoint b, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						a(get(a,options,locale)),
						b(get(b,options,locale))
				{
				
					if (this->a>this->b) std::swap(this->a,this->b);
				
				}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					if (!engine) return false;
					
					auto cp=*engine;
					
					if (Check(RegexOptions::IgnoreCase)) cp=fold(cp,Locale);
					
					if ((cp<a) || (cp>b)) return false;
					
					++engine;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Any code point ";
					if (Check(RegexOptions::IgnoreCase)) retr.Parent << "whose simple case folding is ";
					retr.Parent << "between " << format(a) << " and " << format(b) << " (inclusive)";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexRangeParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					auto first=*compiler;
					
					if (!(++compiler && (*compiler=='-') && ++compiler)) return false;
					
					auto second=*compiler;
					
					if (first==second) compiler.Raise("Range has both endpoints equal");
					if (first>second) compiler.Raise("Range has highest code point first");
					
					compiler.Add<RegexRange>(first,second);
					
					++compiler;
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexRangeParser> installer(true);


}
