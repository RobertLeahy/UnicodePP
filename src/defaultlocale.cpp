#include <unicode/locale.hpp>
#include "compositions.cpp"
#include "data.cpp"


namespace Unicode {


	static Locale build_default_locale ();


	const Locale DefaultLocale=build_default_locale();
	
	
	static Locale build_default_locale () {
	
		Locale::Set(DefaultLocale);
		
		Locale retr;
		retr.Info=Locale::Make(map);
		retr.Compositions=Locale::Make(compositions);
		
		return retr;
	
	}


}
