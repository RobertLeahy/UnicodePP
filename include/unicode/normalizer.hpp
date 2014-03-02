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
	 *	Normalizes Unicode strings.
	 */
	class Normalizer {
	
	
		private:
		
		
			const Locale & locale;
			
			
			QuickCheck is_impl (const CodePoint *, const CodePoint *, QuickCheck (CodePointInfo::*)) const noexcept;
			
			
			void decompose (std::vector<CodePoint> &, CodePoint) const;
			std::vector<CodePoint> decompose (const CodePoint *, const CodePoint *) const;
			
			
			void sort_impl (std::vector<CodePoint>::iterator, std::vector<CodePoint>::iterator) const;
			void sort (std::vector<CodePoint>::iterator, std::vector<CodePoint>::iterator) const;
			
			
			std::optional<CodePoint> compose (CodePoint, CodePoint) const noexcept;
			std::vector<CodePoint> compose (std::vector<CodePoint>) const;
		
		
		public:
		
		
			/**
			 *	Creates a Normalizer.
			 *
			 *	\param [in] locale
			 *		The locale according to which
			 *		the Normalizer should normalize
			 *		text.
			 */
			Normalizer (const Locale & locale=Locale::Get()) noexcept : locale(locale) {	}
			
			
			/**
			 *	Checks if a certain string is in Normal Form
			 *	Canonical Decomposition.
			 *
			 *	Note that this method may return \em false
			 *	for a string that is in NFC, but will never
			 *	return \em true for a string that is not in
			 *	NFD.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		\em true if the string is in NFD, \em false
			 *		otherwise.
			 */
			bool IsNFD (const CodePoint * begin, const CodePoint * end) const noexcept;
			/**
			 *	Checks if a certain string is in Normal Form
			 *	Canonical Composition.
			 *
			 *	Note that this method may return \em false
			 *	for a string that is in NFC, but will never
			 *	return \em true for a string that is not in
			 *	NFC.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		\em true if the string is in NFC, \em false
			 *		otherwise.
			 */
			bool IsNFC (const CodePoint * begin, const CodePoint * end) const noexcept;
			
			
			/**
			 *	Converts a string to Normal Form Canonical Decomposition.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		The string in NFD.
			 */
			std::vector<CodePoint> ToNFD (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Converts a string to Normal Form Canonical Decomposition.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		The string in NFD.
			 */
			std::vector<CodePoint> ToNFC (const CodePoint * begin, const CodePoint * end) const;
	
	
	};


}
 