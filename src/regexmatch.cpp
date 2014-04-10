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
	
	
	template <typename T>
	RegexMatch::Type & emplace (T && key, std::unordered_map<typename std::decay<T>::type,RegexMatch::Type> & map) {
	
		return map.emplace(std::forward<T>(key),RegexMatch::Type{}).first->second;
	
	}
	
	
	template <typename T>
	RegexMatch::Type & retrieve (const T & key, std::unordered_map<T,RegexMatch::Type> & map) {
	
		auto iter=map.find(key);
		return (iter==map.end()) ? emplace(key,map) : iter->second;
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (const String & key) {
	
		return retrieve(key,named);
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (String && key) {
	
		return emplace(std::move(key),named);
	
	}
	
	
	RegexMatch::Type & RegexMatch::operator [] (std::size_t key) {
	
		return retrieve(key,numbered);
	
	}


}
