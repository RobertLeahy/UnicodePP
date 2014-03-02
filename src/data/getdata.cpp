#include <unicode/data.hpp>
#include <iterator>


#include "data.cpp"


namespace Unicode {


	template <typename T>
	Array<T> make_array (const T * begin, const T * end) noexcept {
	
		return Array<T>(
			begin,
			static_cast<std::size_t>(end-begin)
		);
	
	}


	Array<CodePointInfo> Data () noexcept {
	
		return make_array(
			std::begin(info),
			std::end(info)
		);
	
	}
	
	
	Array<Composition> Compositions () noexcept {
	
		return make_array(
			std::begin(compositions),
			std::end(compositions)
		);
	
	}


}
