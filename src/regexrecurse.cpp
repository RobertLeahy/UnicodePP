#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <cstddef>
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
	
	
		class RegexRecurseGroup : public RegexPatternElement {
		
		
			protected:
			
			
				const RegexPatternElement * ptr;
				
				
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					return (*ptr)(engine,state);
				
				}
		
		
		};
	
	
		template <typename T>
		class RegexRecurseAbsoluteGroup : public RegexRecurseGroup {
		
		
			private:
			
			
				T id;
				
				
			public:
			
			
				RegexRecurseAbsoluteGroup (RegexCompiler & compiler, T id, RegexOptions options, const Unicode::Locale & locale)
					:	RegexRecurseGroup(options,locale),
						id(std::move(id))
				{
				
					compiler.GetCapturingGroup(this->id,ptr);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Group " << format(id) << " recursively";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexRecurseRelativeGroup : public RegexRecurseGroup {
		
		
			private:
			
			
				std::ptrdiff_t offset;
				
				
			public:
			
			
				RegexRecurseRelativeGroup (RegexCompiler & compiler, std::ptrdiff_t offset, RegexOptions options, const Unicode::Locale & locale)
					:	RegexRecurseGroup(options,locale),
						offset(offset)
				{
				
					compiler.GetRelativeCapturingGroup(this->offset,ptr);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					//	TODO: Implement
					
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
					
					compiler.Add<RegexRecurseAbsoluteGroup<std::size_t>>(compiler,*number);
					
					return true;
				
				}
			
			
				static bool get_numbered (RegexCompiler & compiler) {
				
					return make_numbered(compiler,get(compiler));
				
				}
				
				
				static bool get_relative (RegexCompiler & compiler, bool prev) {
				
					auto number=get(compiler).To<std::ptrdiff_t>();
					if (!number) return false;
					
					if (prev) *number*=-1;
					
					compiler.Add<RegexRecurseRelativeGroup>(compiler,*number);
					
					return true;
				
				}
				
				
				static bool get_named (RegexCompiler & compiler) {
				
					auto s=get(compiler);
					
					if (make_numbered(compiler,s)) return true;
					
					compiler.Add<RegexRecurseAbsoluteGroup<String>>(compiler,std::move(s));
					
					return true;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext("(?")) return false;
					
					if (
						compiler.IsNext('&') ||
						compiler.IsNext("P>")
					) return get_named(compiler);
					
					if (compiler.IsNext('-')) return get_relative(compiler,true);
					
					if (compiler.IsNext('+')) return get_relative(compiler,false);
					
					return get_numbered(compiler);
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexRecurseGroupParser> installer;


}
