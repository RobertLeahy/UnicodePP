#pragma once


#include <sstream>
#include <string>
#include <utility>


inline std::string Join () {

	return std::string{};

}


template <typename... Args>
std::string Join (const std::string & str, Args &&... args) {

	std::stringstream s;

	if (str.size()!=0) {
	
		auto begin=str.begin();
		auto end=str.end();
		
		if (
			(*begin=='/') ||
			(*begin=='\\')
		) ++begin;
		
		if (begin!=end) {
		
			--end;
			
			if (
				(begin==end) ||
				!(
					(*end=='/') ||
					(*end=='\\')
				)
			) ++end;
		
		}
		
		s << str.substr(
			begin-str.begin(),
			end-begin
		);
	
	}
	
	auto next=Join(std::forward<Args>(args)...);
	if (next.size()!=0) s << "/" << next;
	
	return s.str();

}
