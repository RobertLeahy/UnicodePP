#include <unicode/data.hpp>
#include <cstddef>
#include <iterator>


#include "data.cpp"


namespace Unicode {


	template <typename T, std::size_t i>
	Array<T> make_array (const T (& arr) [i]) noexcept {
	
		return Array<T>(arr,i);
	
	}


	Array<CodePointInfo> Data () noexcept {
	
		return make_array(info);
	
	}
	
	
	Array<Composition> Compositions () noexcept {
	
		return make_array(compositions);
	
	}


}
