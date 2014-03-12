#include <unicode/regex.hpp>


namespace Unicode {


	RegexPatternElement::RegexPatternElement (RegexOptions options, const Unicode::Locale & locale) noexcept
		:	Options(options),
			Locale(locale)
	{	}


	//	Implementation of virtual destructor
	RegexPatternElement::~RegexPatternElement () noexcept {	}


}
