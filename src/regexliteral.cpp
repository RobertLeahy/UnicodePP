#include <unicode/caseconverter.hpp>
#include <unicode/codepoint.hpp>
#include <unicode/makereverseiterator.hpp>
#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <type_traits>
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

	
	static String format (std::size_t number) {
	
		return String(number);
	
	}
	
	
	static String format (const String & name) {
	
		String retr;
		retr << "\"" << name << "\"";
		
		return retr;
	
	}
	
	
	namespace {
	
	
		class RegexLiteralBase : public RegexPatternElement {
		
		
			private:
			
			
				template <typename Iterator>
				static bool case_sensitive_check (Iterator begin, Iterator end, RegexEngine & engine) noexcept {
				
					for (;(begin!=end) && engine;++engine,++begin) if (*begin!=*engine) return false;
					
					return begin==end;
				
				}
				
				
				template <typename Iterator1, typename Iterator2>
				static bool check (Iterator1 & begin_a, Iterator1 & end_a, Iterator2 begin_b, Iterator2 end_b) noexcept {
				
					for (;!((begin_a==end_a) || (begin_b==end_b));++begin_a,++begin_b) if (*begin_a!=*begin_b) return false;
					
					return begin_b==end_b;
				
				}
				
				
				template <typename Iterator>
				bool case_insensitive_check (Iterator begin, Iterator end, RegexEngine & engine) const noexcept {
				
					auto b=engine.Begin();
					auto e=engine.End();
					CaseConverter cc(Locale);
					for (;(begin!=end) && engine;++engine) {
					
						auto folding=cc.Fold(&(*engine),b,e);
						if (!(
							engine.Reversed() ? check(
								begin,
								end,
								MakeReverseIterator(folding.end()),
								MakeReverseIterator(folding.begin())
							) : check(
								begin,
								end,
								folding.begin(),
								folding.end()
							)
						)) return false;
					
					}
					
					return begin==end;	
				
				}
		
		
			protected:
			
			
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
			
			
				using RegexPatternElement::RegexPatternElement;
		
		
		};
	
	
		class RegexLiteral : public RegexLiteralBase {
		
		
			private:
			
			
				const CodePoint * last;
				String str;
		
		
			public:
			
			
				RegexLiteral (RegexOptions options, const Unicode::Locale & locale) noexcept : RegexLiteralBase(options,locale), last(nullptr) {
				
					str.SetLocale(locale);
				
				}
				
				
				RegexLiteral (String str, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexLiteralBase(options,locale),
						last(nullptr),
						str(std::move(str))
				{
				
					str.SetLocale(locale);
				
				}
			
			
				using RegexLiteralBase::RegexLiteralBase;
				
				
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
					if (Check(RegexOptions::IgnoreCase)) retr.Parent << " (case insensitive)";
					
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
				
				
				bool CanContinue (RegexCompiler & compiler) const noexcept {
				
					//	We cannot continue parsing the same pattern element if
					//	IgnoreCase has been set or unset -- that changes the
					//	behaviour of this pattern element, and therefore a new
					//	pattern element is required
					return Unicode::Check(compiler.Options,RegexOptions::IgnoreCase)==Check(RegexOptions::IgnoreCase);
				
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
					
					if (compiler.Successive && !compiler.CharacterClass) {
					
						auto & back=compiler.Back<RegexLiteral>();
						
						if (back.CanContinue(compiler)) {
						
							back.Add(get(compiler.Current,compiler.End));
							
							return true;
						
						}
					
					}
					
					compiler.Add<RegexLiteral>().Add(get(compiler.Current,compiler.End));
					
					return true;
				
				}
				
				
				virtual void Complete (RegexPatternElement & element) const override {
				
					reinterpret_cast<RegexLiteral &>(element).Complete();
				
				}
				
				
				virtual RegexCompiler::Element GetLast (RegexPatternElement & element) const override {
				
					return reinterpret_cast<RegexLiteral &>(element).GetLast();
				
				}
		
		
		};
	
	
		template <typename T>
		class RegexBackreference : public RegexLiteralBase {
		
		
			private:
			
			
				T name;
				
				
				template <typename Iterator>
				bool check (Iterator begin, Iterator end, RegexEngine & engine) const noexcept {
				
					return engine.Reversed() ? RegexLiteralBase::check(
						MakeReverseIterator(std::move(end)),
						MakeReverseIterator(std::move(begin)),
						engine
					) : RegexLiteralBase::check(
						std::move(begin),
						std::move(end),
						engine
					);
				
				}
				
				
			public:
			
			
				RegexBackreference (T name, RegexOptions options, const Unicode::Locale & locale) noexcept(
					std::is_nothrow_move_constructible<T>::value
				)	:	RegexLiteralBase(options,locale),
						name(std::move(name))
				{	}
				
				
				virtual bool operator () (RegexEngine & engine, RegexState &) const override {
				
					//	Check to see if the referenced capture has,
					//	any captures.  If it doesn't, always fail
					auto & captures=engine.Match[name];
					if (captures.size()==0) return false;
					
					//	Extract the begin and end iterators for the
					//	capture.  Due to the way RegexCapture objects
					//	are structured, extracting the begin and end
					//	iterators does not trigger evaluation (i.e.
					//	creation of an internal String object), and
					//	therefore is much less expensive
					auto & capture=captures.back();
					auto begin=capture.begin();
					auto end=capture.end();
					
					//	If case is being ignored, we must perform case
					//	folding before comparing
					if (Check(RegexOptions::IgnoreCase)) {
					
						auto folded=CaseConverter(Locale).Fold(begin,end);
						return check(folded.begin(),folded.end(),engine);
						
					}
					
					return check(begin,end,engine);
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "The value of capturing group " << format(name);
					if (Check(RegexOptions::IgnoreCase)) retr.Parent << " with case differences ignored";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexBackreferenceParser : public RegexParser {
		
		
			private:
			
			
				static bool k (RegexCompiler & compiler) {
				
					String name;
					for (;compiler && (*compiler!='>');++compiler) name << *compiler;
					
					if (!compiler) compiler.Raise("Unterminated backreference");
					
					++compiler;
					
					auto number=name.To<std::size_t>();
					if (number) compiler.Add<RegexBackreference<std::size_t>>(*number);
					else compiler.Add<RegexBackreference<String>>(std::move(name));
					
					return true;
				
				}
				
				
				static bool number (RegexCompiler & compiler) {
				
					String str;
					for (bool first=true;compiler;++compiler,first=false) {
					
						auto cpi=compiler->GetInfo(compiler.Locale);
						if (
							(cpi==nullptr) ||
							(!cpi->Numeric) ||
							(cpi->Numeric->Type!=NumericType::Decimal)
						) break;
						
						//	Leading zeroes lead to immediate failure
						if (first && (cpi->Numeric->Value==0.0)) return false;
						
						str << *compiler;
					
					}
					
					//	If there were no digits, fail
					if (str.Size()==0) return false;
					
					auto number=str.Convert<std::size_t>();
					
					//	If this capturing group doesn't exist, fail
					if (compiler.GetCapturingGroup(number)==nullptr) return false;
					
					compiler.Add<RegexBackreference<std::size_t>>(number);
					
					return true;
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompiler & compiler) const override {
				
					if (!compiler.IsNext('\\')) return false;
					
					if (compiler.IsNext("k<")) return k(compiler);
					
					return number(compiler);
				
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
	static const RegexParserInstaller<RegexBackreferenceParser> installer_2(false);
	static const RegexParserInstaller<RegexRangeParser> installer_3(priority-1);


}
