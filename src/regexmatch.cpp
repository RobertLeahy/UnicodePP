#include <unicode/regex.hpp>
#include <new>
#include <type_traits>


namespace Unicode {


	void RegexMatch::Complete (const CodePoint * begin, const CodePoint * end) {
	
		b=begin;
		e=end;
		s=String(begin,end);
	
	}
	
	
	String & RegexMatch::Get () noexcept {
	
		return s;
	
	}
	
	
	const CodePoint * RegexMatch::begin () const noexcept {
	
		return b;
	
	}
	
	
	const CodePoint * RegexMatch::end () const noexcept {
	
		return e;
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (const String & key) {
	
		return named[key];
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (String && key) {
	
		return named[std::move(key)];
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (std::size_t key) {
	
		return numbered[key];
	
	}


}
