#pragma once


#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>


template <typename T, std::size_t i>
const char * FromEnum (const std::pair<T,const char *> (& arr) [i], T val) {

	for (auto & pair : arr) if (val==pair.first) return pair.second;
	
	throw std::runtime_error("Invalid enumeration value");

}


template <typename T, std::size_t i>
T ToEnum (const std::pair<T,const char *> (& arr) [i], const std::string & str) {

	for (auto & pair : arr) if (str==pair.second) return pair.first;
	
	throw std::runtime_error("Invalid enumeration value");

}
