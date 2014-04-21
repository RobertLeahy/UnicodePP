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
					RegexCompiler::Elements pattern,
					T key,
					RegexOptions options,
					const Unicode::Locale & locale
				)	:	RegexGroup(std::move(pattern),options,locale),
						key(std::move(key))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					auto & s=state ? state.Get<type>() : state.Imbue<type>(engine,pattern,key);
					s.Release(engine);
					
					auto begin=state.Begin().Base();
					
					if (RegexGroup::operator ()(engine,state)) {
					
						auto end=engine.begin().Base();
						if (begin>end) std::swap(begin,end);
						
						engine.Match[key].emplace_back(begin,end);
						
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
				
				
				static std::optional<Options> parse_options (RegexCompiler & compiler) {
				
					auto start=compiler.Current;
				
					Options retr;
					
					bool disable=false;
					for (;compiler;++compiler) {
					
						switch (*compiler) {
						
							case '-':
								disable=true;
								break;
							case ':':
							case ')':
								return retr;
							case 'i':
								retr.Add(RegexOptions::IgnoreCase,disable);
								break;
							case 'm':
								retr.Add(RegexOptions::Multiline,disable);
								break;
							case 's':
								retr.Add(RegexOptions::Singleline,disable);
								break;
							case 'n':
								retr.Add(RegexOptions::ExplicitCapture,disable);
								break;
							case 'x':
								retr.Add(RegexOptions::IgnorePatternWhiteSpace,disable);
								break;
							default:
								compiler.Current=start;
								return std::nullopt;
						
						}
					
					}
					
					//	We ran out of string, return what we've got,
					//	the fact that this is invalid will be caught
					//	elsewhere
					return retr;
				
				}
				
				
				static RegexCompiler::Elements compile (RegexCompiler & compiler) {
				
					RegexGroupCompiler c(compiler);
					c();
					
					compiler.Current=c.Current;
					
					return c.Get();
				
				}
				
				
				template <typename T>
				static bool capturing_group (RegexCompiler & compiler, T && key) {
				
					auto & element=compiler.Add<RegexCapturingGroup<typename std::decay<T>::type>>(
						compile(compiler),
						std::forward<T>(key)
					);
					
					compiler.AddCapturingGroup(
						element.Key(),
						element
					);
					
					return true;
				
				}
				
				
				static bool non_capturing_group (RegexCompiler & compiler, const Options & options) {
				
					if (!compiler) return false;
					
					//	Whether or not this is simply changing the
					//	options
					bool opts=*compiler==')';
					++compiler;
					
					//	Simply changing the options
					if (opts) {
					
						options.Apply(compiler.Options);
						
						return true;
					
					}
					
					//	Handle a non-capturing group
					compiler.Add<RegexGroup>(compile(compiler));
					
					return true;
				
				}
				
				
				static bool automatic_capturing_group (RegexCompiler & compiler) {
				
					return compiler.Check(RegexOptions::ExplicitCapture)
						?	non_capturing_group(compiler,Options{})
						:	capturing_group(compiler,compiler.GetCaptureNumber());
				
				}
				
				
				static std::optional<String> get (RegexCompiler & compiler, CodePoint delimiter) {
				
					String str;
					for (;compiler;++compiler) {
					
						if (*compiler==')') return std::nullopt;
						if (*compiler==delimiter) return str;
						
						str << *compiler;
					
					}
					
					return std::nullopt;
				
				}
				
				
				static bool named_capturing_group (RegexCompiler & compiler, CodePoint delimiter) {
				
					auto name=get(compiler,(delimiter=='<') ? '>' : '\'');
					if (!name) {
					
						if (!compiler) raise(compiler);
						
						compiler.Raise("Unterminated group name");
					
					}
					
					//	Move past the delimiter
					++compiler;
					
					//	Check to see if this is a numbered capturing group
					auto number=name->To<std::size_t>();
					if (number) return capturing_group(compiler,*number);
					
					//	Otherwise it's unconditionally a named capturing group
					return capturing_group(compiler,std::move(*name));
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext('(')) return false;
					
					//	All grouping constructs EXCEPT automatically numbered
					//	capturing groups begin with (?, so if the ( isn't followed
					//	by a ?, take that route
					if (!compiler.IsNext('?')) return automatic_capturing_group(compiler);
					
					//	(?< means the beginning of an explicitly named capturing group
					if (compiler.IsNext('<')) return named_capturing_group(compiler,'<');
					//	(?' means the beginning of an explicitly named capturing group
					//	(PCRE syntax)
					if (compiler.IsNext('\'')) return named_capturing_group(compiler,'\'');
					//	(?P< means the beginning of an explicitly named capturing group
					//	(PCRE syntax)
					if (compiler.IsNext("P<")) return named_capturing_group(compiler,'<');
					
					//	If we succeed in extracting options flags after the ?, then
					//	it's either changing the options of this compiler, or changing
					//	the options for a non-capturing group, so take that route
					auto options=parse_options(compiler);
					if (options) return non_capturing_group(compiler,*options);
					
					return false;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexGroupParser> installer((std::numeric_limits<std::size_t>::max()/4)*3,false);


}
