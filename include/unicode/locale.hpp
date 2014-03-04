/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>


namespace Unicode {


	/**
	 *	Represents a locale, a certain set of rules
	 *	as to how Unicode code points should be
	 *	interpreted and treated.
	 */
	class Locale {
	
	
		public:
		
		
			/**
			 *	Sets the current locale.
			 *
			 *	This method is thread safe.
			 *
			 *	After calling this method \em locale
			 *	must persist until program termination.
			 *
			 *	\param [in] locale
			 *		A reference to the locale to set.
			 */
			static void Set (const Locale & locale) noexcept;
			/**
			 *	Retrieves the current locale.
			 *
			 *	\return
			 *		A reference to the current locale.
			 */
			static const Locale & Get () noexcept;
			
			
			/**
			 *	The language of this locale.
			 */
			const char * Language;
			/**
			 *	An array of CodePointInfo structures that
			 *	given information about code points for
			 *	this locale.
			 *
			 *	This array must be in ascending sorted order
			 *	on code point numeric value.
			 */
			Array<CodePointInfo> Info;
			/**
			 *	An array of Composition structures that
			 *	give information about how code points
			 *	should be composed for Normal Form
			 *	Canonical Composition.
			 *
			 *	This array must be in ascending sorted order.
			 *	The ordering is lexicographic on the composition
			 *	(the code point to which a composition composes
			 *	is ignored for the purposes of sorting).
			 */
			Array<Composition> Compositions;
			/**
			 *	An array of CollationElementTableEntry objects
			 *	which give the collation element table for this
			 *	locale.
			 *
			 *	This array must be in ascending sorted order.
			 *	The ordering is lexicographic on the code points.
			 */
			Array<CollationElementTableEntry> Collation;
			
			
			constexpr Locale () noexcept : Language(nullptr) {	}
		
		
			/**
			 *	Retrieves information about a certain code point
			 *	from the information available in this locale.
			 *
			 *	\param [in] cp
			 *		The code point about which information shall
			 *		be retrieved.
			 *
			 *	\return
			 *		A pointer to a CodePointInfo structure if one
			 *		exists, \em nullptr otherwise.
			 */
			const CodePointInfo * GetInfo (CodePoint cp) const noexcept;
	
	
	};
	
	
	/**
	 *	The default locale.
	 */
	extern const Locale DefaultLocale;


}
