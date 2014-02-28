#include "tointeger.hpp"


namespace ToIntegerImpl {


	static const std::pair<char,unsigned char> map []={
		{'0',0},
		{'1',1},
		{'2',2},
		{'3',3},
		{'4',4},
		{'5',5},
		{'6',6},
		{'7',7},
		{'8',8},
		{'9',9},
		{'A',10},
		{'B',11},
		{'C',12},
		{'D',13},
		{'E',14},
		{'F',15}
	};
	
	
	std::optional<unsigned char> GetDigit (char c) noexcept {
	
		for (auto & pair : map) if (pair.first==c) return pair.second;
		
		return std::optional<unsigned char>{};
	
	}
	
	
}
