#include <unicode/data.hpp>
#include <utility>


namespace Unicode {


	Condition::Condition (bool negated, std::vector<char> name) : name(std::move(name)), Negated(negated) {
	
		//	Make into a C string
		this->name.push_back('\0');
	
	}


}
