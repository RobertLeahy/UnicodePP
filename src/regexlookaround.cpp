#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexLookAroundState : public RegexPrivateState {
		
		
			public:
			
			
				RegexEngine Engine;
				
				
				RegexLookAroundState (RegexEngine & engine, const RegexCompiler::Pattern & elements, bool behind)
					:	Engine(engine,elements,behind)
				{	}
				
				
				virtual void Rewind (RegexEngine &) override {
				
					Engine.Rewind();
				
				}
		
		
		};
	
	
		class RegexLookAround : public RegexPatternElement {
		
		
			private:
			
			
				RegexCompiler::Pattern elements;
				bool inverted;
				bool behind;
				
				
				String get_desc () const {
				
					String retr;
					
					if (inverted) retr << "Negative";
					else retr << "Positive";
					
					retr << " look ";
					
					if (behind) retr << "behind";
					else retr << "ahead";
					
					return retr;
				
				}
				
				
			public:
			
			
				RegexLookAround (RegexCompiler::Pattern elements, bool inverted, bool behind, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						elements(std::move(elements)),
						inverted(inverted),
						behind(behind)
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					auto & e=state.Imbue<RegexLookAroundState>(engine,elements,behind).Engine;
					auto result=e();
					
					state.PreventsBacktracking=e.PreventsBacktracking;
					
					return result;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent=get_desc();
					for (auto & element : elements) retr.Children.push_back(element->ToString());
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexLookAroundCompiler : public RegexCompiler {
		
		
			public:
			
			
				RegexLookAroundCompiler (RegexCompiler & compiler) noexcept : RegexCompiler(compiler) {	}
				
				
				virtual bool Done () override {
				
					if (!*this) Raise("Unterminated look around");
					
					return IsNext(')');
				
				}
		
		
		};
		
		
		class RegexLookAroundParser : public RegexParser {
		
		
			private:
			
			
				class Result {
				
				
					public:
					
					
						bool Inverted;
						bool Behind;
				
				
				};
				
				
				static std::optional<Result> get_type (RegexCompiler & compiler) noexcept {
				
					if (!compiler.IsNext("(?")) return std::nullopt;
					
					if (compiler.IsNext('=')) return Result{false,false};
					if (compiler.IsNext('!')) return Result{true,false};
					
					if (!compiler.IsNext('<')) return std::nullopt;
					
					if (compiler.IsNext('=')) return Result{false,true};
					if (compiler.IsNext('!')) return Result{true,true};
					
					return std::nullopt;
				
				}
				
				
				static void set_options (RegexCompiler & compiler, bool behind) noexcept {
				
					if (behind) compiler.Options|=RegexOptions::RightToLeft;
					else compiler.Options&=~RegexOptions::RightToLeft;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					auto type=get_type(compiler);
					if (!type) return false;
					
					RegexLookAroundCompiler c(compiler);
					set_options(compiler,type->Behind);
					
					c();
					
					compiler.Current=c.Current;
					
					compiler.Add<RegexLookAround>(c.Get(),type->Inverted,type->Behind);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexLookAroundParser> installer(false);


}
