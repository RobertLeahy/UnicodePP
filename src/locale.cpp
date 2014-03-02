#include <unicode/binarysearch.hpp>
#include <unicode/locale.hpp>
#include <atomic>


namespace Unicode {


	static std::atomic<const Locale *> locale;
	
	
	void Locale::Set (const Locale & locale) noexcept {
	
		Unicode::locale=&locale;
	
	}
	
	
	const Locale & Locale::Get () noexcept {
	
		return *static_cast<const Locale *>(locale);
	
	}
	
	
	const CodePointInfo * Locale::GetInfo (CodePoint cp) const noexcept {
	
		auto end=Info.end();
		auto iter=BinarySearch(
			Info.begin(),
			end,
			cp,
			[] (const CodePointInfo & a, CodePoint b) noexcept {	return a.CodePoint<b;	},
			[] (const CodePointInfo & a, CodePoint b) noexcept {	return a.CodePoint==b;	}
		);
		
		return (iter==end) ? nullptr : iter;
	
	}


}
