#include <unicode/endianencoding.hpp>


namespace Unicode {


	EndianEncoding::EndianEncoding (Unicode::Endianness output_order, std::optional<Unicode::Endianness> input_order) noexcept
		:	OutputOrder(output_order),
			InputOrder(input_order)
	{	}
	
	
	void EndianEncoding::GetBOM (std::vector<unsigned char> & buffer) const {
	
		BOM().Get(buffer,OutputOrder);
	
	}


}
