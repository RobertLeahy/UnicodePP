#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <type_traits>
#include <utility>


namespace Unicode {


	static String format (std::size_t i) {
	
		return i;
	
	}
	
	
	static String format (const String & s) {
	
		String retr;
		retr << "\"" << s << "\"";
		
		return retr;
	
	}


	namespace {
	
	
		template <typename T>
		class RegexRecurseGroup : public RegexPatternElement {
		
		
			private:
			
			
				T id;
				const RegexPatternElement * ptr;
				
				
			public:
			
			
				RegexRecurseGroup (RegexCompiler & compiler, T id, RegexOptions options, const Unicode::Locale & locale) noexcept(
					std::is_nothrow_move_constructible<T>::value
				)	:	RegexPatternElement(options,locale),
						id(std::move(id))
				{
				
					compiler.GetCapturingGroup(this->id,ptr);
				
				}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					return (*ptr)(engine,state);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Group " << format(id) << " recursively";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexRecurseGroupParser : public RegexParser {
		
		
			private:
				
				
				static String get (RegexCompiler & compiler) {
				
					String s;
					for (;compiler && (*compiler!=')');++compiler) s << *compiler;
					
					if (!compiler) compiler.Raise("Unterminated recursion specification");
					
					if (s.Size()==0) compiler.Raise("Empty group specification");
					
					++compiler;
					
					return s;
				
				}
				
				
				static bool make_numbered (RegexCompiler & compiler, const String & s) {
				
					auto number=s.To<std::size_t>();
					if (!number) return false;
					
					compiler.Add<RegexRecurseGroup<std::size_t>>(compiler,*number);
					
					return true;
				
				}
			
			
				static bool get_numbered (RegexCompiler & compiler) {
				
					if (!make_numbered(compiler,get(compiler))) return false;
					
					return true;
				
				}
				
				
				static bool get_named (RegexCompiler & compiler) {
				
					auto s=get(compiler);
					
					if (make_numbered(compiler,s)) return true;
					
					compiler.Add<RegexRecurseGroup<String>>(compiler,std::move(s));
					
					return true;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("(?")) return false;
					
					if (
						compiler.IsNext('&') ||
						compiler.IsNext("P>")
					) return get_named(compiler);
					
					return get_numbered(compiler);
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexRecurseGroupParser> installer;


}
