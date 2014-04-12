#include <unicode/boundaries.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>


namespace Unicode {


	static bool is_line_break (const RegexEngine & engine) noexcept {
	
		return IsLineBreak(engine.begin().Base(),engine.Begin(),engine.End());
	
	}


	namespace {
	
	
		class RegexBegin : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
			
			
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					return engine.Begin()==engine.begin().Base();
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "The beginning of the string";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexBeginLine : public RegexBegin {
		
		
			public:
			
			
				using RegexBegin::RegexBegin;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					if (RegexBegin::operator ()(engine,state)) return true;
					
					return is_line_break(engine);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexBegin::ToString();
					retr.Parent << " or line";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexEnd : public RegexPatternElement {
		
		
			public:
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					return engine.End()==engine.begin().Base();
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "The end of the string";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexEndNewline : public RegexEnd {
		
		
			public:
		
		
				using RegexEnd::RegexEnd;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					if (RegexEnd::operator ()(engine,state)) return true;
					
					if (*engine!='\n') return false;
					
					bool result=!++engine;
					
					--engine;
					
					return result;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexEnd::ToString();
					retr.Parent << " or before LINE FEED (LF) (U+000A) at the end of the string";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexEndLine : public RegexEndNewline {
		
		
			public:
			
			
				using RegexEndNewline::RegexEndNewline;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState & state) const override {
				
					if (
						RegexEndNewline::operator ()(engine,state) ||
						is_line_break(engine)
					) return true;
					
					if (*engine!='\n') return false;
					
					++engine;
					if (!is_line_break(engine)) return false;
					--engine;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					auto retr=RegexEndNewline::ToString();
					retr.Parent << " or equivalently at the end of a line";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexAnchorParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (compiler.IsNext('^')) {
					
						if (compiler.Check(RegexOptions::Multiline)) compiler.Add<RegexBeginLine>();
						else compiler.Add<RegexBegin>();
						
						return true;
					
					}
					
					if (compiler.IsNext('$')) {
					
						if (compiler.Check(RegexOptions::Multiline)) compiler.Add<RegexEndLine>();
						else compiler.Add<RegexEndNewline>();
						
						return true;
					
					}
					
					if (!(compiler.IsNext('\\') && compiler)) return false;
					
					switch (*compiler) {
					
						case 'A':
							compiler.Add<RegexBegin>();
							break;
						case 'Z':
							break;
						case 'z':
							compiler.Add<RegexEnd>();
							break;
						default:
							return false;
					
					}
					
					++compiler;
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexAnchorParser> installer(false);


}
