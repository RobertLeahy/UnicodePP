#include <unicode/regex.hpp>
#include <utility>


namespace Unicode {


	RegexCapture::RegexCapture (const CodePoint * begin, const CodePoint * end) noexcept
		:	b(begin), e(end)
	{
	
		if (b>e) std::swap(b,e);
	
	}
	
	
	void RegexCapture::Complete () {
	
		if (!s) s.emplace(b,e);
	
	}
	
	
	String & RegexCapture::Get () {
	
		Complete();
		
		return *s;
	
	}
	
	
	const CodePoint * RegexCapture::begin () const noexcept {
	
		return b;
	
	}
	
	
	const CodePoint * RegexCapture::end () const noexcept {
	
		return e;
	
	}


}
