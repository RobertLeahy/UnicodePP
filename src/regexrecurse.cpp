#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexRecurseState : public RegexPrivateState {
		
		
			public:
			
			
				RegexEngine Engine;
				
				
				RegexRecurseState (const RegexEngine & engine) : Engine(engine,engine.Pattern) {	}
				
				
				virtual void Rewind (RegexEngine &) override {
				
					Engine.Rewind();
				
				}
		
		
		};
	
	
		class RegexRecurse : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					typedef RegexRecurseState type;
				
					auto & s=state ? state.Get<type>() : state.Imbue<type>(engine);
					
					if (s.Engine()) {
					
						s.Engine.Set(engine);
						s.Engine.Set(state);
						
						return true;
					
					}
					
					state.Clear();
					
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="The entire pattern, recursively";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexRecurseParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("(?R)")) return false;
					
					compiler.Add<RegexRecurse>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexRecurseParser> installer(false);


}
