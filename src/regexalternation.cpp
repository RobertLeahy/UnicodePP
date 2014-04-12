#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>
#include <vector>


namespace Unicode {


	namespace {
	
	
		typedef std::vector<RegexCompiler::Pattern> patterns_type;
	
	
		class RegexAlternationState : public RegexPrivateState {
		
		
			public:
			
			
				typedef patterns_type::const_iterator iterator;
			
			
				iterator Begin;
				iterator End;
				RegexEngine Engine;
				
				
				RegexAlternationState (RegexEngine & engine, iterator begin, iterator end)
					:	Begin(begin),
						End(end),
						Engine(engine,*begin)
				{	}
				
				
				explicit operator bool () const noexcept {
				
					return Engine.CanBacktrack || (Begin!=(End-1));
				
				}
				
				
				virtual void Rewind (RegexEngine &) override {
				
					if (!*this) Engine.Rewind();
				
				}
		
		
		};
		
		
		class RegexAlternation : public RegexPatternElement {
		
		
			private:
			
			
				typedef RegexAlternationState type;
			
			
				patterns_type patterns;
				
				
				static bool match (RegexEngine & engine, RegexState & state) {
				
					auto & s=state.Get<type>();
					
					if (s.Engine()) {
					
						state.CanBacktrack=!!s;
						
						s.Engine.Set(engine);
						
						return true;
					
					}
					
					state.CanBacktrack=false;
					
					return false;
				
				}
		
		
			public:
			
			
				RegexAlternation (patterns_type patterns, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						patterns(std::move(patterns))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					if (!state) state.Imbue<type>(engine,patterns.begin(),patterns.end());
				
					for (;;) {
					
						if (match(engine,state)) return true;
						
						auto & s=state.Get<type>();
						if (s) state.Imbue<type>(engine,s.Begin+1,patterns.end());
						else break;
					
					}
					
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="One of";
					bool first=true;
					for (auto & pattern : patterns) {
					
						RegexToString rts;
					
						if (first) {
						
							first=false;
							
							rts.Parent="The following pattern";
						
						} else {
						
							rts.Parent="Or the following pattern";
						
						}
						
						for (auto & element : pattern) rts.Children.push_back(element->ToString());
						
						retr.Children.push_back(std::move(rts));
					
					}
				
					return retr;
				
				}
		
		
		};
		
		
		class RegexAlternationCompiler : public RegexCompiler {
		
		
			private:
			
			
				RegexCompiler & inner;
		
		
			public:
			
			
				bool Again;
			
			
				RegexAlternationCompiler (RegexCompiler & compiler) noexcept
					:	RegexCompiler(compiler),
						inner(compiler),
						Again(false)
				{	}
				
				
				virtual bool Done () override {
				
					if (IsNext('|')) {
					
						Again=true;
						
						return true;
					
					}
				
					inner.Current=Current;
					auto result=inner.Done();
					inner.Current=Current;
					
					return result;
				
				}
		
		
		};
		
		
		class RegexAlternationParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext('|')) return false;
					
					patterns_type patterns;
					patterns.push_back(compiler.Get());
					
					for (RegexAlternationCompiler c(compiler);;c.Again=false) {
					
						c();
						patterns.push_back(c.Get());
						
						if (!c.Again) break;
					
					}
					
					compiler.Add<RegexAlternation>(std::move(patterns));
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexAlternationParser> installer(false);


}
