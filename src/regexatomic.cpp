#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexAtomic : public RegexPatternElement {
		
		
			private:
			
			
				RegexCompiler::Pattern elements;
				
				
			public:
			
			
				RegexAtomic (RegexCompiler::Pattern elements, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						elements(std::move(elements))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					RegexEngine e(engine,elements);
					
					state.PreventsBacktracking=e();
					
					e.Set(engine);
					
					return state.PreventsBacktracking;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="Atomically";
					for (auto & element : elements) retr.Children.push_back(element->ToString());
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexAtomicCompiler : public RegexCompiler {
		
		
			public:
			
			
				RegexAtomicCompiler (const RegexCompiler & compiler) noexcept : RegexCompiler(compiler) {	}
				
				
				virtual bool Done () override {
				
					if (!*this) Raise("Unterminated atomic group");
					
					return IsNext(')');
				
				}
		
		
		};
		
		
		class RegexAtomicParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("(?>")) return false;
					
					RegexAtomicCompiler c(compiler);
					c();
					compiler.Add<RegexAtomic>(c.Get());
					compiler.Current=c.Current;
				
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexAtomicParser> installer(false);


}
