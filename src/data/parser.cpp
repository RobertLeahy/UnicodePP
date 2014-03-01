#include "enum.hpp"
#include "parser.hpp"
#include "path.hpp"
#include "tointeger.hpp"
#include <algorithm>
#include <cstdlib>


using namespace Unicode;


static const std::pair<GeneralCategory,const char *> general_category []={
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


static const std::pair<BidirectionalClass,const char *> bidi_class []={
	{BidirectionalClass::L,"L"},
	{BidirectionalClass::R,"R"},
	{BidirectionalClass::AL,"AL"},
	{BidirectionalClass::EN,"EN"},
	{BidirectionalClass::ES,"ES"},
	{BidirectionalClass::ET,"ET"},
	{BidirectionalClass::AN,"AN"},
	{BidirectionalClass::CS,"CS"},
	{BidirectionalClass::NSM,"NSM"},
	{BidirectionalClass::BN,"BN"},
	{BidirectionalClass::B,"B"},
	{BidirectionalClass::S,"S"},
	{BidirectionalClass::WS,"WS"},
	{BidirectionalClass::ON,"ON"},
	{BidirectionalClass::LRE,"LRE"},
	{BidirectionalClass::LRO,"LRO"},
	{BidirectionalClass::RLE,"RLE"},
	{BidirectionalClass::RLO,"RLO"},
	{BidirectionalClass::PDF,"PDF"},
	{BidirectionalClass::LRI,"LRI"},
	{BidirectionalClass::RLI,"RLI"},
	{BidirectionalClass::FSI,"FSI"},
	{BidirectionalClass::PDI,"PDI"}
};


static const std::pair<GraphemeClusterBreak,const char *> grapheme_cluster_break []={
	{GraphemeClusterBreak::CR,"CR"},
	{GraphemeClusterBreak::LF,"LF"},
	{GraphemeClusterBreak::Control,"Control"},
	{GraphemeClusterBreak::Extend,"Extend"},
	{GraphemeClusterBreak::RegionalIndicator,"Regional_Indicator"},
	{GraphemeClusterBreak::Prepend,"Prepend"},
	{GraphemeClusterBreak::SpacingMark,"SpacingMark"},
	{GraphemeClusterBreak::L,"L"},
	{GraphemeClusterBreak::V,"V"},
	{GraphemeClusterBreak::T,"T"},
	{GraphemeClusterBreak::LV,"LV"},
	{GraphemeClusterBreak::LVT,"LVT"},
	{GraphemeClusterBreak::XX,"XX"}
};


static const std::pair<LineBreak,const char *> line_break []={
	{LineBreak::BK,"BK"},
	{LineBreak::CR,"CR"},
	{LineBreak::LF,"LF"},
	{LineBreak::CM,"CM"},
	{LineBreak::NL,"NL"},
	{LineBreak::SG,"SG"},
	{LineBreak::WJ,"WJ"},
	{LineBreak::ZW,"ZW"},
	{LineBreak::GL,"GL"},
	{LineBreak::SP,"SP"},
	{LineBreak::B2,"B2"},
	{LineBreak::BA,"BA"},
	{LineBreak::BB,"BB"},
	{LineBreak::HY,"HY"},
	{LineBreak::CB,"CB"},
	{LineBreak::CL,"CL"},
	{LineBreak::CP,"CP"},
	{LineBreak::EX,"EX"},
	{LineBreak::IN,"IN"},
	{LineBreak::NS,"NS"},
	{LineBreak::OP,"OP"},
	{LineBreak::QU,"QU"},
	{LineBreak::IS,"IS"},
	{LineBreak::NU,"NU"},
	{LineBreak::PO,"PO"},
	{LineBreak::PR,"PR"},
	{LineBreak::SY,"SY"},
	{LineBreak::AI,"AI"},
	{LineBreak::AL,"AL"},
	{LineBreak::CJ,"CJ"},
	{LineBreak::H2,"H2"},
	{LineBreak::H3,"H3"},
	{LineBreak::HL,"HL"},
	{LineBreak::ID,"ID"},
	{LineBreak::JL,"JL"},
	{LineBreak::JV,"JV"},
	{LineBreak::JT,"JT"},
	{LineBreak::RI,"RI"},
	{LineBreak::SA,"SA"},
	{LineBreak::XX,"XX"}
};


static const std::pair<WordBreak,const char *> word_break []={
	{WordBreak::CR,"CR"},
	{WordBreak::LF,"LF"},
	{WordBreak::Newline,"Newline"},
	{WordBreak::Extend,"Extend"},
	{WordBreak::RegionalIndicator,"Regional_Indicator"},
	{WordBreak::Format,"Format"},
	{WordBreak::Katakana,"Katakana"},
	{WordBreak::HebrewLetter,"Hebrew_Letter"},
	{WordBreak::ALetter,"ALetter"},
	{WordBreak::SingleQuote,"Single_Quote"},
	{WordBreak::DoubleQuote,"Double_Quote"},
	{WordBreak::MidNumLet,"MidNumLet"},
	{WordBreak::MidLetter,"MidLetter"},
	{WordBreak::MidNum,"MidNum"},
	{WordBreak::Numeric,"Numeric"},
	{WordBreak::ExtendNumLet,"ExtendNumLet"},
	{WordBreak::XX,"XX"}
};


static const std::pair<SentenceBreak,const char *> sentence_break []={
	{SentenceBreak::CR,"CR"},
	{SentenceBreak::LF,"LF"},
	{SentenceBreak::Extend,"Extend"},
	{SentenceBreak::Sep,"Sep"},
	{SentenceBreak::Format,"Format"},
	{SentenceBreak::Sp,"Sp"},
	{SentenceBreak::Lower,"Lower"},
	{SentenceBreak::Upper,"Upper"},
	{SentenceBreak::OLetter,"OLetter"},
	{SentenceBreak::Numeric,"Numeric"},
	{SentenceBreak::ATerm,"ATerm"},
	{SentenceBreak::SContinue,"SContinue"},
	{SentenceBreak::STerm,"STerm"},
	{SentenceBreak::Close,"Close"},
	{SentenceBreak::XX,"XX"}
};


static const std::pair<QuickCheck,const char *> quick_check []={
	{QuickCheck::Yes,"Yes"},
	{QuickCheck::No,"No"},
	{QuickCheck::Maybe,"Maybe"}
};


static const std::pair<const char *,const char *> conditions []={
	{"Final_Sigma","FinalSigma"},
	{"After_Soft_Dotted","AfterSoftDotted"},
	{"More_Above","MoreAbove"},
	{"Before_Dot","BeforeDot"},
	{"After_I","AfterI"}
};


static const std::pair<NumericType,const char *> numeric_type []={
	{NumericType::Decimal,"Decimal"},
	{NumericType::Digit,"Digit"},
	{NumericType::Numeric,"Numeric"}
};


static bool in_range (CodePoint::Type cp, CodePoint::Type low, CodePoint::Type high) noexcept {

	return (cp>=low) && (cp<=high);

}


bool Parser::Info::in_range (CodePoint::Type low, CodePoint::Type high) const noexcept {

	return ::in_range(CodePoint,low,high);

}


Parser::Info::Info (Unicode::CodePoint::Type cp) noexcept
	:	CodePoint(cp),
		WhiteSpace(false),
		Alphabetic(false),
		NoncharacterCodePoint(false),
		DefaultIgnorableCodePoint(false),
		Deprecated(false),
		LogicalOrderException(false),
		VariationSelector(false),
		Uppercase(false),
		Lowercase(false),
		SoftDotted(false),
		Cased(false),
		CaseIgnorable(false),
		HexDigit(false),
		ASCIIHexDigit(false),
		CanonicalCombiningClass(0),
		CompositionExclusion(false),
		FullCompositionExclusion(false),
		NFCQuickCheck(QuickCheck::Yes),
		NFDQuickCheck(QuickCheck::Yes),
		LineBreak(Unicode::LineBreak::XX),
		GraphemeClusterBreak(Unicode::GraphemeClusterBreak::XX),
		SentenceBreak(Unicode::SentenceBreak::XX),
		WordBreak(Unicode::WordBreak::XX),
		BidirectionalControl(false),
		Mirrored(false),
		Math(false),
		QuotationMark(false),
		Dash(false),
		STerm(false),
		TerminalPunctuation(false),
		Diacritic(false),
		Extender(false),
		GraphemeBase(false),
		GraphemeLink(false)
{

	//	Certain code points do not have a default
	//	line break property of XX
	if (
		in_range(0x3400,0x4DBF) ||
		in_range(0x4E00,0x9FFF) ||
		in_range(0xF900,0xFAFF) ||
		in_range(0x20000,0x2FFFD) ||
		in_range(0x30000,0x3FFFD)
	) LineBreak=Unicode::LineBreak::ID;
	else if (in_range(0x20A0,0x20CF)) LineBreak=Unicode::LineBreak::PR;

}


void Parser::Info::Derive () noexcept {

	typedef Unicode::GeneralCategory GC;

	//	Lowercase
	if (GeneralCategory==GC::Ll) Lowercase=true;
	
	//	Uppercase
	if (GeneralCategory==GC::Lu) Uppercase=true;
	
	//	Cased
	if (Lowercase || Uppercase || (GeneralCategory==GC::Lt)) Cased=true;
	
	//	Case ignorable
	switch (GeneralCategory) {
	
		case GC::Mn:
		case GC::Me:
		case GC::Cf:
		case GC::Lm:
		case GC::Sk:
			CaseIgnorable=true;
		default:
			break;
	
	}
	
	switch (WordBreak) {
	
		case Unicode::WordBreak::MidLetter:
		case Unicode::WordBreak::MidNumLet:
			CaseIgnorable=true;
		default:
			break;
	
	}
	
	//	Alphabetic
	switch (GeneralCategory) {
	
		case GC::Lu:
		case GC::Ll:
		case GC::Lt:
		case GC::Lm:
		case GC::Lo:
		case GC::Nl:
			Alphabetic=true;
		default:
			break;
	
	}
	
	//	Default Ignorable Code Point
	
	//	Handle the negative case first
	if (
		WhiteSpace ||
		(
			(CodePoint>=0xFFF9) &&
			(CodePoint<=0xFFFB)
		) ||
		(
			(CodePoint>=0x0600) &&
			(CodePoint<=0x0604)
		) ||
		(CodePoint==0x06DD) ||
		(CodePoint==0x070F) ||
		(CodePoint==0x110BD)
	) DefaultIgnorableCodePoint=false;
	//	Now the positive case
	else if (
		(GeneralCategory==GC::Cf) ||
		VariationSelector
	) DefaultIgnorableCodePoint=true;
	
	//	Grapheme Base
	switch (GeneralCategory) {
	
		case GC::Cc:
		case GC::Cf:
		case GC::Cs:
		case GC::Co:
		case GC::Cn:
		case GC::Zl:
		case GC::Zp:
			GraphemeBase=false;
			break;
		default:
			if (GraphemeClusterBreak==Unicode::GraphemeClusterBreak::Extend) {
				GraphemeBase=false;
				break;
			}
			GraphemeBase=true;
			break;
	
	}
	
	//	Grapheme Link
	
	//	Canonical_Combining_Class=Virama
	if (CanonicalCombiningClass==9) GraphemeLink=true;
	
	//	Math
	if (GeneralCategory==GC::Sm) Math=true;

}


std::vector<Parser::Info>::iterator Parser::lower_bound (CodePoint::Type cp) noexcept {

	return std::lower_bound(
		info.begin(),
		info.end(),
		cp,
		[] (const Info & a, CodePoint::Type cp) noexcept {	return a.CodePoint<cp;	}
	);

}


std::vector<Parser::Info>::iterator Parser::find (CodePoint::Type cp) noexcept {

	auto iter=lower_bound(cp);
	auto end=info.end();
	return ((iter==end) || (iter->CodePoint!=cp)) ? end : iter;

}


Parser::Info & Parser::get (CodePoint::Type cp) {

	auto iter=lower_bound(cp);
	
	if (
		(iter!=info.end()) &&
		(iter->CodePoint==cp)
	) return *iter;
	
	auto loc=iter-info.begin();
	info.emplace(iter,cp);
	return info[loc];

}


CodePoint::Type Parser::get_cp (const Item & item) {

	auto cp=item.CodePoint();
	if (!cp) throw std::runtime_error("Expected code point");
	
	return *cp;

}


Parser::Info & Parser::get (const Item & item) {

	return get(get_cp(item));

}


[[noreturn]]
static void bad_case_mapping () {

	throw std::runtime_error("Bad case mapping in UnicodeData.txt");

}


static std::optional<CodePoint::Type> get_simple_case_mapping (const Item & i) {

	auto mapping=i.CodePoints();
	
	if (!mapping) bad_case_mapping();
	
	switch (mapping->size()) {
	
		case 0:
			return std::optional<CodePoint::Type>{};
		case 1:
			return (*mapping)[0];
		default:
			bad_case_mapping();
	
	}

}


ArrayAggregator<CodePoint::Type>::Key Parser::get_decomposition_mapping (const Item & item) {

	auto & str=item.Get();
	
	if (
		(str.size()==0) ||
		(str[0]=='<')
	) return cps.Add();
	
	auto dcm=item.CodePoints();
	if (!dcm) throw std::runtime_error("Bad decomposition mapping in UnicodeData.txt");
	
	return cps.Add(std::move(*dcm));

}


[[noreturn]]
static void invalid_numeric () {

	throw std::runtime_error("Invalid numeric entry in UnicodeData.txt");

}


static double get_digit (const std::string & str) {

	auto num=ToInteger<std::size_t>(str);
	if (
		!num ||
		(*num>9)
	) invalid_numeric();
	
	return static_cast<double>(*num);

}


static std::optional<double> get_double (const std::string & str) {

	if (str.size()==0) return std::optional<double>{};
	
	auto begin=&str[0];
	auto end=&str[0]+str.size();
	
	char * str_end;
	auto retr=std::strtod(begin,&str_end);
	if (str_end!=end) return std::optional<double>{};
	
	return retr;

}


static double get_numeric (const std::string & str) {

	auto dbl=get_double(str);
	if (dbl) return *dbl;
	
	std::string num;
	std::string denom;
	bool found=false;
	for (auto c : str) {
	
		if (c=='/') {
		
			if (found) invalid_numeric();
			
			found=true;
			
			continue;
		
		}
		
		if (found) denom.push_back(c);
		else num.push_back(c);
	
	}
	
	if (!found) invalid_numeric();
	
	auto num_d=get_double(num);
	if (!num_d) invalid_numeric();
	auto denom_d=get_double(denom);
	if (
		!denom_d ||
		(*denom_d==0)
	) invalid_numeric();
	
	return *num_d/ *denom_d;

}


static std::optional<Numeric> get_numeric (const Line & line) {

	auto & six=line[6].Get();
	auto & seven=line[7].Get();
	auto & eight=line[8].Get();
	
	NumericType type;
	double value;
	if (six.size()==0) {
	
		if ((seven.size()==0) && (eight.size()==0)) return std::optional<Numeric>{};
	
		//	Either Digit or Numeric
		
		if (eight.size()==0) invalid_numeric();
		
		if (seven.size()==0) {
		
			//	Numeric
			
			type=NumericType::Numeric;
			value=get_numeric(eight);
		
		} else {
		
			//	Digit
			
			type=NumericType::Digit;
			value=get_digit(seven);
		
		}
	
	} else if ((seven.size()==0) || (eight.size()==0)) {
	
		invalid_numeric();
	
	} else {
	
		//	Decimal
		
		type=NumericType::Decimal;
		value=get_digit(six);
	
	}

	return Numeric{type,value};

}


void Parser::get_data (const Line & line) {

	//	Verify this line, there must be 15 fields:
	//
	//	1.	Code point
	//	2.	Name
	//	3.	General category
	//	4.	Canonical combining class
	//	5.	Bidirectional category
	//	6.	Decomposition mapping
	//	7.	Decimal digit value
	//	8.	Digit value
	//	9.	Numeric value
	//	10.	Mirrored (Y/N)
	//	11.	Unicode 1.0 Name
	//	12.	10646 comment field
	//	13.	Uppercase mapping (simple)
	//	14.	Lowercase mapping (simple)
	//	15.	Titlecase mapping (simple)
	if (line.size()<15) throw std::runtime_error("Bad line in UnicodeData.txt");

	//	Get the code point info structure
	//	for this code point
	auto & info=get(line[0]);
	
	//	Populate
	
	info.Name=line[1].Get();
	
	info.GeneralCategory=ToEnum(general_category,line[2].Get());
	
	auto ccc=ToInteger<decltype(info.CanonicalCombiningClass)>(line[3].Get());
	if (!ccc) throw std::runtime_error("Bad canonical combining class in UnicodeData.txt");
	info.CanonicalCombiningClass=*ccc;
	
	info.BidirectionalClass=ToEnum(bidi_class,line[4].Get());
	
	info.DecompositionMapping=get_decomposition_mapping(line[5]);
	
	info.Numeric=get_numeric(line);
	
	auto & mirrored=line[9].Get();
	if (mirrored=="Y") info.Mirrored=true;
	else if (mirrored=="N") info.Mirrored=false;
	else throw std::runtime_error("Bad mirrored in UnicodeData.txt");
	
	info.SimpleUppercaseMapping=get_simple_case_mapping(line[11]);
	info.SimpleLowercaseMapping=get_simple_case_mapping(line[12]);
	info.SimpleTitlecaseMapping=get_simple_case_mapping(line[13]);
	
	//	Derived properties
	info.Derive();

}


void Parser::get_data () {

	for (auto & line : data) get_data(line);

}


const std::pair<bool (Parser::Info::*),const char *> Parser::prop_map []={
	{&Info::WhiteSpace,"White_Space"},
	{&Info::Dash,"Dash"},
	{&Info::Hyphen,"Hyphen"},
	{&Info::QuotationMark,"Quotation_Mark"},
	{&Info::TerminalPunctuation,"Terminal_Punctuation"},
	{&Info::Math,"Other_Math"},
	{&Info::HexDigit,"Hex_Digit"},
	{&Info::ASCIIHexDigit,"ASCII_Hex_Digit"},
	{&Info::Alphabetic,"Other_Alphabetic"},
	{&Info::Diacritic,"Diacritic"},
	{&Info::Extender,"Extender"},
	{&Info::Lowercase,"Other_Lowercase"},
	{&Info::Uppercase,"Other_Uppercase"},
	{&Info::NoncharacterCodePoint,"Noncharacter_Code_Point"},
	{&Info::DefaultIgnorableCodePoint,"Other_Default_Ignorable_Code_Point"},
	{&Info::Deprecated,"Deprecated"},
	{&Info::SoftDotted,"Soft_Dotted"},
	{&Info::LogicalOrderException,"Logical_Order_Exception"},
	{&Info::VariationSelector,"Variation_Selector"},
	{&Info::STerm,"STerm"},
	{&Info::BidirectionalControl,"Bidi_Control"}
};


void Parser::get_prop_list () {

	foreach(prop_list,2,[&] (Info & info, const Line & line) noexcept {
	
		auto & str=line[1].Get();
		
		for (auto & pair : prop_map) if (str==pair.second) {
		
			info.*(pair.first)=true;
			
			info.Derive();
			
			return;
		
		};
	
	});

}


static std::size_t get_string (
	const std::string & str,
	std::vector<std::string> & vec,
	std::unordered_map<std::string,std::size_t> & map
) {

	auto iter=map.find(str);
	if (iter!=map.end()) return iter->second;
	
	auto key=vec.size();
	vec.push_back(str);
	map.emplace(str,key);
	
	return key;

}


void Parser::get_blocks (const Range & range, std::size_t i) noexcept {

	foreach(range,[&] (Info & info) noexcept {	info.Block=i;	});

}


void Parser::get_blocks (const Line & line) {

	if (line.size()<2) throw std::runtime_error("Bad line in Blocks.txt");
	
	auto i=get_string(line[1].Get(),blocks,blocks_map);
	
	auto range=line[0].Range();
	if (range) {
	
		get_blocks(*range,i);
		
		return;
	
	}
	
	auto cp=get_cp(line[0]);
	get_blocks(Range{cp,cp},i);

}


void Parser::get_blocks () {

	for (auto & line : b) get_blocks(line);

}


void Parser::get_scripts (const Range & range, std::size_t i) noexcept {

	foreach(range,[&] (Info & info) noexcept {	info.Script=i;	});

}


void Parser::get_scripts (const Line & line) {

	if (line.size()<2) throw std::runtime_error("Bad line in Scripts.txt");
	
	auto i=get_string(line[1].Get(),scripts,scripts_map);
	
	auto range=line[0].Range();
	if (range) {
	
		get_scripts(*range,i);
		
		return;
	
	}
	
	auto cp=get_cp(line[0]);
	get_scripts(Range{cp,cp},i);

}


void Parser::get_scripts () {

	for (auto & line : s) get_scripts(line);

}


void Parser::get_name_aliases (const Line & line) {

	if (line.size()<3) throw std::runtime_error("Bad line in NameAliases.txt");
	
	auto iter=find(get_cp(line[0]));
	if (iter==info.end()) return;
	
	auto & alias=line[1].Get();
	auto & str=line[2].Get();
	
	if (str=="abbreviation") iter->Abbreviation=alias;
	else if (str=="correction") iter->Name=alias;
	else iter->Alias=alias;

}


void Parser::get_name_aliases () {

	for (auto & line : name_aliases) get_name_aliases(line);

}


void Parser::get_composition_exclusions () {

	foreach(composition_exclusions,1,[&] (Info & info, const Line &) noexcept {
	
		info.CompositionExclusion=true;
		
		info.Derive();
	
	});

}


void Parser::get_line_break () {

	foreach(line_break,2,[&] (Info & info, const Line & line) {
	
		info.LineBreak=ToEnum(::line_break,line[1].Get());
		
		info.Derive();
	
	});

}


void Parser::get_grapheme_cluster_break () {

	foreach(grapheme_break,2,[&] (Info & info, const Line & line) {
	
		info.GraphemeClusterBreak=ToEnum(grapheme_cluster_break,line[1].Get());
		
		info.Derive();
	
	});

}


void Parser::get_sentence_break () {

	foreach(sentence_break,2,[&] (Info & info, const Line & line) {
	
		info.SentenceBreak=ToEnum(::sentence_break,line[1].Get());
		
		info.Derive();
	
	});

}


void Parser::get_word_break () {

	foreach(sentence_break,2,[&] (Info & info, const Line & line) {
	
		info.WordBreak=ToEnum(::word_break,line[1].Get());
		
		info.Derive();
	
	});

}


void Parser::get_derived_normalization_props (Info & info, bool nfd, const std::string & val) {

	QuickCheck qc;
	if (val=="Y") qc=QuickCheck::Yes;
	else if (val=="M") qc=QuickCheck::Maybe;
	else if (val=="N") qc=QuickCheck::No;
	else throw std::runtime_error("Invalid quick check value in DerivedNormalizationProps.txt");
	
	if (nfd) info.NFDQuickCheck=qc;
	else info.NFCQuickCheck=qc;
	
	info.Derive();

}


void Parser::get_derived_normalization_props () {

	//	DerivedNormalizationProps.txt has lines with only
	//	2 entries, all the ones we want have at least 3,
	//	so we'll have to do a separate check
	foreach(derived_normalization_props,2,[&] (Info & info, const Line & line) {
	
		auto & str=line[1].Get();
		bool nfd;
		if (str=="NFD_QC") nfd=true;
		else if (str=="NFC_QC") nfd=false;
		else return;
	
		if (line.size()<3) throw std::runtime_error("Bad line in DerivedNormalizationProps.txt");
		
		get_derived_normalization_props(info,nfd,line[2].Get());
	
	});
	
}


void Parser::get_full_composition_exclusion (Info & info) {
	
	//	If this code point is excluded from composition
	//	already, clearly the FullCompositionExclusion
	//	property is true
	//
	//	Alternately, if this code point is a non-starter,
	//	it's excluded from composition
	if (
		info.CompositionExclusion ||
		(info.CanonicalCombiningClass!=0)
	) {
	
		info.FullCompositionExclusion=true;
		
		return;
	
	}
	
	auto & vec=cps[info.DecompositionMapping];
	
	//	If there's no decomposition, ignore
	if (vec.size()==0) return;
	
	//	If this code point decomposes to a single character,
	//	it shouldn't be composed to
	if (vec.size()==1) {
	
		info.FullCompositionExclusion=true;
		
		return;
	
	}
	
	//	If this code point decomposes to a sequence
	//	which begins with a non-starter, it is excluded
	
	auto iter=find(vec[0]);
	if (!(
		(iter==this->info.end()) ||
		(iter->CanonicalCombiningClass==0)
	)) {
	
		info.FullCompositionExclusion=true;
		
		return;
	
	}

}


void Parser::get_full_composition_exclusion () {

	for (auto & i : info) get_full_composition_exclusion(i);

}


void Parser::output (const std::string & str) {

	if (str.size()==0) out << "nullptr";
	else out.WriteString(str);

}


void Parser::output (bool bln) {

	out << (bln ? "true" : "false");

}


void Parser::output (const ConditionInfo & c) {

	out << "{";
	
	output(c.Negated);
	out << ",";
	bool found=false;
	for (auto & pair : conditions) if (c.Name==pair.first) {
	
		out << "Conditions::" << pair.second;
		
		found=true;
		
		break;
	
	}
	if (!found) out.WriteString(c.Name);
	
	out << "}";

}


void Parser::output (const ::Array & arr, const std::string & str) {

	out << "{";
	if (arr.Size==0) out << "nullptr,0";
	else out << "&" << str << "[" << arr.Offset << "]," << arr.Size;
	out << "}";

}


void Parser::output (const Numeric & n) {

	out << "Numeric{NumericType::" << FromEnum(numeric_type,n.Type) << "," << n.Value << "}";

}


void Parser::next (bool newline) {

	out << ",";
	if (newline) out << Newline;

}


void Parser::output_cps () {

	out.BeginArray("CodePoint::Type","cps");
	bool first=true;
	for (auto cp : cps) {
	
		if (first) first=false;
		else out << ",";
		
		output(cp);
	
	}
	out.EndArray();

}


void Parser::output_blocks () {

	out.BeginArray("char *","blocks");
	out.BeginIndent();
	bool first=true;
	for (auto & block : blocks) {
	
		if (first) first=false;
		else out << "," << Newline;
		
		output(block);
	
	}
	out.EndIndent();
	out.EndArray();	

}


void Parser::output_scripts () {

	out.BeginArray("char *","scripts");
	out.BeginIndent();
	bool first=true;
	for (auto & script : scripts) {
		
		if (first) first=false;
		else out << "," << Newline;
		
		output(script);
		
	}
	out.EndIndent();
	out.EndArray();

}


void Parser::output_conds () {

	out.BeginArray("Condition","conds");
	
	bool first=true;
	for (auto & c : conds) {
	
		if (first) first=false;
		else out << ",";
		
		output(c);
	
	}
	
	out.EndArray();

}


void Parser::output_arrays () {

	output_cps();
	out.WhiteSpace();
	
	output_blocks();
	out.WhiteSpace();
	
	output_scripts();
	out.WhiteSpace();
	
	output_conds();
	out.WhiteSpace();
	
	casing.Output();

}


void Parser::output_code_point_info_inner (const Info & info) {

	//	Get casing information for this
	//	code point
	auto casing_info=casing.Get(info.CodePoint);

	output(info.Name);
	next();
	
	output(info.Alias);
	next();
	
	output(info.Abbreviation);
	next();
	
	if (info.Block) out << "blocks[" << *info.Block << "]";
	else out << "nullptr";
	next();
	
	out << "GeneralCategory::" << FromEnum(general_category,info.GeneralCategory);
	next();
	
	if (info.Script) out << "scripts[" << *info.Script << "]";
	else out << "nullptr";
	next();
	
	output(info.WhiteSpace);
	next();
	
	output(info.Alphabetic);
	next();
	
	output(info.NoncharacterCodePoint);
	next();
	
	output(info.DefaultIgnorableCodePoint);
	next();
	
	output(info.Deprecated);
	next();
	
	output(info.LogicalOrderException);
	next();
	
	output(info.VariationSelector);
	next();
	
	output(info.Uppercase);
	next();
	
	output(info.Lowercase);
	next();
	
	auto str="mappings";
	
	output(casing_info.LowercaseMappings,str);
	next();
	
	output(casing_info.TitlecaseMappings,str);
	next();
	
	output(casing_info.UppercaseMappings,str);
	next();
	
	output(casing_info.CaseFoldings,str);
	next();
	
	output(info.SimpleLowercaseMapping);
	next();
	
	output(info.SimpleTitlecaseMapping);
	next();
	
	output(info.SimpleUppercaseMapping);
	next();
	
	output(casing_info.SimpleCaseFolding);
	next();
	
	output(info.SoftDotted);
	next();
	
	output(info.Cased);
	next();
	
	output(info.CaseIgnorable);
	next();
	
	output(info.Numeric);
	next();
	
	output(info.HexDigit);
	next();
	
	output(info.ASCIIHexDigit);
	next();
	
	out << info.CanonicalCombiningClass;
	next();
	
	output(
		cps.Get(info.DecompositionMapping),
		"cps"
	);
	next();
	
	output(info.CompositionExclusion);
	next();
	
	output(info.FullCompositionExclusion);
	next();
	
	out << "QuickCheck::" << FromEnum(quick_check,info.NFCQuickCheck);
	next();
	
	out << "QuickCheck::" << FromEnum(quick_check,info.NFDQuickCheck);
	next();
	
	out << "LineBreak::" << FromEnum(::line_break,info.LineBreak);
	next();
	
	out << "GraphemeClusterBreak::" << FromEnum(::grapheme_cluster_break,info.GraphemeClusterBreak);
	next();
	
	out << "SentenceBreak::" << FromEnum(::sentence_break,info.SentenceBreak);
	next();
	
	out << "WordBreak::" << FromEnum(::word_break,info.WordBreak);
	next();
	
	out << "BidirectionalClass::" << FromEnum(bidi_class,info.BidirectionalClass);
	next();
	
	output(info.BidirectionalControl);
	next();
	
	output(info.Mirrored);
	next();
	
	output(info.Math);
	next();
	
	output(info.QuotationMark);
	next();
	
	output(info.Dash);
	next();
	
	output(info.Hyphen);
	next();
	
	output(info.STerm);
	next();
	
	output(info.TerminalPunctuation);
	next();
	
	output(info.Diacritic);
	next();
	
	output(info.Extender);
	next();
	
	output(info.GraphemeBase);
	next();
	
	output(info.GraphemeLink);

}


void Parser::output_code_point_info (const Info & info) {

	out << "{";
	out.BeginIndent();
	
	output(info.CodePoint);
	out << "," << Newline << "{";
	out.BeginIndent();
	
	output_code_point_info_inner(info);
	
	out.EndIndent();
	out << "}";
	
	out.EndIndent();
	out << "}";

}


void Parser::output_code_point_info () {

	out.BeginArray("std::pair<CodePoint::Type,CodePointInfo>","info");
	out.BeginIndent();

	bool first=true;
	for (auto & i : info) {
	
		if (first) first=false;
		else out << "," << Newline;
		
		output_code_point_info(i);
	
	}
	
	out.EndIndent();
	out.EndArray();

}


Parser::Parser (const std::string & base, const std::string & output)
	:	data(Join(base,"UnicodeData.txt").c_str()),
		prop_list(Join(base,"PropList.txt").c_str()),
		b(Join(base,"Blocks.txt").c_str()),
		s(Join(base,"Scripts.txt").c_str()),
		name_aliases(Join(base,"NameAliases.txt").c_str()),
		composition_exclusions(Join(base,"CompositionExclusions.txt").c_str()),
		line_break(Join(base,"LineBreak.txt").c_str()),
		grapheme_break(Join(base,"auxiliary","GraphemeBreakProperty.txt").c_str()),
		sentence_break(Join(base,"auxiliary","SentenceBreakProperty.txt").c_str()),
		word_break(Join(base,"auxiliary","WordBreakProperty.txt").c_str()),
		derived_normalization_props(Join(base,"DerivedNormalizationProps.txt").c_str()),
		out(output),
		casing(
			base,
			cps,
			conds,
			out
		)
{	}


void Parser::Get () {

	//	Fire nested objects
	casing.Get();
	
	//	Get UnicodeData.txt
	get_data();
	
	//	Get PropList.txt
	get_prop_list();
	
	//	Get Blocks.txt
	get_blocks();
	
	//	Get Scripts.txt
	get_scripts();
	
	//	Get NameAliases.txt
	get_name_aliases();
	
	//	Get CompositionExclusions.txt
	get_composition_exclusions();
	
	//	Get LineBreak.txt
	get_line_break();
	
	//	Get GraphemeBreakProperty.txt
	get_grapheme_cluster_break();
	
	//	Get SentenceBreakProperty.txt
	get_sentence_break();
	
	//	Get WordBreakProperty.txt
	get_word_break();
	
	//	Get DerivedNormalizationProps.txt
	get_derived_normalization_props();
	
	//	Derive full composition exclusion
	get_full_composition_exclusion();

}


void Parser::Output () {

	//	Print out required headers
	out.Header("unicode/codepoint.hpp");
	out.Header("unicode/conditions.hpp");
	out.Header("utility");
	
	//	Begin the Unicode namespace
	out.WhiteSpace();
	out.BeginNamespace("Unicode");
	
	//	Output arrays
	output_arrays();
	out.WhiteSpace();
	
	//	Output CodePointInfo structures
	output_code_point_info();
	
	//	Done
	out.EndNamespace();
	out.End();

}
