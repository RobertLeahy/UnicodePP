#include "codeoutput.hpp"


const NewlineT Newline;


CodeOutput::CodeOutput (const std::string & file) : out(file.c_str(),std::ios::out|std::ios::trunc), indent(0), line(false) {	}


void CodeOutput::BeginIndent () {

	++indent;
	
	*this << Newline;

}


void CodeOutput::EndIndent () {

	if (indent!=0) --indent;
	
	*this << Newline;

}


void CodeOutput::WhiteSpace () {

	if (line) *this << Newline;

	*this << Newline << Newline;

}


void CodeOutput::Header (const std::string & header, bool local) {

	*this << "#include " << (local ? "\"" : "<") << header << (local ? "\"" : ">") << Newline;

}


void CodeOutput::End () {

	if (line) out << std::endl;
	
	out << std::flush;

}


void CodeOutput::WriteString (const std::string & str) {

	*this << "\"";
	for (auto c : str) switch (c) {
	
		case '\a':
			*this << "\\a";
			break;
		case '\b':
			*this << "\\b";
			break;
		case '\f':
			*this << "\\f";
			break;
		case '\n':
			*this << "\\n";
			break;
		case '\r':
			*this << "\\r";
			break;
		case '\t':
			*this << "\\t";
			break;
		case '\v':
			*this << "\\v";
			break;
		case '\'':
			*this << "\\'";
			break;
		case '"':
			*this << "\\\"";
			break;
		default:
			*this << c;
			break;
	
	}
	*this << "\"";

}


void CodeOutput::BeginNamespace (const std::string & ns) {

	*this << "namespace " << ns << " {";
	BeginIndent();
	WhiteSpace();

}


void CodeOutput::EndNamespace () {

	EndIndent();
	WhiteSpace();
	*this << "}";

}


void CodeOutput::BeginArray (const std::string & type, const std::string & name) {

	if (line) WhiteSpace();
	
	*this << "static const " << type << " " << name << " []={";

}


void CodeOutput::EndArray () {

	*this << "};";

}


CodeOutput & CodeOutput::operator << (const NewlineT &) {

	line=false;

	out << std::endl;
	
	for (std::size_t i=0;i<indent;++i) out << "\t";
	
	return *this;

}
