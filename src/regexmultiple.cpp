#include <unicode/codepoint.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexMultipleState : public RegexPrivateState {
		
		
			private:
			
			
				std::vector<RegexState> states;
				bool first;
				//	Set if the last pattern element matched was
				//	zero width
				bool zero_width;
				
				
				bool backtrack (RegexEngine & engine, std::size_t min) {
				
					while (states.size()!=0) {
					
						auto iter=states.end()-1;
						
						//	If the pattern element prevented backtracking
						//	when it formed this match, backtracking fails
						//	immediately
						if (iter->PreventsBacktracking) return false;
						
						//	If this pattern element can possibly form a
						//	different match, return at this point
						if (iter->CanBacktrack) return true;
						
						//	Otherwise clean up and keep moving backwards
						
						iter->Rewind(engine);
						
						states.erase(iter);
						
						//	Simply backing off is an acceptable way to
						//	form a different match, provided we've formed
						//	the minimal number of matches
						if (states.size()>=min) return false;
					
					}
					
					//	If we hit the bottom, we rewound every
					//	state and none of them could form a
					//	different match -- backtracking failed
					return false;
				
				}
				
				
				void set (RegexState & state) const noexcept {
				
					for (auto begin=states.rbegin(),end=states.rend();begin!=end;++begin) {
					
						if (begin->PreventsBacktracking) {
						
							state.PreventsBacktracking=true;
							
							break;
						
						}
						
						if (begin->CanBacktrack) {
						
							state.CanBacktrack=true;
							
							break;
						
						}
					
					}
				
				}
				
				
				void set (RegexState & state, std::size_t min, bool possessive) const noexcept {
				
					state.Clear();
					
					if (possessive) {
					
						state.PreventsBacktracking=true;
						
						return;
					
					}
					
					if (states.size()>min) {
					
						auto & back=states.back();
						
						if (back.PreventsBacktracking) {
						
							state.PreventsBacktracking=true;
							
							return;
						
						}
						
						state.CanBacktrack=true;
						
						return;
					
					}
					
					set(state);
				
				}
				
				
				bool execute (
					RegexEngine & engine,
					RegexState & state,
					const RegexPatternElement & element,
					std::size_t min,
					std::optional<std::size_t> max,
					bool possessive
				) {
				
					//	Backtrack unless this the first time
					bool backtracked=!first;
					if (backtracked) {
					
						auto prev=states.size();
						if (!backtrack(engine,min)) {
						
							//	If backtracking fails, we can only succeed if:
							//
							//	-	Backtracking actually changes the match
							//		(i.e. removes at least one state)
							//	-	We still have a sufficient number of matches
							auto curr=states.size();
							return (curr!=prev) && (curr>=min);
						
						}
					
					}
					
					//	Loop until:
					//
					//	-	We get the maximum allowed number of matches
					//		(if there is such a maximum)
					//	-	We fail at matching
					while (backtracked || !max || (states.size()!=*max)) {
					
						//	Create a fresh state for this match, unless
						//	we just backtracked, in which case we need to
						//	re-run the last state
						if (backtracked) backtracked=false;
						else states.emplace_back(engine);
						
						auto & back=states.back();
						//	Record where the match began to detect zero
						//	width matches
						auto begin=back.Begin();
						if (element(engine,back)) {
						
							zero_width=begin==engine.begin();
							
							if (zero_width && (states.size()>=min)) {
							
								return true;
							
							}
						
						} else {
						
							//	Match failed
							
							//	If we have the minimum number of matches,
							//	we can simply succeed with that
							if ((states.size()-1)>=min) {
							
								//	Rewind, erase, and succeed
								back.Rewind(engine);
								states.erase(states.end()-1);
								return true;
							
							}
							
							//	Insufficient matches, attempt backtracking
							//
							//	If that fails, fail
							if (!backtrack(engine,min)) return false;
						
						}
					
					}
					
					return states.size()>=min;
				
				}
		
		
			public:
			
			
				RegexMultipleState () : first(true), zero_width(false) {	}
				

				virtual void Rewind (RegexEngine & engine) override {
				
					while (states.size()!=0) {
					
						auto iter=states.end()-1;
						
						iter->Rewind(engine);
						
						states.erase(iter);
					
					}
					
					first=true;
				
				}
				
				
				bool operator () (
					RegexEngine & engine,
					RegexState & state,
					const RegexCompiler::Element & element,
					std::size_t min,
					std::optional<std::size_t> max,
					bool possessive
				) {
				
					auto retr=execute(engine,state,*element,min,max,possessive);
					first=false;
					
					if (retr) set(state,min,possessive);
					else state.Clear();
					
					return retr;
				
				}
				
				
				bool ZeroWidth () const noexcept {
				
					return zero_width;
				
				}
		
		
		};
		
		
		class RegexMultiple : public RegexPatternElement {
		
		
			protected:
			
			
				RegexCompiler::Element element;
				std::size_t min;
				std::optional<std::size_t> max;
				bool possessive;
				
				
			public:
			
			
				RegexMultiple (
					RegexCompiler::Element element,
					std::size_t min,
					std::optional<std::size_t> max,
					bool possessive,
					RegexOptions options,
					const Unicode::Locale & locale
				) noexcept
					:	RegexPatternElement(options,locale),
						element(std::move(element)),
						min(min),
						max(max),
						possessive(possessive)
				{	}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Children.push_back(element->ToString());
					
					if (max) {
					
						if (min==*max) {
						
							retr.Parent << "Exactly " << min;
							
						} else {
						
							if (min==0) retr.Parent="Up to";
							else retr.Parent << "Between " << min << " and";
							
							retr.Parent << " " << *max;
						
						}
						
					} else if (min==0) {
					
						retr.Parent << "Any number of";
					
					} else {
					
						retr.Parent << "At least " << min;
					
					}
					
					retr.Parent << " times";
					
					if (possessive) retr.Parent << " (possessive)";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexGreedyMultiple : public RegexMultiple {
		
		
			private:
			
			
				typedef RegexMultipleState type;
			
			
			public:
			
			
				using RegexMultiple::RegexMultiple;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					return (state ? state.Get<type>() : state.Imbue<type>())(engine,state,element,min,max,possessive);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexMultiple::ToString();
					if (!(max && (min==*max))) retr.Parent << " (greedy)";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexLazyMultipleState : public RegexMultipleState {
		
		
			public:
			
			
				std::size_t Current;
				bool RewindRequired;
				bool ZeroWidth;
				
				
				RegexLazyMultipleState (std::size_t curr) : Current(curr), RewindRequired(false), ZeroWidth(true) {	}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					RewindRequired=false;
					ZeroWidth=true;
					
					RegexMultipleState::Rewind(engine);
				
				}
		
		
		};
		
		
		class RegexLazyMultiple : public RegexMultiple {
		
		
			private:
			
			
				typedef RegexLazyMultipleState type;
				
				
				bool is_done (RegexEngine & engine, RegexState & state) const {
				
					state.Rewind(engine);
					
					auto & s=state.Get<type>();
					
					++s.Current;
					
					return max ? (s.Current<=*max) : false;
				
				}
				
				
			public:
			
			
				using RegexMultiple::RegexMultiple;
			
			
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					auto & s=state ? state.Get<type>() : state.Imbue<type>(min);
					
					bool first=true;
					
					//	If the last match ended with us preemptively increasing the
					//	target match count, we have to rewind before attempting any
					//	matches
					if (s.RewindRequired) {
					
						//	If all matches between the previous rewind and now
						//	were zero length, we're never going to make a longer
						//	match, so fail
						if (s.ZeroWidth) {
						
							state.Clear();
							
							return false;
						
						}
					
						state.Rewind(engine);
						
						first=false;
						
					}
					
					for (;;first=false) {
					
						//	Attempt to match
						if (s(engine,state,element,s.Current,s.Current,possessive)) {
						
							//	SUCCESS
							
							//	Maintain zero width flag
							if (!s.RegexMultipleState::ZeroWidth()) s.ZeroWidth=false;
						
							//	If the match formed prevents backtracking, suceed
							//	at once without further processing
							if (state.PreventsBacktracking) return true;
							
							//	If the match formed cannot match a different substring,
							//	we need to decide whether this pattern element can match
							//	a different substring.
							//
							//	If we can simply expand the number of matches we'll allow
							//	to be performed, then we can backtrack
							if (!state.CanBacktrack) {
							
								++s.Current;
								s.RewindRequired=true;
								
								state.CanBacktrack=!max || (s.Current<*max);
							
							}
							
							return true;
						
						}
						
						//	If we failed on a fresh match, we fail at once, it isn't
						//	possible to match n+1 times without the first n matches
						if (!first || is_done(engine,state)) break;
					
					}
					
					state.Clear();
					return false;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexMultiple::ToString();
					retr.Parent << " (lazy)";
					
					return retr;
				
				}
		
		
		};
	
	
		class RegexMultipleParser : public RegexParser {
		
		
			private:
			
			
				static RegexCompiler::Element pop (RegexCompiler & compiler) {
				
					if (compiler.Size()==0) compiler.Raise("No preceding pattern element");
					
					return compiler.Pop();
				
				}
		
		
			protected:
			
			
				class Result {
				
				
					public:
				
				
						std::size_t Minimum;
						std::optional<std::size_t> Maximum;
						bool Greedy;
						bool Possessive;
						
						
						Result () noexcept
							:	Minimum(0),
								Greedy(true),
								Possessive(false)
						{	}
				
				
				};
				
				
				static void add (RegexCompiler & compiler, const Result & result) {
				
					if (result.Maximum && (result.Minimum==*result.Maximum)) {
					
						if (result.Minimum==0) {
						
							pop(compiler);
							
							return;
						
						}
						
						goto greedy;
					
					}
					
					if (result.Greedy) {
					
						greedy:
						
						compiler.Add<RegexGreedyMultiple>(
							pop(compiler),
							result.Minimum,
							result.Maximum,
							result.Possessive
						);
						
						return;
					
					}
					
					compiler.Add<RegexLazyMultiple>(
						pop(compiler),
						result.Minimum,
						result.Maximum,
						result.Possessive
					);
				
				}
				
				
				static void get_flags (RegexCompiler & compiler, Result & result) {
				
					if (result.Maximum && (result.Minimum==*result.Maximum)) return;
					
					if (compiler.IsNext('+')) {
					
						result.Possessive=true;
						
						return;
					
					}
					
					result.Greedy=!compiler.IsNext('?');
				
				}
		
		
		};
		
		
		class RegexMultipleQuantifierParser : public RegexMultipleParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					Result info;
					switch (*compiler) {
					
						case '*':
							break;
						case '+':
							info.Minimum=1;
							break;
						case '?':
							info.Maximum=1;
							break;
						default:
							return false;
					
					}
					
					++compiler;
					
					get_flags(compiler,info);
					
					add(compiler,info);
					
					return true;
				
				}
		
		
		};
	
	
		class RegexMultipleBracketParser : public RegexMultipleParser {
		
		
			private:
			
			
				static bool is_digit (CodePoint cp, const Locale & locale) noexcept {
				
					auto cpi=cp.GetInfo(locale);
					return (
						(cpi!=nullptr) &&
						cpi->Numeric &&
						(cpi->Numeric->Type==NumericType::Decimal)
					);
				
				}
				
				
				static std::optional<std::size_t> to_integer (const String & str) {
				
					if (str.Size()==0) return std::nullopt;
					
					return str.Convert<std::size_t>();
				
				}
				
				
				static std::optional<Result> get (const String & lo, const std::optional<String> & hi) {
				
					auto l=to_integer(lo);
					decltype(l) h;
					if (hi) h=to_integer(*hi);
					
					//	If neither lo or high is specified, this
					//	isn't a multiple specification at all
					if (!(l || h)) return std::nullopt;
				
					Result retr;
					
					if (l) retr.Minimum=*l;
					if (hi) retr.Maximum=h;
					else retr.Maximum=*l;
					
					return retr;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext('{')) return false;
					
					String lo;
					std::optional<String> hi;
					for (;;++compiler) {
					
						if (!compiler) return false;
						
						if (*compiler=='}') break;
						
						if (compiler->IsWhiteSpace()) continue;
						
						if (*compiler==',') {
						
							hi.emplace();
							
							continue;
						
						}
						
						if (!is_digit(*compiler,compiler.Locale)) return false;
						
						(hi ? *hi : lo) << *compiler;
					
					}
					
					//	Skip the closing curly
					++compiler;
					
					//	Trim
					lo.Trim();
					if (hi) hi->Trim();
					
					//	Get information about parse
					auto info=get(lo,hi);
					if (!info) return false;
					get_flags(compiler,*info);
					
					add(compiler,*info);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexMultipleQuantifierParser> installer_1(false);
	static const RegexParserInstaller<RegexMultipleBracketParser> installer_2(false);


}
