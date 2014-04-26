#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <type_traits>
#include <utility>


namespace Unicode {


	[[noreturn]]
	static void raise (const RegexCompiler & compiler) {
	
		compiler.Raise("Unterminated conditional construct");
	
	}


	namespace {
	
	
		class RegexConditionalState : public RegexPrivateState {
		
		
			private:
			
			
				RegexEngine engine;
		
		
			public:
				
				
				RegexConditionalState (const RegexEngine & engine, const RegexCompiler::Elements & pattern)
					:	engine(engine,pattern)
				{	}
				
				
				virtual void Rewind (RegexEngine &) override {
				
					engine.Rewind();
				
				}
				
				
				bool operator () (RegexEngine & engine, RegexState & state) {
				
					if (this->engine()) {
					
						this->engine.Set(engine);
						this->engine.Set(state);
						
						return true;
					
					}
					
					state.Clear();
					
					return false;
				
				}
		
		
		};
		
		
		class RegexConditional : public RegexPatternElement {
		
		
			private:
			
			
				RegexCompiler::Elements yes;
				RegexCompiler::Elements no;
				
				
			protected:
			
			
				const RegexCompiler::Elements & Get (bool yes) const noexcept {
				
					return yes ? this->yes : no;
				
				}
				
				
			public:
			
			
				RegexConditional (RegexCompiler::Elements yes, RegexCompiler::Elements no, RegexOptions options, const Unicode::Locale & locale)
					:	RegexPatternElement(options,locale),
						yes(std::move(yes)),
						no(std::move(no))
				{	}
				
				
				virtual RegexToString ToString () const override {
				
					//	TODO: Implement
					return RegexToString{};
				
				}
		
		
		};
		
		
		template <typename T>
		class RegexCaptureConditional : public RegexConditional {
		
		
			private:
			
			
				T name;
				
				
			public:
			
			
				RegexCaptureConditional (
					T name,
					RegexCompiler::Elements yes,
					RegexCompiler::Elements no,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexConditional(std::move(yes),std::move(no),options,locale),
						name(std::move(name))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					typedef RegexConditionalState type;
				
					if (!state) state.Imbue<type>(
						engine,
						Get(engine.Match[name].size()!=0)
					);
					
					return state.Get<type>()(engine,state);
				
				}
		
		
		};
		
		
		class RegexPatternConditionalState : public RegexPrivateState {
		
		
			public:
			
			
				RegexEngine Engine;
				std::optional<RegexConditionalState> State;
				
				
				RegexPatternConditionalState (
					const RegexEngine & engine,
					const RegexCompiler::Elements & pattern
				)	:	Engine(engine,pattern)
				{	}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					if (State) State->Rewind(engine);
					
					Engine.Rewind();
				
				}
		
		
		};
		
		
		class RegexPatternConditional : public RegexConditional {
		
		
			private:
			
			
				RegexCompiler::Elements pattern;
				
				
			public:
			
			
				RegexPatternConditional (
					RegexCompiler::Elements pattern,
					RegexCompiler::Elements yes,
					RegexCompiler::Elements no,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexConditional(std::move(yes),std::move(no),options,locale),
						pattern(std::move(pattern))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					typedef RegexPatternConditionalState type;
				
					if (!state) {
					
						auto & s=state.Imbue<type>(engine,pattern);
						s.State.emplace(
							engine,
							Get(s.Engine())
						);
					
					}
					
					return (*state.Get<type>().State)(engine,state);
				
				}
		
		
		};
	
	
		class RegexConditionalCompiler : public RegexCompiler {
		
		
			public:
			
			
				bool ParseOptions;
				bool Alternation;
			
			
				RegexConditionalCompiler (const RegexCompiler & compiler) noexcept
					:	RegexCompiler(compiler),
						ParseOptions(false),
						Alternation(false)
				{	}
				
				
				virtual bool Done () override {
				
					auto & self=*this;
					
					if (!self) raise(self);
					
					if (ParseOptions && !Alternation && (*self=='|')) {
					
						++self;
						
						Alternation=true;
						
						return true;
					
					}
					
					if (*self==')') {
					
						++self;
						
						return true;
					
					}
					
					return false;
				
				}
		
		
		};
	
	
		class RegexConditionalParser : public RegexParser {
		
		
			private:
			
			
				typedef std::pair<RegexCompiler::Elements,RegexCompiler::Elements> result_type;
			
			
				static result_type compile (RegexConditionalCompiler & c, RegexCompiler & compiler) {
				
					c.ParseOptions=true;
					c();
					auto yes=c.Get();
					
					RegexCompiler::Elements no;
					if (c.Alternation) {
					
						c();
						no=c.Get();
					
					}
					
					compiler.Current=c.Current;
					
					return {std::move(yes),std::move(no)};
				
				}
			
			
				static result_type compile (RegexCompiler & compiler) {
				
					RegexConditionalCompiler c(compiler);
					return compile(c,compiler);
				
				}
			
			
				static String get (RegexCompiler & compiler) {
				
					String retr;
					for (;compiler && (*compiler!=')');++compiler) retr << *compiler;
					
					if (!(compiler && ++compiler)) raise(compiler);
					
					return retr;
				
				}
				
				
				template <typename T>
				static void create (RegexCompiler & compiler, T && name) {
				
					auto pair=compile(compiler);
					compiler.Add<RegexCaptureConditional<typename std::decay<T>::type>>(
						std::forward<T>(name),
						std::move(pair.first),
						std::move(pair.second)
					);
				
				}
				
				
				static bool group (RegexCompiler & compiler) {
				
					auto name=get(compiler);
					auto number=name.To<std::size_t>();
					if (number) {
					
						if (compiler.GetCapturingGroup(*number)==nullptr) return false;
						
						create(compiler,*number);
						
						return true;
					
					}
					
					if (compiler.GetCapturingGroup(name)==nullptr) return false;
					
					create(compiler,std::move(name));
					
					return true;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("(?(")) return false;
					
					auto start=compiler.Current;
					if (group(compiler)) return true;
					compiler.Current=start;
					
					RegexConditionalCompiler c(compiler);
					c();
					auto pattern=c.Get();
					auto pair=compile(c,compiler);
					
					compiler.Add<RegexPatternConditional>(
						std::move(pattern),
						std::move(pair.first),
						std::move(pair.second)
					);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexConditionalParser> installer(false);


}
