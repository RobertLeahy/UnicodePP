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
	
		auto curr=*(begin++);
	
		//	If the current code point is not a backslash, just
		//	return the current code point
		if (curr!='\\') return curr;
		
		//	If the current code point is a backslash, there is
		//	custom handling -- the backslash begins an escape
		//	sequence
		
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
				auto cp=get(begin,end,get_num(next));
				if (cp) return *cp;
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
				--begin;
				auto cp=get(begin,end,3,8);
				if (cp) return *cp;
				++begin;
				break;
			}
			default:
				break;
				
		}
		
		//	If all else fails, take the current code point
		//	literally
		return next;
	
	}


	namespace {
	
	
		class RegexLiteral : public RegexPatternElement {
		
		
			private:
			
			
				const CodePoint * last;
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
			
			
				RegexLiteral (RegexOptions options, const Unicode::Locale & locale) noexcept : RegexPatternElement(options,locale), last(nullptr) {
				
					str.SetLocale(locale);
				
				}
				
				
				RegexLiteral (String str, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						last(nullptr),
						str(std::move(str))
				{
				
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
				
					auto s=str.Size();
				
					str << cp;
					
					last=str.begin()+s;
				
				}
				
				
				void Complete () {
				
					str.ToNFD();
					
					if (Check(RegexOptions::IgnoreCase)) str.ToCaseFold();
				
				}
				
				
				RegexCompiler::Element GetLast () {
				
					if ((last==nullptr) || (last==str.begin())) return RegexCompiler::Element{};
					
					String s(last,static_cast<const CodePoint *>(str.end()));
					str.CodePoints().resize(last-str.begin());
					last=nullptr;
					
					auto ptr=new RegexLiteral(
						std::move(s),
						Options,
						Locale
					);
					auto retr=RegexCompiler::Element(ptr);
					
					ptr->Complete();
					
					return retr;
				
				}
		
		
		};
	
	
		class RegexLiteralParser : public RegexParser {
		
		
			private:
				
				
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
				
				
				virtual RegexCompiler::Element GetLast (RegexPatternElement & element) const override {
				
					return reinterpret_cast<RegexLiteral &>(element).GetLast();
				
				}
		
		
		};
	
	
		class RegexRange : public RegexPatternElement {
		
		
			private:
			
			
				CodePoint a;
				CodePoint b;
				
				
				static CodePoint fold (CodePoint cp, const Unicode::Locale & locale) noexcept {
				
					return CaseConverter(locale,false).Fold(&cp,&cp,&cp+1)[0];
				
				}
				
				
				static CodePoint get (CodePoint cp, RegexOptions options, const Unicode::Locale & locale) noexcept {
				
					return Unicode::Check(options,RegexOptions::IgnoreCase) ? fold(cp,locale) : cp;
				
				}
				
				
				static String format (CodePoint cp) {
				
					String retr("U+");
					String str(Converter<CodePoint::Type>{}(cp));
					str.ToUpper();
					for (auto i=str.Size();i<4;++i) retr << "0";
					retr << str;
					
					return retr;
				
				}
				
				
			public:
			
			
				RegexRange (CodePoint a, CodePoint b, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						a(get(a,options,locale)),
						b(get(b,options,locale))
				{
				
					if (this->a>this->b) std::swap(this->a,this->b);
				
				}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					if (!engine) return false;
					
					auto cp=*engine;
					
					if (Check(RegexOptions::IgnoreCase)) cp=fold(cp,Locale);
					
					if ((cp<a) || (cp>b)) return false;
					
					++engine;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Any code point ";
					if (Check(RegexOptions::IgnoreCase)) retr.Parent << "whose simple case folding is ";
					retr.Parent << "between " << format(a) << " and " << format(b) << " (inclusive)";
					
					return retr;
				
				}
		
		
		};
	
	
		class RegexRangeParser : public RegexParser {
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					auto first=get(compiler.Current,compiler.End);
					
					if (!(compiler.IsNext('-') && compiler && (*compiler!=']'))) return false;
					
					auto second=get(compiler.Current,compiler.End);
					
					if (first==second) compiler.Raise("Range has both endpoints equal");
					if (first>second) compiler.Raise("Range has highest code point first");
					
					compiler.Add<RegexRange>(first,second);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	constexpr std::size_t priority=std::numeric_limits<std::size_t>::max();
	
	
	static const RegexParserInstaller<RegexLiteralParser> installer_1(priority);
	static const RegexParserInstaller<RegexRangeParser> installer_2(priority-1);


}
