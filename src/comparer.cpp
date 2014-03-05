#include <unicode/caseconverter.hpp>
#include <unicode/comparer.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/vector.hpp>
#include <algorithm>
#include <cctype>


namespace Unicode {


	std::optional<std::vector<CodePoint>> Comparer::prepare (const CodePoint * b, const CodePoint * e, bool nfc) const {
	
		std::optional<std::vector<CodePoint>> retr;
		
		if (!case_sensitive) {
		
			CaseConverter c(locale,full_mappings);
			retr=c.Fold(b,e);
			b=Begin(*retr);
			e=End(*retr);
		
		}
		
		if (normalize) {
		
			Normalizer n(locale);
			if (nfc) {
			
				if (!n.IsNFC(b,e)) retr=n.ToNFC(b,e);
			
			} else if (!n.IsNFD(b,e)) {
			
				retr=n.ToNFD(b,e);
			
			}
		
		}
		
		return retr;
	
	}
	
	
	bool Comparer::compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const noexcept {
	
		//	This can be eliminated come C++14 and the new
		//	overloads of std::equal
		if ((end_a-begin_a)!=(end_b-begin_b)) return false;
		
		return std::equal(begin_a,end_a,begin_b);
	
	}
	
	
	CodePoint Comparer::case_fold (char c) const noexcept {
	
		if (case_sensitive) return c;
		
		return static_cast<CodePoint::Type>(std::tolower(c));
	
	}
	
	
	bool Comparer::compare (const CodePoint * begin, const CodePoint * end, const char * str) const noexcept {
	
		for (;!((begin==end) || (*str=='\0'));++begin,++str) {
		
			if (*begin!=case_fold(*str)) return false;
		
		}
		
		return (begin==end) && (*str=='\0');
	
	}


	bool Comparer::Compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const {
	
		//	If one string is empty, the other must be too
		if ((end_a-begin_a)==0) return (end_b-begin_b)==0;
		else if ((end_b-begin_b)==0) return false;
	
		//	Prepare the strings for comparison
		auto a=prepare(begin_a,end_a);
		auto b=prepare(begin_b,end_b);
		
		//	Compare the strings
		return compare(
			Begin(begin_a,a),
			End(end_a,a),
			Begin(begin_b,b),
			End(end_b,b)
		);
	
	}
	
	
	bool Comparer::Compare (const CodePoint * begin, const CodePoint * end, const char * str) const {
		
		//	Avoid SIGSEGV
		if ((str==nullptr) || (*str=='\0')) return (end-begin)==0;
	
		//	Prepare Unicode string for comparison
		auto a=prepare(begin,end,true);
		
		//	Compare the strings
		return compare(
			Begin(begin,a),
			End(end,a),
			str
		);
	
	}


}
