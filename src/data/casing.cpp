#include "casing.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>


using namespace Unicode;


CasingParser::Entry & CasingParser::get (Unicode::CodePoint::Type cp) {

	//	Find the insertion point
	auto end=entries.end();
	auto iter=std::lower_bound(
		entries.begin(),
		end,
		cp,
		[] (const Entry & a, Unicode::CodePoint::Type b) noexcept {
		
			return a.CodePoint<b;
		
		}
	);
	
	//	If the iterator we got back points to
	//	an entry for this code point, there's no
	//	need to insert, just return a reference
	//	to this entry
	if (
		(iter!=end) &&
		(iter->CodePoint==cp)
	) return *iter;
	
	//	Otherwise create and insert a new entry
	
	auto loc=entries.size();
	entries.emplace_back();
	auto & retr=entries[loc];
	
	retr.CodePoint=cp;
	retr.Lower=casings.Add();
	retr.Title=casings.Add();
	retr.Upper=casings.Add();
	retr.Folding=casings.Add();
	
	return retr;

}


CasingParser::conds_key CasingParser::get_tr () {

	if (!tr) tr=conds.Add({
		{false,"tr"},
		{false,"az"}
	});
	
	return *tr;

}


void CasingParser::add_casing (cps_key cp_key, std::optional<conds_key> cond_key, std::vector<Unicode::CodePoint::Type> cps) {

	casings.Add(
		cp_key,
		Casing{
			cond_key,
			this->cps.Add(
				std::move(cps)
			)
		}
	);

}


std::vector<Unicode::CodePoint::Type> CasingParser::get_casing (const Line & line, std::size_t i) {
	
	auto retr=line[i].CodePoints();
	
	if (!retr) throw std::runtime_error("Expected code points");
	
	return std::move(*retr);

}


void CasingParser::get_casing (const Line & line) {

	//	Lines must have 5 fields:
	//
	//	1.	Code point
	//	2.	Lowercase mapping
	//	3.	Titlecase mapping
	//	4.	Uppercase mapping
	//	5.	Condition list
	if (line.size()<5) throw std::runtime_error("Bad line in SpecialCasing.txt");

	//	Get the code point this line
	//	is associated with
	auto cp=line[0].CodePoint();
	
	//	If there's no code point, that's
	//	an error
	if (!cp) throw std::runtime_error("Line in SpecialCasing.txt does not have code point");
	
	//	Get an entry for this code point
	auto & entry=get(*cp);
	
	//	Create/add to the casing entry
	
	std::optional<conds_key> key;
	auto c=line[4].Conditions();
	if (c.size()!=0) key=conds.Add(std::move(c));
	
	add_casing(
		entry.Lower,
		key,
		get_casing(line,1)
	);
	add_casing(
		entry.Title,
		key,
		get_casing(line,2)
	);
	add_casing(
		entry.Upper,
		key,
		get_casing(line,3)
	);

}


void CasingParser::get_casings () {

	for (auto & line : casing) get_casing(line);

}


void CasingParser::get_folding (Entry & entry, const Item & item, bool turkic) {

	//	Get the code points
	auto mapping=item.CodePoints();
	
	if (!mapping) throw std::runtime_error("Expected code points");

	//	Prepare conditions (none by default, otherwise
	//	turkic languages only)	
	std::optional<conds_key> key;
	if (turkic) key=get_tr();
	
	//	Add
	add_casing(
		entry.Folding,
		key,
		std::move(*mapping)
	);

}


void CasingParser::get_folding (Entry & entry, const Item & item) {

	entry.SimpleFolding=item.CodePoint();
	
	//	If the simple folding is still null,
	//	this line is malformed
	if (!entry.SimpleFolding) throw std::runtime_error("Expected code point");

}


void CasingParser::get_folding (const Line & line) {

	//	Lines must have 3 fields:
	//
	//	1.	Code point
	//	2.	Type of mapping
	//	3.	Mapping
	if (line.size()<3) throw std::runtime_error("Bad line in CaseFolding.txt");
	
	//	Get the code point this line
	//	is associated with
	auto cp=line[0].CodePoint();
	
	//	If there's no code point, that's
	//	an error
	if (!cp) throw std::runtime_error("Line in CaseFolding.txt does not have code point");
	
	//	Get an entry for this code point
	auto & entry=get(*cp);
	
	//	There are two ways to proceed:
	//
	//	1.	This is a simple mapping -- i.e.
	//		one-to-one (i.e. type "S" or "C")
	//	2.	This is a complex mapping -- i.e.
	//		one-to-many (i.e. type "F") or
	//		conditional (i.e. type "T")
	auto & str=line[1].Get();
	if (
		(str=="S") ||
		(str=="C")
	) get_folding(entry,line[2]);
	else if (str=="F") get_folding(entry,line[2],false);
	else if (str=="T") get_folding(entry,line[2],true);
	else throw std::runtime_error("Line in CaseFolding.txt has unrecognized mapping type");

}


void CasingParser::get_foldings () {

	for (auto & line : folding) get_folding(line);

}


void CasingParser::Get () {

	//	Get information from SpecialCasing.txt
	get_casings();
	
	//	Get information from CaseFolding.txt
	get_foldings();

}
