#include <unicode/ucd.hpp>
#include <algorithm>
#include <cctype>
#include <utility>


namespace Unicode {


	Line::Line (std::vector<Item> items) noexcept : items(std::move(items)) {	}
	
	
	static bool is_not_whitespace (char c) noexcept {
	
		return !std::isspace(c);
	
	}
	
	
	static void trim_front (std::string & str) {
	
		str.erase(
			str.begin(),
			std::find_if(
				str.begin(),
				str.end(),
				is_not_whitespace
			)
		);
	
	}
	
	
	static void trim_back (std::string & str) {
	
		auto begin=str.rbegin();
		auto iter=std::find_if(
			begin,
			str.rend(),
			is_not_whitespace
		);
		
		str.resize(
			static_cast<std::size_t>(
				str.size()-(iter-begin)
			)
		);
	
	}
	
	
	static void trim (std::string & str) {
	
		trim_back(str);
		trim_front(str);
	
	}
	
	
	std::optional<Line> Line::Get (std::string str) {
	
		std::optional<Line> retr;
		
		trim(str);

		//	Ignore empty lines and lines
		//	that are just comments
		if (
			(str.size()==0) ||
			(str[0]=='#')
		) return retr;
		
		//	Loop and extract each semicolon-delimited
		//	item
		//
		//	If a hash is found, that means the rest of
		//	the line is a comment and ought to be ignored
		std::vector<Item> items;
		std::string item;
		for (auto c : str) switch (c) {
		
			case ';':
				trim(item);
				items.push_back(std::move(item));
				item=std::string{};
				break;
				
			case '#':
				goto done;
				
			default:
				item.push_back(c);
				break;
		
		}
		
		done:
		
		trim(item);
		items.push_back(std::move(item));
		
		retr.emplace(std::move(items));
		return retr;
	
	}


}
