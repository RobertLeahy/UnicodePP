#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {


	//	Throws an exception appropriate for an unterminated
	//	character class
	[[noreturn]]
	static void raise (RegexCompiler & compiler) {
	
		compiler.Raise("No end of character class");
	
	}


	namespace {
	
	
		class RegexCharacterClassPrivateState : public RegexPrivateState {
		
		
			private:
			
			
				typedef RegexCompiler::Pattern::const_iterator type;
		
		
			public:
			
			
				type Location;
				
				
				RegexCharacterClassPrivateState (type loc) noexcept : Location(loc) {	}
		
		
		};
	
	
		class RegexCharacterClass : public RegexPatternElement {
		
		
			private:
			
			
				typedef RegexCharacterClassPrivateState state_type;
			
			
				RegexCompiler::Pattern elements;
				RegexCompiler::Pattern subtracted;
				bool inverted;
				
				
				bool is_subtracted (RegexEngine & engine, RegexState & state) const {
				
					//	If this pattern element has already been
					//	invoked at this point in the string, we
					//	know that nothing subtracted matches, otherwise
					//	there would not have been a match here in the
					//	first place
					if (state) return false;
				
					for (auto & element : subtracted) {
					
						RegexState s(engine.begin());
						auto result=(*element)(engine,s);
						
						s.Rewind(engine);
						
						if (result) return true;
					
					}
					
					return false;
				
				}
				
				
				bool is_match (RegexEngine & engine, RegexState & state) const {
				
					if (state) ++state.Get<state_type>().Location;
					auto & s=state ? state.Get<state_type>() : state.Imbue<state_type>(elements.begin());
					
					for (auto end=elements.end();s.Location!=end;++s.Location) {
					
						RegexState st(engine.begin());
						if ((*(s.Location->get()))(engine,st)) return true;
						
						st.Rewind(engine);
					
					}
					
					return false;
				
				}
				
				
				bool evaluate (RegexEngine & engine, RegexState & state) const {
				
					if (is_subtracted(engine,state)) return false;
					
					if (is_match(engine,state)) {
					
						state.CanBacktrack=state.Get<state_type>().Location!=elements.end()-1;
						
						return true;
					
					}
					
					state.CanBacktrack=false;
					
					return false;
				
				}
				
				
			public:
			
			
				RegexCharacterClass (
					RegexCompiler::Pattern elements,
					RegexCompiler::Pattern subtracted,
					bool inverted,
					RegexOptions options,
					const Unicode::Locale & locale
				) noexcept
					:	RegexPatternElement(options,locale),
						elements(std::move(elements)),
						subtracted(std::move(subtracted)),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					//	Handle the case where this character class
					//	is empty
					if (elements.size()==0) {
					
						++engine;
						
						return inverted;
					
					}
					
					auto result=evaluate(engine,state);
					
					if (inverted) {
					
						if (!result) ++engine;
						state.CanBacktrack=false;
						
						return !result;
						
					}
					
					return result;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					
					if (elements.size()==0) {

						if (inverted) retr.Parent << "Always matches";
						else retr.Parent << "Never matches";
					
					} else {
					
						retr.Parent << "Any ";
						if (inverted) retr.Parent << "but";
						else retr.Parent << "of";
					
					}
					
					for (auto & element : elements) retr.Children.push_back(element->ToString());
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexCharacterClassCompiler : public RegexCompiler {
		
		
			public:
			
			
				bool Subtraction;
			
			
				RegexCharacterClassCompiler (
					const CodePoint * loc,
					const CodePoint * begin,
					const CodePoint * end,
					RegexOptions options,
					const Unicode::Locale & locale
				) noexcept
					:	RegexCompiler(
							loc,
							begin,
							end,
							options,
							locale
						),
						Subtraction(false)
				{
				
					CharacterClass=true;
				
				}
			
			
				virtual bool Done () override {
				
					if (!*this) raise(*this);
					
					auto cp=*(*this);
					
					if (!Subtraction && (cp=='-')) {
					
						Subtraction=true;
						
						return true;
					
					}
					
					return cp==']';
				
				}
		
		
		};
		
		
		class RegexCharacterClassParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					//	Verify that this is a character class
					if (*compiler!='[') return false;
					
					if (!++compiler) raise(compiler);
					
					//	Inverted?
					bool inverted;
					if ((inverted=*compiler=='^')) if (!++compiler) raise(compiler);
					
					//	Compile
					
					RegexCharacterClassCompiler c(
						compiler.Current,
						compiler.Begin,
						compiler.End,
						compiler.Options,
						compiler.Locale
					);
					c.CharacterClass=true;
					
					c();
					auto elements=c.Get();
					
					++c;
					
					RegexCompiler::Pattern subtracted;
					if (c.Subtraction) {
					
						c();
						subtracted=c.Get();
						
						++c;
						
					}
					
					//	Advance this compiler's compiler
					compiler.Current=c.Current;
					
					compiler.Add<RegexCharacterClass>(
						std::move(elements),
						std::move(subtracted),
						inverted
					);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCharacterClassParser> installer(false);


}
