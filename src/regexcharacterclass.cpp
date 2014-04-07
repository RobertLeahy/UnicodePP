#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <utility>


namespace Unicode {


	//	Throws an exception appropriate for an unterminated
	//	character class
	[[noreturn]]
	static void raise (RegexCompilerState & state) {
	
		state.Raise("No end of character class");
	
	}


	namespace {
	
	
		class RegexCharacterClass : public RegexPatternElement {
		
		
			private:
			
			
				RegexCompiler::Pattern elements;
				bool inverted;
				
				
			public:
			
			
				RegexCharacterClass (
					RegexCompiler::Pattern elements,
					bool inverted,
					RegexOptions options,
					const Unicode::Locale & locale
				) noexcept
					:	RegexPatternElement(options,locale),
						elements(std::move(elements)),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					//	Handle the case where this character class
					//	is empty
					if (elements.size()==0) {
					
						++state;
						
						return inverted;
					
					}
				
					for (auto & element : elements) {
					
						RegexPatternElementState s;
						if ((*element)(state,s)) return !inverted;
					
					}
					
					return inverted;
				
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
		
		
		class RegexCharacterClassCompilerState : public RegexCompilerState {
		
		
			public:
			
			
				RegexCharacterClassCompilerState (
					const CodePoint * loc,
					const CodePoint * begin,
					const CodePoint * end,
					RegexOptions options,
					const Unicode::Locale & locale
				) noexcept
					:	RegexCompilerState(
							loc,
							begin,
							end,
							options,
							locale
						)
				{
				
					CharacterClass=true;
				
				}
			
			
				virtual bool Done () override {
				
					if (!*this) raise(*this);
					
					return *(*this)==']';
				
				}
		
		
		};
		
		
		class RegexCharacterClassParser : public RegexParser {
		
		
			public:
			
			
				virtual RegexCompiler::Element operator () (RegexCompilerState & state) const override {
				
					//	Verify that this is a character class
					if (*state!='[') return RegexCompiler::Element{};
					
					if (!++state) raise(state);
					
					//	Inverted?
					bool inverted;
					if ((inverted=*state=='^')) if (!++state) raise(state);
					
					//	Compile
					RegexCharacterClassCompilerState c(
						state.Current,
						state.Begin,
						state.End,
						state.Options,
						state.Locale
					);
					auto retr=RegexCompiler::Element(
						new RegexCharacterClass(
							RegexCompiler{}(c),
							inverted,
							state.Options,
							state.Locale
						)
					);
					
					//	Advance this compiler's state
					++c;
					state.Current=c.Current;
					
					return retr;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexCharacterClassParser> installer(false);


}
