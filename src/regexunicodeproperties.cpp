#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <optional>
#include <stdexcept>
#include <utility>


namespace Unicode {


	namespace {
	
	
		enum class SuperCategory {
		
			L,
			M,
			N,
			P,
			S,
			Z,
			C
		
		};
		
		
		const std::pair<GeneralCategory,String> gc_map []={
			{GeneralCategory::Lu,"Lu"},
			{GeneralCategory::Ll,"Ll"},
			{GeneralCategory::Lt,"Lt"},
			{GeneralCategory::Mn,"Mn"},
			{GeneralCategory::Mc,"Mc"},
			{GeneralCategory::Me,"Me"},
			{GeneralCategory::Nd,"Nd"},
			{GeneralCategory::Nl,"Nl"},
			{GeneralCategory::No,"No"},
			{GeneralCategory::Zs,"Zs"},
			{GeneralCategory::Zl,"Zl"},
			{GeneralCategory::Zp,"Zp"},
			{GeneralCategory::Cc,"Cc"},
			{GeneralCategory::Cf,"Cf"},
			{GeneralCategory::Cs,"Cs"},
			{GeneralCategory::Co,"Co"},
			{GeneralCategory::Cn,"Cn"},
			{GeneralCategory::Lm,"Lm"},
			{GeneralCategory::Lo,"Lo"},
			{GeneralCategory::Pc,"Pc"},
			{GeneralCategory::Pd,"Pd"},
			{GeneralCategory::Ps,"Ps"},
			{GeneralCategory::Pe,"Pe"},
			{GeneralCategory::Pi,"Pi"},
			{GeneralCategory::Pf,"Pf"},
			{GeneralCategory::Po,"Po"},
			{GeneralCategory::Sm,"Sm"},
			{GeneralCategory::Sc,"Sc"},
			{GeneralCategory::Sk,"Sk"},
			{GeneralCategory::So,"So"}
		};
		const std::pair<SuperCategory,String> sc_map []={
			{SuperCategory::L,"L"},
			{SuperCategory::M,"M"},
			{SuperCategory::N,"N"},
			{SuperCategory::P,"P"},
			{SuperCategory::S,"S"},
			{SuperCategory::Z,"Z"},
			{SuperCategory::C,"C"}
		};
		template <typename T, std::size_t i>
		const String & get (T category, const std::pair<T,String> (& map) [i]) {
		
			for (auto & pair : map) if (pair.first==category) return pair.second;
			
			throw std::out_of_range("No string");
		
		}
		template <typename T, std::size_t i>
		std::optional<T> get (const String & str, const std::pair<T,String> (& map) [i]) {
		
			for (auto & pair : map) if (pair.second==str) return pair.first;
			
			return std::nullopt;
			
		}
		
		
		template <typename>
		class CategoryImpl {	};
		
		
		template <>
		class CategoryImpl<GeneralCategory> {
		
		
			public:
			
			
				static bool Check (GeneralCategory category, const CodePointInfo & cpi) noexcept {
				
					return cpi.GeneralCategory==category;
				
				}
				
				
				static const String & ToString (GeneralCategory category) {
				
					return get(category,gc_map);
				
				}
				
				
				static std::optional<GeneralCategory> FromString (const String & str) {
				
					return get(str,gc_map);
				
				}
		
		
		};
		
		
		template <>
		class CategoryImpl<SuperCategory> {
		
		
			public:
			
			
				static bool Check (SuperCategory category, const CodePointInfo & cpi) noexcept {
				
					switch (cpi.GeneralCategory) {
					
						case GeneralCategory::Lu:
						case GeneralCategory::Ll:
						case GeneralCategory::Lt:
						case GeneralCategory::Lm:
						case GeneralCategory::Lo:
							return category==SuperCategory::L;
						case GeneralCategory::Mn:
						case GeneralCategory::Mc:
						case GeneralCategory::Me:
							return category==SuperCategory::M;
						case GeneralCategory::Nd:
						case GeneralCategory::Nl:
						case GeneralCategory::No:
							return category==SuperCategory::N;
						case GeneralCategory::Pc:
						case GeneralCategory::Pd:
						case GeneralCategory::Ps:
						case GeneralCategory::Pe:
						case GeneralCategory::Pi:
						case GeneralCategory::Pf:
						case GeneralCategory::Po:
							return category==SuperCategory::P;
						case GeneralCategory::Sm:
						case GeneralCategory::Sc:
						case GeneralCategory::Sk:
						case GeneralCategory::So:
							return category==SuperCategory::S;
						case GeneralCategory::Zs:
						case GeneralCategory::Zl:
						case GeneralCategory::Zp:
							return category==SuperCategory::Z;
						case GeneralCategory::Cc:
						case GeneralCategory::Cf:
						case GeneralCategory::Cs:
						case GeneralCategory::Co:
						case GeneralCategory::Cn:
							return category==SuperCategory::C;
						default:
							return false;
					
					}
				
				}
				
				
				static const String & ToString (SuperCategory category) {
				
					return get(category,sc_map);
				
				}
				
				
				static std::optional<SuperCategory> FromString (const String & str) {
				
					return get(str,sc_map);
				
				}
		
		
		};
		
		
		template <typename T>
		class RegexCategory : public RegexPatternElement {
		
		
			private:
			
			
				T category;
				bool inverted;
				
				
			public:
			
			
				RegexCategory (T category, bool inverted, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						category(category),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					if (!state) return false;
					
					auto cpi=state->GetInfo(Locale);
					if (cpi==nullptr) return inverted;
					
					if (CategoryImpl<T>::Check(category,*cpi)==inverted) return false;
					
					++state;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Any code point ";
					if (inverted) retr.Parent << "not ";
					retr.Parent << "in the Unicode category " << CategoryImpl<T>::ToString(category);
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexBlockOrScript : public RegexPatternElement {
		
		
			private:
			
			
				String name;
				bool is_block;
				bool inverted;
				
				
				bool check (const char * str) const noexcept {
				
					if (str==nullptr) return false;
					
					auto begin=name.begin();
					auto end=name.end();
					for (
						;
						!((begin==end) || (*str=='\0'));
						++str
					) {
					
						if (*str==' ') continue;
						
						if (*str!=*begin) return false;
					
					}
					
					return (begin==end) && (*str=='\0');
				
				}
				
				
				bool check (const CodePointInfo & cpi) const noexcept {
				
					return check(is_block ? cpi.Block : cpi.Script);
				
				}
				
				
			public:
			
			
				RegexBlockOrScript (String name, bool is_block, bool inverted, RegexOptions options, const Unicode::Locale & locale) noexcept
					:	RegexPatternElement(options,locale),
						name(std::move(name)),
						is_block(is_block),
						inverted(inverted)
				{	}
				
				
				virtual bool operator () (RegexState & state, RegexPatternElementState &) const override {
				
					if (!state) return false;
					
					auto cpi=state->GetInfo(Locale);
					if (cpi==nullptr) return inverted;
					
					if (check(*cpi)==inverted) return false;
					
					++state;
					
					return true;
				
				}
				
				
				virtual RegexToString ToString () const override {
				
					RegexToString retr;
					retr.Parent << "Any code point ";
					if (inverted) retr.Parent << "not ";
					retr.Parent << "in the Unicode ";
					if (is_block) retr.Parent << "block";
					else retr.Parent << "script";
					retr.Parent << " \"" << name << "\"";
					
					return retr;
				
				}
		
		
		};
		
		
		class RegexUnicodePropertiesParser : public RegexParser {
		
		
			private:
			
			
				template <typename T>
				static void create (T category, bool inverted, RegexCompilerState & state) {
				
					state.Add<RegexCategory<T>>(category,inverted);
				
				}
				
				
				static void create (
					const CodePoint * begin,
					const CodePoint * end,
					bool is_block,
					bool inverted,
					RegexCompilerState & state
				) {
				
					state.Add<RegexBlockOrScript>(
						String(begin,end),
						is_block,
						inverted
					);
				
				}
			
			
				static void get (const String & str, bool inverted, RegexCompilerState & state) {
				
					if (str.Size()==0) state.Raise("No Unicode property specification");
					
					//	Try and get a super category
					auto sc=CategoryImpl<SuperCategory>::FromString(str);
					if (sc) {
					
						create(*sc,inverted,state);
						
						return;
						
					}
					
					//	Try and get a general category
					auto gc=CategoryImpl<GeneralCategory>::FromString(str);
					if (gc) {
					
						create(*gc,inverted,state);
						
						return;
						
					}
					
					//	Blocks and scripts require two code points at least
					if (str.Size()>2) {
					
						auto begin=str.begin();
						
						if (*(begin++)!='I') goto invalid;
						
						switch (*(begin++)) {
						
							//	Block
							case 's':
								create(begin,str.end(),true,inverted,state);
								return;
							//	Script
							case 'n':
								create(begin,str.end(),false,inverted,state);
								return;
							//	Invalid
							default:
								invalid:
								state.Raise("Invalid block or script specification");
						
						}
					
					}
					
					//	Invalid
					state.Raise("Invalid Unicode property specification");
				
				}
		
		
			public:
			
			
				virtual bool operator () (RegexCompilerState & state) const override {
				
					if (!((*state=='\\') && ++state)) return false;
					
					bool inverted;
					switch (*state) {
					
						case 'p':
							inverted=false;
							break;
						case 'P':
							inverted=true;
							break;
						default:
							return false;
					
					}
					
					if (!(++state && (*state=='{') && ++state)) return false;
					
					String str;
					while (*state!='}') {
					
						str << *state;
						
						if (!++state) state.Raise("No end of Unicode property specification");
					
					}
					
					++state;
					
					get(str,inverted,state);
					
					return true;
				
				}
		
		
		};
	
	
	}
	
	
	static const RegexParserInstaller<RegexUnicodePropertiesParser> installer;


}
