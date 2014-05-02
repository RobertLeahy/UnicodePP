#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/string.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>


namespace Unicode {


	[[noreturn]]
	static void raise (const RegexCompiler & compiler) {
	
		compiler.Raise("Unterminated grouping construct");
	
	}
	
	
	template <typename T>
	String format (T && str) {
	
		return String(std::forward<T>(str));
	
	}
	
	
	String format (const String & str) {
	
		String retr;
		retr << "\"" << str << "\"";
		
		return retr;
	
	}


	namespace {
	
	
		class RegexGroupState : public RegexPrivateState {
		
		
			public:
			
			
				RegexEngine Engine;
				
				
				RegexGroupState (const RegexEngine & engine, const RegexCompiler::Elements & pattern) : Engine(engine,pattern) {	}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					Engine.Rewind();
				
				}
		
		
		};
		
		
		class RegexGroup : public RegexPatternElement {
		
		
			private:
			
			
				typedef RegexGroupState type;
				
				
			protected:
			
			
				RegexCompiler::Elements pattern;
				
				
			public:
			
			
				RegexGroup (RegexOptions options, const Unicode::Locale & locale) : RegexPatternElement(options,locale) {	}
			
			
				RegexGroup (RegexCompiler::Elements pattern, RegexOptions options, const Unicode::Locale & locale)
					:	RegexPatternElement(options,locale),
						pattern(std::move(pattern))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					auto & e=(state ? state.Get<type>() : state.Imbue<type>(engine,pattern)).Engine;
					
					if (e()) {
					
						e.Set(engine);
						e.Set(state);
						
						return true;
					
					}
					
					state.Clear();
					
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent="The subpattern";
					for (auto & element : pattern) retr.Children.push_back(element->ToString());
					
					return retr;
				
				}
				
				
				void Pattern (RegexCompiler::Elements pattern) {
				
					this->pattern=std::move(pattern);
				
				}
		
		
		};
		
		
		template <typename T>
		class RegexCapturingGroupState : public RegexGroupState {
		
		
			private:
			
			
				T key;
				
				
			public:
			
			
				bool Captured;
				
				
				RegexCapturingGroupState (
					const RegexEngine & engine,
					const RegexCompiler::Elements & pattern,
					T key
				) noexcept(std::is_nothrow_move_constructible<T>::value)
					:	RegexGroupState(engine,pattern),
						key(std::move(key)),
						Captured(false)
				{	}
				
				
				void Release (RegexEngine & engine) noexcept {
				
					if (!Captured) return;
					Captured=false;
					
					auto & captures=engine.Match[key];
					captures.erase(captures.end()-1);
				
				}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					Release(engine);
				
					RegexGroupState::Rewind(engine);
				
				}
		
		
		};
		
		
		template <typename T>
		class RegexCapturingGroup : public RegexGroup {
		
		
			private:
			
			
				typedef RegexCapturingGroupState<T> type;
			
			
				T key;
				
				
			public:
			
			
				RegexCapturingGroup (
					T key,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexGroup(options,locale),
						key(std::move(key))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					auto & s=state ? state.Get<type>() : state.Imbue<type>(engine,pattern,key);
					s.Release(engine);
					
					auto begin=state.Begin().Base();
					
					if (RegexGroup::operator ()(engine,state)) {
					
						engine.Match[key].emplace_back(begin,engine.begin().Base());
						
						s.Captured=true;
						
						return true;
					
					}
					
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexGroup::ToString();
					retr.Parent << " (capturing group " << format(key) << ")";
					
					return retr;
				
				}
				
				
				const T & Key () const noexcept {
				
					return key;
				
				}
		
		
		};
		
		
		template <typename T1, typename T2>
		class RegexBalancingState : public RegexGroupState {
		
		
			private:
			
			
				T1 name1;
				T2 name2;
				RegexCapture capture;
				
				
			public:
			
			
				RegexEngine Engine;
			
			
				RegexBalancingState (
					const RegexEngine & engine,
					const RegexCompiler::Elements & pattern,
					T1 name1,
					T2 name2,
					RegexCapture capture
				)	:	RegexGroupState(engine,pattern),
						name1(std::move(name1)),
						name2(std::move(name2)),
						capture(std::move(capture)),
						Engine(engine,pattern)
				{	}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					RegexGroupState::Rewind(engine);
					
					//	Remove the capture that we formed
					auto & ours=engine.Match[name1];
					ours.erase(ours.end()-1);
					
					//	Restore the capture that we popped
					engine.Match[name2].push_back(std::move(capture));
				
				}
		
		
		};
		
		
		template <typename T1, typename T2>
		class RegexBalancing : public RegexGroup {
		
		
			private:
			
			
				typedef RegexBalancingState<T1,T2> type;
			
			
				T1 name1;
				T2 name2;
				
				
				static std::size_t distance (const CodePoint * a, const CodePoint * b) noexcept {
				
					return static_cast<std::size_t>((a<b) ? (b-a) : (a-b));
				
				}
				
				
				static RegexCapture capture (const RegexEngine & engine, const RegexCapture & capture) noexcept {
				
					auto loc=engine.begin().Base();
					auto begin=capture.begin();
					auto end=capture.end();
					return RegexCapture(
						loc,
						(distance(loc,begin)<distance(loc,end)) ? begin : end
					);
				
				}
				
				
				bool capture (RegexEngine & engine, RegexState & state) const {
				
					//	Fetch the capturing group we're balancing with,
					//	if it hasn't captured, fail
					auto & captures=engine.Match[name2];
					if (captures.size()==0) return false;
					
					//	Pop the last capture
					auto c=std::move(captures.back());
					captures.erase(captures.end()-1);
					
					//	Add our capture
					engine.Match[name1].push_back(capture(engine,c));
					
					//	Create state
					state.Imbue<type>(engine,pattern,name1,name2,std::move(c));
					
					return true;
				
				}
				
				
			public:
			
			
				RegexBalancing (
					T1 name1,
					T2 name2,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexGroup(options,locale),
						name1(std::move(name1)),
						name2(std::move(name2))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					if (!(state || capture(engine,state))) return false;
					
					return RegexGroup::operator ()(engine,state);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent	<< "The subpattern (capturing group "
								<< format(name1)
								<< ", balances against capturing group "
								<< format(name2)
								<< ")";
					for (auto & element : pattern) retr.Children.push_back(element->ToString());

					return retr;
				
				}
				
				
				const T2 & Key () const noexcept {
				
					return name2;
				
				}
		
		
		};
		
		
		class RegexGroupCompiler : public RegexCompiler {
		
		
			public:
			
			
				RegexGroupCompiler (const RegexCompiler & compiler) noexcept : RegexCompiler(compiler) {	}
				
				
				virtual bool Done () override {
				
					if (!*this) raise(*this);
					
					return IsNext(')');
				
				}
		
		
		};
	
	
		class RegexGroupParser : public RegexParser {
		
		
			private:
			
			
				class Options {
				
				
					private:
					
					
						RegexOptions enable;
						RegexOptions disable;
						
						
					public:
					
					
						Options () noexcept : enable(RegexOptions::None), disable(RegexOptions::None) {	}
						
						
						void Enable (RegexOptions option) noexcept {
						
							enable|=option;
						
						}
						
						
						void Disable (RegexOptions option) noexcept {
						
							disable|=option;
						
						}
						
						
						void Add (RegexOptions option, bool disable) noexcept {
						
							if (disable) Disable(option);
							else Enable(option);
						
						}
						
						
						void Apply (RegexOptions & options) const noexcept {
						
							options|=enable;
							options&=~disable;
						
						}
				
				
				};
				
				
				static RegexOptions get_option (CodePoint cp) noexcept {
				
					switch (cp) {
					
						case 'i':
							return RegexOptions::IgnoreCase;
						case 'm':
							return RegexOptions::Multiline;
						case 'n':
							return RegexOptions::ExplicitCapture;
						case 's':
							return RegexOptions::Singleline;
						case 'x':
							return RegexOptions::IgnorePatternWhiteSpace;
						default:
							return RegexOptions::None;
					
					}
				
				}
				
				
				static RegexOptions get_option (RegexCompiler & compiler) {
				
					if (!compiler) raise(compiler);
					
					auto retr=get_option(*compiler);
					if (retr==RegexOptions::None) compiler.Raise("Unrecognized option");
					
					++compiler;
					
					return retr;
				
				}
				
				
				static Options parse_options (RegexCompiler & compiler) {
				
					Options options;
					bool disable=false;
					for (;;) {
					
						if (!compiler) raise(compiler);
						
						if ((*compiler==':') || (*compiler==')')) return options;
						
						if (!disable && (*compiler=='-')) {
						
							disable=true;
							
							if (!++compiler) raise(compiler);
							
						}
						
						options.Add(get_option(compiler),disable);
					
					}
				
				}
				
				
				static RegexCompiler::Elements compile (RegexCompiler & compiler, Options options=Options{}) {
				
					RegexGroupCompiler c(compiler);
					options.Apply(c.Options);
					
					c();
					
					compiler.Current=c.Current;
					
					return c.Get();
				
				}
				
				
				static bool non_capturing_group (RegexCompiler & compiler, Options options=Options{}) {
				
					compiler.Add<RegexGroup>(compile(compiler,std::move(options)));
					
					return true;
				
				}
				
				
				static bool parse_non_capturing_group (RegexCompiler & compiler) {
				
					auto options=parse_options(compiler);
					++compiler;
					
					return non_capturing_group(compiler,std::move(options));
				
				}
				
				
				template <typename T>
				static void capturing_group (RegexCompiler & compiler, T && name) {
				
					auto & element=compiler.Add<RegexCapturingGroup<typename std::decay<T>::type>>(std::forward<T>(name));
					compiler.AddCapturingGroup(element.Key(),element);
					
					element.Pattern(compile(compiler));
				
				}
				
				
				static bool automatic_capturing_group (RegexCompiler & compiler) {
				
					if (Check(compiler.Options,RegexOptions::ExplicitCapture)) {
					
						compiler.Add<RegexGroup>(compile(compiler));
					
					} else {
					
						capturing_group(compiler,compiler.GetCaptureNumber());
						
					}
					
					return true;
				
				}
				
				
				static std::pair<String,bool> get_first (RegexCompiler & compiler, CodePoint terminator) {
				
					typedef std::pair<String,bool> type;
				
					String name;
					for (;;++compiler) {
					
						if (!compiler) raise(compiler);
						
						auto cp=*compiler;
						
						if (cp==terminator) return type(std::move(name),false);
						if (cp=='-') return type(std::move(name),true);
						
						name << cp;
					
					}
				
				}
				
				
				static String get_second (RegexCompiler & compiler, CodePoint terminator) {
				
					String name;
					for (;;++compiler) {
					
						if (!compiler) raise(compiler);
						
						auto cp=*compiler;
						
						if (cp==terminator) return name;
						
						name << cp;
					
					}
				
				}
				
				
				template <typename T1, typename T2>
				static void compile_balancing (RegexCompiler & compiler, RegexBalancing<T1,T2> & element) {
				
					compiler.AddCapturingGroup(element.Key(),element);
					
					element.Pattern(compile(compiler));
				
				}
				
				
				static bool balancing (RegexCompiler & compiler, String first, String second) {
				
					std::optional<std::size_t> number_first;
					if (first.Size()==0) number_first=compiler.GetCaptureNumber();
					else number_first=first.To<std::size_t>();
					auto number_second=second.To<std::size_t>();
					
					if (number_first) {
					
						if (number_second) compile_balancing(
							compiler,
							compiler.Add<RegexBalancing<std::size_t,std::size_t>>(*number_first,*number_second)
						);
						else compile_balancing(
							compiler,
							compiler.Add<RegexBalancing<std::size_t,String>>(*number_first,std::move(second))
						);
					
					} else {
					
						if (number_second) compile_balancing(
							compiler,
							compiler.Add<RegexBalancing<String,std::size_t>>(std::move(first),*number_second)
						);
						else compile_balancing(
							compiler,
							compiler.Add<RegexBalancing<String,String>>(std::move(first),std::move(second))
						);
					
					}
				
					return true;
				
				}
				
				
				static bool parse_capturing_group (RegexCompiler & compiler, CodePoint terminator) {
				
					auto pair=get_first(compiler,terminator);
					++compiler;
					
					//	Handle balancing
					if (pair.second) {
					
						auto second=get_second(compiler,terminator);
						++compiler;
						
						return balancing(
							compiler,
							std::move(pair.first),
							std::move(second)
						);
					
					}
					
					auto & name=pair.first;
					
					//	Check if name is numeric
					auto number=name.To<std::size_t>();
					
					//	Assemble pattern element
					if (number) capturing_group(compiler,*number);
					else capturing_group(compiler,std::move(name));
					
					return true;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext('(')) return false;
					
					if (!compiler.IsNext('?')) return automatic_capturing_group(compiler);
					
					if (compiler.IsNext("P<") || compiler.IsNext('<')) return parse_capturing_group(compiler,'>');
					if (compiler.IsNext('\'')) return parse_capturing_group(compiler,'\'');
					
					return parse_non_capturing_group(compiler);
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexGroupParser> installer((std::numeric_limits<std::size_t>::max()/4)*3,false);


}
