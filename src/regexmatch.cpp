#include <unicode/regex.hpp>
#include <new>
#include <stdexcept>
#include <type_traits>


namespace Unicode {
	
	
	template <typename T>
	void complete (std::unordered_map<T,RegexMatch::Type> & map) {
	
		for (auto & pair : map) for (auto & capture : pair.second) capture.Complete();
	
	}


	void RegexMatch::Complete (const CodePoint * begin, const CodePoint * end) {
	
		b=begin;
		e=end;
		s=String(begin,end);
		
		complete(numbered);
		complete(named);
	
	}
	
	
	String & RegexMatch::Get () noexcept {
	
		return s;
	
	}
	
	
	const String & RegexMatch::Get () const noexcept {
	
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
	
	
	template <typename T>
	const RegexMatch::Type & get (const RegexMatch::Map<T> & map, const T & key) {
	
		auto iter=map.find(key);
		if ((iter==map.end()) || (iter->second.size()==0)) throw std::out_of_range("No such capturing group");
		
		return iter->second;
	
	}
	
	
	const RegexMatch::Type & RegexMatch::operator [] (const String & key) const {
	
		return get(named,key);
	
	}
	
	
	const RegexMatch::Type & RegexMatch::operator [] (std::size_t key) const {
	
		return get(numbered,key);
	
	}
	
	
	const RegexMatch::Map<std::size_t> & RegexMatch::Numbered () const noexcept {
	
		return numbered;
	
	}
	
	
	const RegexMatch::Map<String> & RegexMatch::Named () const noexcept {
	
		return named;
	
	}


}
