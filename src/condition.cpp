#include <unicode/codepoint.hpp>
#include <unicode/conditions.hpp>
#include <unicode/locale.hpp>
#include <cstring>
#include <utility>


namespace Unicode {


	static bool cmp (const char * a, const char * b) noexcept {
	
		return std::strcmp(a,b)==0;
	
	}
	
	
	static const std::pair<
		const char *,
		bool (*) (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept
	> conditions []={
		{"Final_Sigma",&Conditions::FinalSigma},
		{"After_Soft_Dotted",&Conditions::AfterSoftDotted},
		{"More_Above",&Conditions::MoreAbove},
		{"Before_Dot",&Conditions::BeforeDot},
		{"After_I",&Conditions::AfterI}
	};
	
	
	static bool retr (bool negated, bool val) noexcept {
	
		return negated ? !val : val;
	
	}


	bool Condition::operator () (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) const noexcept {
	
		//	Check to see if the Unicode condition
		//	has a function which implements it
		for (auto & pair : conditions) {
		
			if (cmp(pair.first,cond)) return retr(
				negated,
				pair.second(
					loc,
					begin,
					end,
					locale
				)
			);
		
		}
		
		//	The Unicode condition does not have a
		//	function which implements it, fall
		//	back to checking the language of this
		//	locale
		return retr(
			negated,
			(locale.Language==nullptr) ? false : cmp(cond,locale.Language)
		);	
	}


}
