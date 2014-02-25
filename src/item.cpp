#include <unicode/data.hpp>
#include <cctype>
#include <limits>
#include <unordered_map>
#include <utility>


namespace Unicode {


	Item::Item (std::string str) noexcept : str(std::move(str)) {	}
	
	
	static const std::unordered_map<char,Unicode::CodePoint::Type> map={
		{'0',0},
		{'1',1},
		{'2',2},
		{'3',3},
		{'4',4},
		{'5',5},
		{'6',6},
		{'7',7},
		{'8',8},
		{'9',9},
		{'A',10},
		{'B',11},
		{'C',12},
		{'D',13},
		{'E',14},
		{'F',15}
	};
	
	
	static std::optional<Unicode::CodePoint::Type> add_digit (Unicode::CodePoint::Type curr, Unicode::CodePoint::Type digit) noexcept {
	
		std::optional<Unicode::CodePoint::Type> retr;
		
		constexpr Unicode::CodePoint::Type max=std::numeric_limits<Unicode::CodePoint::Type>::max();
		
		//	Checked multiply
		if (
			(curr!=0) &&
			((max/curr)<16)
		) return retr;
		curr*=16;
		
		//	Checked add
		if ((max-curr)<digit) return retr;
		retr=curr+digit;
		
		return retr;
	
	}
	
	
	template <typename T>
	std::optional<Unicode::CodePoint::Type> get_code_point (T & begin, const T & end) noexcept {
	
		std::optional<Unicode::CodePoint::Type> retr;
		
		bool found=false;
		Unicode::CodePoint::Type i=0;
		
		for (;begin!=end;++begin) {
		
			if (std::isspace(*begin)) {
			
				//	End on trailing whitespace
				if (found) break;
		
				//	Skip leading whitespace
				continue;
			
			}
			
			//	See if this character is a hex
			//	digit
			auto iter=map.find(static_cast<char>(std::toupper(*begin)));
			
			if (iter==map.end()) break;
			
			found=true;
			
			//	Try and add this digit in
			auto next=add_digit(i,iter->second);
			if (!next) return retr;
			i=*next;
		
		}
		
		if (found) retr=i;
		
		return retr;
	
	}
	
	
	template <typename T>
	bool is_whitespace (T & begin, const T & end) noexcept {
	
		for (;begin!=end;++begin) if (!std::isspace(*begin)) return false;
		
		return true;
	
	}
	
	
	std::optional<Unicode::CodePoint::Type> Item::CodePoint () const noexcept {
	
		auto begin=this->begin();
		auto end=this->end();
		auto retr=get_code_point(begin,end);
		
		//	Make sure all the trailing characters
		//	(if any) are whitespace
		for (;begin!=end;++begin)
		if (!std::isspace(*begin))
		return std::optional<Unicode::CodePoint::Type>{};
		
		return retr;
	
	}
	
	
	std::optional<std::vector<Unicode::CodePoint::Type>> Item::CodePoints () const {
	
		std::optional<std::vector<Unicode::CodePoint::Type>> retr;
		std::vector<Unicode::CodePoint::Type> vec;
	
		//	1 for just the null terminator
		if (str.size()==1) {
		
			retr.emplace(std::move(vec));
			return retr;
		
		}
		
		auto begin=this->begin();
		auto end=this->end();
		for (;;) {
		
			//	Attempt to get next code point
			auto next=get_code_point(begin,end);
			
			if (next) {
			
				vec.push_back(*next);
				
			} else {
			
				if (is_whitespace(begin,end)) retr.emplace(std::move(vec));
				
				return retr;
			
			}
		
		}
	
	}
	
	
	std::optional<Range> Item::Range () const noexcept {
	
		std::optional<Unicode::Range> retr;
		
		auto begin=this->begin();
		auto end=this->end();
		
		//	Get and check the beginning of the range
		auto first=get_code_point(begin,end);
		if (
			!first ||
			(begin==end) ||
			is_whitespace(begin,end)
		) return retr;
		
		//	Get the range separator
		for (std::size_t i=0;i<2;++i) {
		
			if (*begin=='.') {
			
				++begin;
				continue;
			
			}
			
			if (is_whitespace(begin,end)) return retr;
		
		}
		
		//	Get and check the end of the range
		auto second=get_code_point(begin,end);
		if (!(
			second &&
			(
				(begin==end) ||
				is_whitespace(begin,end)
			)
		)) return retr;
		
		return Unicode::Range{*first,*second};
	
	}
	
	
	template <typename T>
	bool begins_with (const char * str, T & begin, const T & end) noexcept {
	
		T curr=begin;
		for (;*str!='\0';++str,++curr) {
		
			if (
				(curr==end) ||
				(*curr!=*str)
			) return false;
		
		}
		
		begin=curr;
		return true;
	
	}
	
	
	template <typename T>
	std::optional<Condition> get_condition (T & begin, const T & end) {
	
		bool negated=false;
		std::string name;
		
		if (is_whitespace(begin,end)) return std::optional<Condition>{};
		
		if (begins_with("Not_",begin,end)) negated=true;
		
		for (
			;
			!(
				(begin==end) ||
				std::isspace(*begin)
			);
			++begin
		) name.push_back(*begin);
		
		return Condition{negated,std::move(name)};
	
	}
	
	
	std::vector<Condition> Item::Conditions () const {
	
		std::vector<Condition> retr;
		
		auto begin=this->begin();
		auto end=this->end();
		
		for (;;) {
		
			auto cond=get_condition(begin,end);
			
			if (cond) retr.push_back(std::move(*cond));
			else return retr;
		
		}
	
	}
	
	
	const char * Item::begin () const noexcept {
	
		return str.c_str();
	
	}
	
	
	const char * Item::end () const noexcept {
	
		return str.c_str()+str.size();
	
	}


}
