#include <unicode/string.hpp>
#include <unicode/utf8.hpp>
#include <algorithm>
#include <cstring>


namespace Unicode {


	void String::trim_front () noexcept {
	
		auto iter=std::find_if(
			cps.begin(),
			cps.end(),
			[] (CodePoint cp) noexcept -> bool {	return !cp.IsWhitespace();	}
		);
		
		auto s=cps.size()-static_cast<std::size_t>(iter-cps.begin());
		
		std::memmove(
			&cps[0],
			&(*iter),
			s
		);
		
		cps.resize(s);
	
	}
	
	
	void String::trim_rear () noexcept {
	
		auto iter=std::find_if(
			cps.rbegin(),
			cps.rend(),
			[] (CodePoint cp) noexcept -> bool {	return !cp.IsWhitespace();	}
		);
		
		cps.resize(
			static_cast<std::size_t>(
				iter-cps.rbegin()
			)
		);
	
	}
	
	
	static std::vector<CodePoint> from_c_string (const char * str) {
	
		return UTF8{}.Decode(
			str,
			str+std::strlen(str)
		).CodePoints();
	
	}
	
	
	String::String (const char * str) : cps(from_c_string(str)) {	}
	
	
	String & String::operator = (const char * str) {
	
		cps=from_c_string(str);
		
		return *this;
	
	}
	
	
	std::vector<char> String::ToCString () const {
	
		auto vec=UTF8{}.Encode(cps);
		vec.push_back('\0');
		
		return vec;
	
	}


	String String::Trim () const {
	
		String cpy=*this;
		
		cpy.Trim();
		
		return cpy;
	
	}
	
	
	String & String::Trim () noexcept {
	
		trim_front();
		trim_rear();
		
		return *this;
	
	}


}
