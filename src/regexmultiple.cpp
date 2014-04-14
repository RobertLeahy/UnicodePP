#include <unicode/codepoint.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexMultipleState : public RegexPrivateState {
		
		
			public:
			
			
				std::vector<RegexState> States;
				
				
				bool Backtrack (RegexEngine & engine, std::optional<std::size_t> min=std::nullopt) {
				
					for (bool first=true;States.size()!=0;first=false) {
					
						auto iter=States.end()-1;
						
						//	If the pattern element prevented backtracking
						//	when it formed this match, backtracking fails
						//	immediately
						if (iter->PreventsBacktracking) return false;
						
						//	If this pattern element can possibly form a
						//	different match, return at this point
						if (iter->CanBacktrack) {
						
							//	If this is the first element we've
							//	considered, have it attempt a different
							//	match, otherwise only consider a different
							//	match if we need more matches
							return !min || first || (States.size()<*min);
							
						}
						
						//	Otherwise cleanup and delete the state to
						//	keep moving backwards
						
						iter->Rewind(engine);
						
						States.erase(iter);
						
						//	With greedy matching, we always make the
						//	longest match first, this means that simply
						//	backing off one match is an acceptable way
						//	to form a new match.
						//
						//	Accordingly if backing off on this one has
						//	yielded a number of matches that is greater
						//	than or equal to the minimum, we can just
						//	terminate right away
						if (min && (States.size()>=*min)) return false;
					
					}
					
					//	If we hit the bottom, we rewound every
					//	state and none of them could form a
					//	different match -- backtracking failed
					return false;
				
				}
				
				
				void Set (RegexState & state) const noexcept {
				
					for (auto begin=States.rbegin(),end=States.rend();begin!=end;++begin) {
					
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
				
				
				void Set (RegexState & state, std::size_t min, bool possessive) const noexcept {
				
					state.Clear();
					
					if (possessive) {
					
						state.PreventsBacktracking=true;
						
						return;
					
					}
					
					if (States.size()>min) {
					
						auto & back=States.back();
						
						if (back.PreventsBacktracking) {
						
							state.PreventsBacktracking=true;
							
							return;
						
						}
						
						state.CanBacktrack=true;
						
						return;
					
					}
					
					Set(state);
				
				}
				
				
				virtual void Rewind (RegexEngine & engine) override {
				
					while (States.size()!=0) {
					
						auto iter=States.end()-1;
						
						iter->Rewind(engine);
						
						States.erase(iter);
					
					}
				
				}
		
		
		};
		
		
		class RegexMultiple : public RegexPatternElement {
		
		
			protected:
			
			
				typedef RegexMultipleState type;
			
			
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
			
			
			public:
			
			
				using RegexMultiple::RegexMultiple;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					//	If this state has been invoked before, start by
					//	backtracking, otherwise create the state
					bool backtracked=!!state;
					if (backtracked) {
					
						auto & s=state.Get<type>();
						
						//	Backtrack
						//
						//	If backtracking couldn't contract the match, we
						//	fail.
						//
						//	If backtracking didn't find an element that could
						//	match differently, we succeed only if we still have
						//	enough matches to qualify
						auto prev=s.States.size();
						if (!s.Backtrack(engine,min)) {
						
							auto curr=s.States.size();
							s.Set(state,min,possessive);
							return (curr!=prev) && (prev>=min);
						
						}
					
					} else {
					
						state.Imbue<type>();
					
					}
					
					auto & s=state.Get<type>();
					
					//	Loop until either:
					//
					//	-	We get the maximum allowed number of
					//		matches (if there is such a maximum)
					//	-	We fail at matching
					while (!(max && (s.States.size()==*max))) {
					
						//	Create a fresh state unless we
						//	backtracked to this location
						if (backtracked) backtracked=false;
						else s.States.emplace_back(engine);
						
						//	Attempt to match
						auto & back=s.States.back();
						if (!(*element)(engine,back)) {
						
							//	Failure
							
							//	Do we have enough matches?
							if (s.States.size()>=min) {
							
								//	YES
								
								//	Rewind/erase/succeed
								back.Rewind(engine);
								s.States.erase(s.States.end()-1);
								break;
							
							}
							
							//	NO
							
							//	Backtrack
							if (!s.Backtrack(engine,min)) break;
						
						}
					
					}
					
					//	If we didn't make enough matches, we fail
					//	and there's no backtracking etc.
					if (s.States.size()<min) {
					
						state.Clear();
						
						return false;
					
					}
					
					//	Otherwise set the state information and succeed
					s.Set(state,min,possessive);
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexMultiple::ToString();
					if (!(max && (min==*max))) retr.Parent << " (greedy)";
					
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
					
					}
				
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
