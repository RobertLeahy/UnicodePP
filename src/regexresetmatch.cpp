#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	namespace {
	
	
		class RegexResetMatchState : public RegexPrivateState {
		
		
			private:
			
			
				const CodePoint * begin;
		
		
			public:
			
			
				RegexResetMatchState (const RegexEngine & engine) noexcept : begin(engine.Match.begin()) {
				
					engine.Match.begin(engine.begin().Base());
				
				}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					engine.Match.begin(begin);
				
				}
		
		
		};
		
		
		class RegexResetMatch : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					state.Imbue<RegexResetMatchState>(engine);
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="The match is reset to this location";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexResetMatchParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("\\K")) return false;
					
					compiler.Add<RegexResetMatch>();
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexResetMatchParser> installer;


}
