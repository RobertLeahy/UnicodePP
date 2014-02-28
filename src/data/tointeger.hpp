#pragma once


#include <cctype>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>


namespace ToIntegerImpl {


	std::optional<unsigned char> GetDigit (char) noexcept;
	
	
	template <typename T>
	std::optional<T> AddDigit (char digit, T curr, std::size_t base) noexcept {
		
		if ((std::numeric_limits<T>::max()/base)<curr) return std::optional<T>{};
		
		curr*=base;
		
		auto val=GetDigit(digit);
		if (!val) return std::optional<T>{};
		T v=static_cast<T>(*val);
		
		if ((std::numeric_limits<T>::max()-curr)<v) return std::optional<T>{};
		
		return curr+v;
	
	}


}


template <typename T>
std::optional<T> ToInteger (const std::string & str, std::size_t base=10) {

	if ((base<1) || (base>16)) throw std::out_of_range("Invalid base");

	T retr=0;
	bool started=false;
	bool finished=false;
	bool negative=false;
	for (auto & c : str) {
	
		if (std::isspace(c)) {
		
			if (started) finished=true;
			
			continue;
		
		}
		
		if (c=='-') {
		
			if (
				started ||
				negative ||
				std::is_unsigned<T>::value
			) return std::optional<T>{};
			
			negative=true;
		
		}
		
		if (finished) return std::optional<T>{};
		
		auto next=ToIntegerImpl::AddDigit(c,retr,base);
		if (!next) return std::optional<T>{};
		
		retr=*next;
	
	}
	
	if (negative) retr*=-1;
	
	return retr;

}
