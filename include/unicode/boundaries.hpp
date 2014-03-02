/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>


namespace Unicode {


	/**
	 *	Determines if a grapheme cluster boundary
	 *	occurs immediately before a certain code
	 *	point.
	 *
	 *	Uses the rules specified in Unicode Standard
	 *	Annex #29 (Unicode Text Segmentation).
	 *
	 *	\param [in] loc
	 *		The current location within the string.
	 *	\param [in] begin
	 *		An iterator to the beginning of the string.
	 *	\param [in] end
	 *		An iterator to the end of the string.
	 *	\param [in] locale
	 *		The locale to use.  Optional.  Defaults to
	 *		the current locale.
	 *
	 *	\return
	 *		\em true if a grapheme cluster boundary
	 *		occurs immediately before \em loc,
	 *		\em false otherwise.
	 */
	bool IsGraphemeClusterBoundary (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale=Locale::Get()) noexcept;
	
	
	/**
	 *	Determines if a word boundary occurs
	 *	immediately before a certain code point.
	 *
	 *	Uses the rules specified in Unicode Standard
	 *	Annex #29 (Unicode Text Segmentation).
	 *
	 *	\param [in] loc
	 *		The current location within the string.
	 *	\param [in] begin
	 *		An iterator to the beginning of the string.
	 *	\param [in] end
	 *		An iterator to the end of the string.
	 *	\param [in] locale
	 *		The locale to use.  Optional.  Defaults to
	 *		the current locale.
	 *
	 *	\return
	 *		\em true if a grapheme cluster boundary
	 *		occurs immediately before \em loc,
	 *		\em false otherwise.
	 */
	bool IsWordBoundary (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale=Locale::Get()) noexcept;


}
