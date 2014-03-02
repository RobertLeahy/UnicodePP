#include <unicode/data.hpp>
#include <unicode/locale.hpp>


namespace Unicode {


	static Locale build_default_locale () noexcept {
	
		Locale::Set(DefaultLocale);
		
		Locale retr;
		retr.Info=Data();
		retr.Compositions=Compositions();
		
		return retr;
	
	}


	const Locale DefaultLocale=build_default_locale();


}
