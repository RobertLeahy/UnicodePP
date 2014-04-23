#include <unicode/regex.hpp>
#include <utility>


namespace Unicode {


	RegexCapture::RegexCapture (const CodePoint * begin, const CodePoint * end)
		:	b(begin), e(end), completed(false)
	{
	
		if (b>e) std::swap(b,e);
	
	}
	
	
	void RegexCapture::Complete () {
	
		if (!completed) {
		
			s=String(b,e);
			
			completed=true;
			
		}
	
	}
	
	
	String & RegexCapture::Get () {
	
		Complete();
		
		return s;
	
	}
	
	
	const String & RegexCapture::Get () const noexcept {
	
		return s;
	
	}
	
	
	const CodePoint * RegexCapture::begin () const noexcept {
	
		return b;
	
	}
	
	
	const CodePoint * RegexCapture::end () const noexcept {
	
		return e;
	
	}


}
