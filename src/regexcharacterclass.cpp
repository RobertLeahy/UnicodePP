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
	
	
		class RegexCharacterClassBase : public RegexPatternElement {
		
		
			protected:
			
			
				RegexCompiler::Pattern elements;
				RegexCompiler::Element subtraction;
				
				
				static RegexToString to_string (String str) {
				
					RegexToString retr;
					retr.Parent=std::move(str);
					
					return retr;
				
				}
				
				
				RegexToString pattern_to_string (String str) const {
				
					auto retr=to_string(std::move(str));
					for (auto & element : elements) retr.Children.push_back(element->ToString());
					
					return retr;
				
				}
				
				
				RegexToString subtraction_to_string (String str) const {
				
					auto retr=to_string(std::move(str));
					retr.Children.push_back(subtraction->ToString());
					
					return retr;
				
				}
				
				
			public:
			
			
				RegexCharacterClassBase (
					RegexCompiler::Pattern elements,
					RegexCompiler::Element subtraction,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexPatternElement(options,locale),
						elements(std::move(elements)),
						subtraction(std::move(subtraction))
				{	}
		
		
		};
		
		
		class RegexCharacterClassStateBase : public RegexPrivateState {
		
		
			public:
			
			
				RegexState State;
				
				
				RegexCharacterClassStateBase (RegexState state) noexcept : State(std::move(state)) {	}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					State.Rewind(engine);
				
				}
		
		
		};
		
		
		class RegexCharacterClassState : public RegexCharacterClassStateBase {
		
		
			private:
			
			
				typedef RegexCompiler::Pattern::const_iterator type;
		
		
			public:
			
			
				type Location;
				
				
				RegexCharacterClassState (RegexState state, type loc) noexcept
					:	RegexCharacterClassStateBase(std::move(state)),
						Location(std::move(loc))
				{	}
		
		
		};
		
		
		class RegexCharacterClass : public RegexCharacterClassBase {
		
		
			private:
			
			
				typedef RegexCharacterClassState type;
			
			
				RegexCompiler::Pattern::const_iterator get_begin (RegexState & state) const noexcept {
				
					return state ? state.Get<type>().Location : elements.begin();
				
				}
		
		
			public:
			
			
				using RegexCharacterClassBase::RegexCharacterClassBase;
			
			
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					//	Check the subtraction -- if it matches,
					//	there's no use checking anything else
					//
					//	If this pattern element has already executed
					//	before, and left a private state behind,
					//	the subtraction must not match, otherwise
					//	the first attempt would not have succeeded
					if (!state && subtraction) {
					
						RegexState s(engine);
						auto result=(*subtraction)(engine,s);
						s.Rewind(engine);
						
						if (result) return false;
					
					}
					
					//	Loop over each element and check to see if
					//	any of them match
					for (auto begin=get_begin(state),end=elements.end();begin!=end;++begin) {
					
						RegexState s(engine);
						if ((*(*begin))(engine,s)) {
						
							state.CanBacktrack=begin+1!=end;
							state.Imbue<type>(
								std::move(s),
								std::move(begin)
							);
							
							return true;
						
						}
						
						s.Rewind(engine);
					
					}
					
					//	Nothing matched, which means this is a failure
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
					
					if (subtraction) {
					
						RegexToString retr;
					
						retr.Parent << "The set";
						retr.Children.push_back(pattern_to_string("Any of"));
						retr.Children.push_back(subtraction_to_string("Except"));
						
						return retr;
					
					}
					
					return pattern_to_string("Any of");
				
				}
		
		
		};
		
		
		class RegexInvertedCharacterClass : public RegexCharacterClassBase {
		
		
			private:
			
			
				typedef RegexCharacterClassStateBase type;
				
				
			public:
			
			
				using RegexCharacterClassBase::RegexCharacterClassBase;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					//	If a match has previously been formed, i.e.
					//	there is a private state, then none of the
					//	pattern elements can have matched, otherwise
					//	that first match would not have succeeded,
					//	therefore we can skip the process of matching
					//	them
					if (!state) {
					
						for (auto & element : elements) {
						
							RegexState s(engine);
							auto result=(*element)(engine,s);
							s.Rewind(engine);
							
							if (result) return false;
						
						}
					
					}
					
					//	If there's no subtraction, we match automatically,
					//	consuming exactly one code point
					if (!subtraction) {
					
						++engine;
						
						return true;
					
					}
					
					//	We only match if the subtraction matches, due
					//	to the inverted nature of the matching
					
					if (state) {
					
						auto & s=state.Get<type>().State;
					
						if ((*subtraction)(engine,s)) {
						
							state.CanBacktrack=s.CanBacktrack;
							
							return true;
						
						}
						
						return false;
					
					}
					
					RegexState s(engine);
					if ((*subtraction)(engine,s)) {
					
						state.CanBacktrack=s.CanBacktrack;
						state.Imbue<type>(std::move(s));
						
						return true;
					
					}
					
					s.Rewind(engine);
					
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					if (subtraction) {
					
						RegexToString retr;
					
						retr.Parent << "The set";
						retr.Children.push_back(subtraction->ToString());
						retr.Children.push_back(pattern_to_string("Except"));
						
						return retr;
					
					}
					
					return pattern_to_string("Any code point except");
				
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
				
					auto & self=*this;
				
					//	If we're at the end, it's an unterminated
					//	set, so throw
					if (!self) raise(self);
					
					//	Check for a subtraction
					if ((Size()!=0) && (*self=='-')) {
					
						if (++self && (*self=='[')) {
						
							if (++self) {
							
								Subtraction=true;
								
								return true;
							
							}
							
							--self;
						
						}
						
						--self;
					
					}
					
					//	Check for end of set
					if (*self==']') {
					
						++self;
						
						return true;
					
					}
					
					return false;
				
				}
		
		
		};
		
		
		class RegexCharacterClassParser : public RegexParser {
		
		
			private:
			
			
				static RegexCompiler::Element get_subtraction (RegexCompiler &);
			
			
				static RegexCompiler::Element compile (RegexCompiler & compiler) {
				
					//	Inverted?
					bool inverted=*compiler=='^';
					if (inverted && !++compiler) raise(compiler);
					
					//	Create a compiler
					RegexCharacterClassCompiler c(
						compiler.Current,
						compiler.Begin,
						compiler.End,
						compiler.Options,
						compiler.Locale
					);
					
					//	Compile
					c();
					if (c.Size()==0) c.Raise("Empty character class");
					auto elements=c.Get();
					
					//	If necessary, compile the subtraction
					RegexCompiler::Element subtraction;
					if (c.Subtraction) subtraction=get_subtraction(c);
					
					//	Advance the outer compiler
					compiler.Current=c.Current;
					
					return inverted ? compiler.Create<RegexInvertedCharacterClass>(
						std::move(elements),
						std::move(subtraction)
					) : compiler.Create<RegexCharacterClass>(
						std::move(elements),
						std::move(subtraction)
					);
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					//	Verify that this is a character class
					if (*compiler!='[') return false;
					
					//	Verify that the opening square bracket
					//	is not the last character in the string
					if (!++compiler) raise(compiler);
					
					//	Compile
					compiler.Add(compile(compiler));
					
					return true;
				
				}
		
		
		};
		
		
		RegexCompiler::Element RegexCharacterClassParser::get_subtraction (RegexCompiler & compiler) {
		
			auto retr=compile(compiler);
			
			if (!(compiler && (*compiler==']'))) raise(compiler);
			
			++compiler;
			
			return retr;
		
		}
	
	
	}
	
	
	static const RegexParserInstaller<RegexCharacterClassParser> installer(false);


}
