#include <unicode/caseconverter.hpp>
#include <unicode/codepoint.hpp>
#include <unicode/makereverseiterator.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <utility>


namespace Unicode {


	namespace {
	
	
		class RegexLiteral : public RegexPatternElement {
		
		
			private:
			
			
				String str;
				
				
				template <typename Iterator>
				static bool case_sensitive_check (Iterator begin, Iterator end, RegexEngine & engine) noexcept {
				
					for (;begin!=end;++begin,++engine) {
					
						if (!(engine && (*engine==*begin))) return false;
					
					}
					
					return true;
				
				}
				
				
				template <typename Iterator1, typename Iterator2>
				static bool check (Iterator1 & begin_a, Iterator1 & end_a, Iterator2 begin_b, Iterator2 end_b) noexcept {
				
					for (;(begin_a!=end_a) && (begin_b!=end_b);++begin_a,++begin_b) {
					
						if (*begin_a!=*begin_b) return false;
						
					}
					
					return begin_b==end_b;
				
				}
				
				
				template <typename Iterator>
				bool case_insensitive_check (Iterator begin, Iterator end, RegexEngine & engine) const noexcept {
				
					auto b=engine.Begin();
					auto e=engine.End();
					CaseConverter cc(Locale);
					for (;begin!=end;++engine) {
					
						if (!engine) return false;
					
						auto folded=cc.Fold(&(*engine),b,e);
						
						if (!(
							engine.Reversed() ? check(
								begin,
								end,
								MakeReverseIterator(folded.end()),
								MakeReverseIterator(folded.begin())
							) : check(
								begin,
								end,
								folded.begin(),
								folded.end()
							)
						)) return false;
					
					}
					
					return true;
				
				}
				
				
				template <typename Iterator>
				bool check (Iterator begin, Iterator end, RegexEngine & engine) const noexcept {
				
					return Check(RegexOptions::IgnoreCase) ? case_insensitive_check(
						std::move(begin),
						std::move(end),
						engine
					) : case_sensitive_check(
						std::move(begin),
						std::move(end),
						engine
					);
				
				}
		
		
			public:
			
			
				RegexLiteral (RegexOptions options, const Unicode::Locale & locale) noexcept : RegexPatternElement(options,locale) {
				
					str.SetLocale(locale);
				
				}
			
			
				using RegexPatternElement::RegexPatternElement;
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					return engine.Reversed() ? check(
						MakeReverseIterator(str.end()),
						MakeReverseIterator(str.begin()),
						engine
					) : check(
						str.begin(),
						str.end(),
						engine
					);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "The literal string: \"" << str << "\"";
					
					return retr;
				
				}
				
				
				void Add (CodePoint cp) {
				
					str << cp;
				
				}
				
				
				void Complete () {
				
					str.ToNFD();
					
					if (Check(RegexOptions::IgnoreCase)) str.ToCaseFold();
				
				}
		
		
		};
	
	
		class RegexLiteralParser : public RegexParser {
		
		
			private:
			
			
				static std::optional<CodePoint> get (const CodePoint * & begin, const CodePoint * end, std::size_t num, std::size_t base=16) {
				
					//	If there are no characters left, fail
					if (begin==end) return std::nullopt;
				
					//	Keep track of the start point in case we
					//	need to rewind
					auto start=begin;
					
					//	Add each character
					std::size_t i=0;
					String s;
					for (;(i<num) && (begin!=end);++i) s << *(begin++);
					
					//	If the requested number of characters are not
					//	taken, fail
					if (i!=num) {
					
						begin=start;
						
						return std::nullopt;
					
					}
					
					//	Attempt conversion to a code point
					auto cp=s.To<CodePoint::Type>(base);
					if (!cp) {
					
						begin=start;
						
						return std::nullopt;
					
					}
					
					return CodePoint(*cp);
				
				}
				
				
				static std::size_t get_num (CodePoint cp) noexcept {
				
					switch (cp) {
					
						case 'x':
							return 2;
						case 'u':
							return 4;
						case 'U':
							return 8;
						default:
							return 5;
					
					}
				
				}
			
			
				static CodePoint get (const CodePoint * & begin, const CodePoint * end) {
				
					//	If the current code point is not a backslash, just
					//	return the current code point
					if (*begin!='\\') return *begin;
					
					//	If the current code point is a backslash, there is
					//	custom handling -- the backslash begins an escape
					//	sequence
					
					++begin;
					
					//	If the backslash is at the end of the string, return
					//	the literal backslash
					if (begin==end) return '\\';
					
					//	Get the next character from the stream
					auto next=*(begin++);
					
					switch (next) {
					
						case 'a':
							return 0x7;
						case 'b':
							return 0x8;
						case 't':
							return 0x9;
						case 'r':
							return 0xD;
						case 'v':
							return 0xB;
						case 'f':
							return 0xC;
						case 'n':
							return 0xA;
						case 'e':
							return 0x1B;
						//	Various hexadecimal escapes
						case 'x':
						case 'u':
						case 'U':{
							auto start=begin;
							auto cp=get(begin,end,get_num(next));
							if (cp) return *cp;
							begin=start;
							break;
						}
						//	Octal escape
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':{
							auto start=begin;
							--begin;
							auto cp=get(begin,end,3,8);
							if (cp) return *cp;
							begin=start;
							break;
						}
						default:
							break;
							
					}
					
					//	If all else fails, take the current code point
					//	literally
					return next;
				
				}
				
				
				static bool ignore (RegexCompiler & compiler) noexcept {
				
					//	White space is not ignored in character classes
					if (compiler.CharacterClass) return false;
				
					if (compiler.Check(RegexOptions::IgnorePatternWhiteSpace) && compiler->IsWhiteSpace()) return true;
					
					return false;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (ignore(compiler)) return true;
					
					(
						(compiler.Successive && !compiler.CharacterClass)
							?	compiler.Back<RegexLiteral>()
							:	compiler.Add<RegexLiteral>()
					).Add(get(compiler.Current,compiler.End));
					
					return true;
				
				}
				
				
				virtual void Complete (RegexPatternElement & element) const override {
				
					reinterpret_cast<RegexLiteral &>(element).Complete();
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexLiteralParser> installer(std::numeric_limits<std::size_t>::max());


}
