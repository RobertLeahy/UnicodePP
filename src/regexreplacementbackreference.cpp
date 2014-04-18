#include <unicode/regex.hpp>
#include <unicode/regexreplacement.hpp>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>


namespace Unicode {


	namespace {
	
	
		template <typename T>
		class RegexReplacementBackreference : public RegexReplacementElement {
		
		
			private:
			
			
				T key;
				
				
			public:
			
			
				RegexReplacementBackreference (T key) noexcept(std::is_nothrow_move_constructible<T>::value)
					:	key(std::move(key))
				{	}
				
				
				virtual String operator () (RegexMatch & match, const CodePoint *, const CodePoint *) const override {
				
					auto captures=match[key];
					if (captures.size()==0) return String{};
					return captures[0].Get();
				
				}
		
		
		};
		
		
		class RegexReplacementBackreferenceParser : public RegexReplacementParser {
		
		
			private:
			
			
				static std::optional<String> get_bracketed (RegexReplacementCompiler & compiler) {
				
					String retr;
					for (;compiler;++compiler) {
					
						if (*compiler=='}') {
						
							++compiler;
							
							return retr;
						
						}
						
						retr << *compiler;
					
					}
					
					return std::nullopt;
				
				}
				
				
				static std::optional<String> get_numeric (RegexReplacementCompiler & compiler) {
				
					String retr;
					for (bool first=true;compiler;++compiler,first=false) {
					
						auto cpi=compiler->GetInfo();
						if (
							(cpi==nullptr) ||
							!cpi->Numeric ||
							(cpi->Numeric->Type!=NumericType::Decimal) ||
							(first && (cpi->Numeric->Value==0))
						) break;
						
						retr << *compiler;
					
					}
					
					if (retr.Size()==0) return std::nullopt;
					
					return retr;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexReplacementCompiler & compiler) const override {
				
					if (!compiler.IsNext('$') && compiler) return false;
					
					auto name=compiler.IsNext('{') ? get_bracketed(compiler) : get_numeric(compiler);
					if (!name) return false;
					
					auto number=name->To<std::size_t>();
					if (number) compiler.Add<RegexReplacementBackreference<std::size_t>>(*number);
					else compiler.Add<RegexReplacementBackreference<String>>(std::move(*name));
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexReplacementParserInstaller<RegexReplacementBackreferenceParser> installer;


}
