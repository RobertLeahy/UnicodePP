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
	
	
		class RegexCharacterClass : public RegexPatternElement {
		
		
			private:
			
			
				RegexCompiler::Pattern elements;
				RegexCompiler::Pattern subtracted;
				bool inverted;
				
				
				static bool check (const RegexCompiler::Pattern & elements, RegexState & state) {
				
					auto begin=state.begin();
					for (auto & element : elements) {
					
						RegexPatternElementState s;
						if ((*element)(state,s)) return true;
						
						state.begin(begin);
					
					}
					
					return false;
				
				}
				
				
				bool evaluate (RegexState & state) const {
				
					auto begin=state.begin();
				
					if (!check(elements,state)) {
					
						++state;
						
						return false;
					
					}
					
					auto after=state.begin();
					state.begin(begin);
					auto result=check(subtracted,state);
					state.begin(after);
					
					return !result;
				
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
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					//	Handle the case where this character class
					//	is empty
					if (elements.size()==0) {
					
						++state;
						
						return inverted;
					
					}
					
					return evaluate(state)!=inverted;
				
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
