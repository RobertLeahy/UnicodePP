/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <optional>
#include <vector>


namespace Unicode {


	/**
	 *	Compares strings for canonical equivalence.
	 */
	class Comparer {
	
	
		private:
		
		
			const Locale & locale;
			bool case_sensitive;
			bool full_mappings;
			bool normalize;
			
			
			std::optional<std::vector<CodePoint>> prepare (const CodePoint *, const CodePoint *, bool nfc=false) const;
			
			
			bool compare (const CodePoint *, const CodePoint *, const CodePoint *, const CodePoint *) const noexcept;
			CodePoint case_fold (char) const noexcept;
			bool compare (const CodePoint *, const CodePoint *, const char *) const noexcept;
			
			
		public:
		
		
			/**
			 *	Creates a new Comparer.
			 *
			 *	\param [in] locale
			 *		The locale to use for normalization and case
			 *		folding operations (if applicable).
			 *	\param [in] case_sensitive
			 *		\em false if the comparer should ignore the
			 *		case of letters when matching, \em true otherwise.
			 *		Defaults to \em true.
			 *	\param [in] normalize
			 *		\em true if the comparer should ensure strings
			 *		are normalized before comparing them, \em false
			 *		otherwise.  Defaults to \em true.
			 */
			Comparer (const Locale & locale=Locale::Get(), bool case_sensitive=true, bool normalize=true) noexcept
				:	locale(locale),
					case_sensitive(case_sensitive),
					full_mappings(true),
					normalize(normalize)
			{	}
			/**
			 *	Creates a new Comparer.
			 *
			 *	The default locale will be used.
			 *
			 *	\param [in] case_sensitive
			 *		\em false if the comparer should ignore the
			 *		case of letters when matching, \em true otherwise.
			 *	\param [in] normalize
			 *		\em true if the comparer should ensure strings
			 *		are normalized before comparing them, \em false
			 *		otherwise.
			 */
			Comparer (bool case_sensitive, bool normalize=true) noexcept
				:	locale(Locale::Get()),
					case_sensitive(case_sensitive),
					full_mappings(true),
					normalize(normalize)
			{	}
			
			
			/**
			 *	Sets whether or not the Comparer will use full case
			 *	folding mappings when comparing strings case
			 *	insensitively.  Is \em true by default.
			 *
			 *	Note that using simple mappings (i.e. setting this
			 *	to \em false) will be faster, but will result in
			 *	lower quality and less reliable results.
			 *
			 *	\param [in] full_mappings
			 *		\em true if this comparer should use full case
			 *		folding mappings, \em false otherwise.
			 */
			void SetFullMappings (bool full_mappings) noexcept {
			
				this->full_mappings=full_mappings;
			
			}
			
			
			/**
			 *	Compares two Unicode strings for canonical equivalence.
			 *
			 *	\param [in] begin_a
			 *		An iterator to the beginning of the first string.
			 *	\param [in] end_a
			 *		An iterator to the end of the first string.
			 *	\param [in] begin_b
			 *		An iterator to the beginning of the second string.
			 *	\param [in] end_b
			 *		An iterator to the end of the second string.
			 *
			 *	\return
			 *		\em true if the first and second string are equivalent
			 *		under the rules of this object, \em false otherwise.
			 */
			bool Compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const;
			/**
			 *	Compares a Unicode string a a C string for canonical
			 *	equivalence.
			 *
			 *	It is assumed that the given C string is ASCII or
			 *	Latin-1, and the case foldings provided by the standard
			 *	library function std::tolower are used in the case of
			 *	case insensitive comparison.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the Unicode string.
			 *	\param [in] end
			 *		An iterator to the end of the Unicode string.
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of ASCII or
			 *		Latin-1 characters.
			 *
			 *	\return
			 *		\em true if the first and second string are equivalent
			 *		under the rules of this object, \em false otherwise.
			 */
			bool Compare (const CodePoint * begin, const CodePoint * end, const char * str) const;
	
	
	};


}
