#include "ducet.hpp"
#include <unicode/binarysearch.hpp>
#include <unicode/data.hpp>
#include <algorithm>
#include <stdexcept>
#include <utility>


using namespace Unicode;


DUCETParser::DUCETParser (const std::string & in, const std::string & out)
	:	keys(in.c_str()),
		out(out)
{	}


[[noreturn]]
static void bad_line () {

	throw std::runtime_error("Bad line");

}


static const CodePointInfo * get (CodePoint::Type cp) noexcept {

	auto arr=Data();
	auto end=arr.end();

	auto iter=BinarySearch(
		arr.begin(),
		end,
		cp,
		[] (const CodePointInfo & cpi, CodePoint::Type cp) noexcept {	return cpi.CodePoint<cp;	},
		[] (const CodePointInfo & cpi, CodePoint::Type cp) noexcept {	return cpi.CodePoint==cp;	}
	);
	
	return (iter==end) ? nullptr : iter;

}


static bool check (const std::vector<CodePoint::Type> & vec) noexcept {

	//	If any code point in the sequence has
	//	a decomposition, the sequenc is unnecessary
	//	because code points that decompose will
	//	never be looked up in the table under a
	//	conformant implementation
	
	for (auto cp : vec) {
	
		auto cpi=get(cp);
		if (cpi==nullptr) continue;
		
		if (cpi->DecompositionMapping.Size!=0) return false;
	
	}
	
	return true;

}


void DUCETParser::process (const Line & line) {

	//	Each line in the file must have
	//	at least two items:
	//
	//	1.	The code point(s)
	//	2.	The collation element(s)
	if (line.size()<2) bad_line();
	
	//	Get the code points
	auto cps=line[0].CodePoints();
	if (!cps) bad_line();
	
	//	Check to make sure there are no
	//	unneeded code points in this sequence
	//	of code points
	if (!check(*cps)) return;
	
	//	Get the collation elements
	auto ces=line[1].CollationElements();
	if (ces.size()==0) bad_line();
	
	std::vector<CollationElement> vec;
	for (auto & info : ces) vec.push_back(CollationElement{
		info.Variable,
		weights.Add(std::move(info.Weights))
	});
	
	entries.push_back(
		Entry{
			this->cps.Add(std::move(*cps)),
			elements.Add(std::move(vec))
		}
	);

}


static bool compare (const std::vector<CodePoint::Type> & a, const std::vector<CodePoint::Type> & b) noexcept {

	auto a_b=a.begin();
	auto a_e=a.end();
	auto b_b=b.begin();
	auto b_e=b.end();
	for (;!((a_b==a_e) || (b_b==b_e));++a_b,++b_b) {
	
		if (*a_b<*b_b) return true;
		if (*a_b>*b_b) return false;
	
	}
	
	return (a_b==a_e) && (b_b!=b_e);

}


void DUCETParser::sort () {

	std::sort(
		entries.begin(),
		entries.end(),
		[&] (const Entry & a, const Entry & b) noexcept {
		
			return compare(cps[a.CodePoints],cps[b.CodePoints]);
		
		}
	);

}


void DUCETParser::output (const ::Array & arr, const std::string & str) {

	out << "{";
	if (arr.Size!=0) out << "&" << str << "[" << arr.Offset << "]," << arr.Size;
	out << "}";

}


void DUCETParser::output_cps () {

	out.BeginArray("CodePoint::Type","cps");
	
	bool first=true;
	for (auto cp : cps) {
	
		if (first) first=false;
		else out << ",";
		
		out << "0x" << std::hex << cp << std::dec;
	
	}
	
	out.EndArray();

}


void DUCETParser::output_weights () {

	out.BeginArray("std::uint16_t","weights");
	
	bool first=true;
	for (auto w : weights) {
	
		if (first) first=false;
		else out << ",";
		
		out << "0x" << std::hex << w << std::dec;
	
	}
	
	out.EndArray();

}


void DUCETParser::output_elements () {

	out.BeginArray("CollationElement","elements");
	
	bool first=true;
	for (auto e : elements) {
	
		if (first) first=false;
		else out << ",";
		
		out << "{" << (e.Variable ? "true" : "false") << ",";
		
		output(weights.Get(e.Weights),"weights");
		
		out << "}";
	
	}
	
	out.EndArray();

}


void DUCETParser::output_table () {

	out.BeginArray("CollationTableEntry","table");
	out.BeginIndent();
	
	bool first=true;
	for (auto & entry : entries) {
	
		if (first) first=false;
		else out << "," << Newline;
		
		out << "{";
		
		output(cps.Get(entry.CodePoints),"cps");
		out << ",";
		output(elements.Get(entry.CollationElements),"elements");
		
		out << "}";
	
	}
	
	out.EndIndent();
	out.EndArray();

}


void DUCETParser::Get () {

	for (auto & line : keys) process(line);
	
	sort();

}


void DUCETParser::Output () {

	//	Required headers
	out.Header("unicode/codepoint.hpp");
	out.Header("cstdint");
	
	//	Begin the Unicode namespace
	out.WhiteSpace();
	out.BeginNamespace("Unicode");
	
	//	Output helper arrays
	output_cps();
	out.WhiteSpace();
	output_weights();
	out.WhiteSpace();
	output_elements();
	
	out.WhiteSpace();
	
	//	Output table
	output_table();
	
	//	Done
	out.EndNamespace();
	out.End();

}
