#include <unicode/data.hpp>
#include <unicode/locale.hpp>


namespace Unicode {


	static const CodePoint digit_map []={
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'A',
		'B',
		'C',
		'D',
		'E',
		'F'
	};


	static Locale build_default_locale () noexcept {
	
		Locale::Set(DefaultLocale);
		
		Locale retr;
		retr.Info=Data();
		retr.Compositions=Compositions();
		retr.CollationTable=CollationTable();
		retr.Digits=decltype(retr.Digits)(digit_map);
		retr.Negative='-';
		
		return retr;
	
	}


	const Locale DefaultLocale=build_default_locale();


}
