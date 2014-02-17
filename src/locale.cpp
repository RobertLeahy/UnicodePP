#include <unicode/locale.hpp>
#include <algorithm>
#include <atomic>
#include <stdexcept>


namespace Unicode {


	static std::atomic<const Locale *> locale;
	
	
	void Locale::Set (const Locale & locale) noexcept {
	
		Unicode::locale=&locale;
	
	}
	
	
	const Locale & Locale::Get () noexcept {
	
		return *static_cast<const Locale *>(locale);
	
	}
	
	
	const CodePointInfo * Locale::GetInfo (CodePoint cp) const noexcept {
	
		auto last=Info.end();
		auto iter=std::lower_bound(
			Info.begin(),
			last,
			cp,
			[] (const CodePointInfo & a, CodePoint b) noexcept {	return a.CP<b;	}
		);
		
		return (iter==last) ? nullptr : iter;
	
	}
	
	
	std::optional<CodePoint> Locale::GetComposition (CodePoint starter, CodePoint joiner) const noexcept {
	
		auto p=std::make_pair(starter,joiner);
		auto last=Compositions.end();
		auto iter=std::lower_bound(
			Compositions.begin(),
			last,
			p,
			[] (const Composition & a, const std::pair<CodePoint,CodePoint> & b) noexcept -> bool {
			
				if (a.Starter<b.first) return true;
				if (a.Starter==b.first) return a.Joiner<b.second;
				return false;
			
			}
		);
		
		if (iter==last) return std::optional<CodePoint>{};
		
		return iter->Result;
	
	}


}
