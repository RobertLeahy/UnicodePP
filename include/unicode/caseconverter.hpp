/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <vector>


namespace Unicode {


	/**
	 *	Performs both full and simple case conversion
	 *	and folding.
	 */
	class CaseConverter {
	
	
		private:
		
		
			const Locale & locale;
			
			
			bool matches (
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				const Array<Condition> &
			) const noexcept;
			
			
			const Array<CodePoint::Type> * get_best (
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				const Array<CaseMapping> &
			) const noexcept;
			
			
			bool get_full (
				std::vector<CodePoint> &,
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				const Array<CaseMapping> &
			) const;
			
			
			void impl (
				std::vector<CodePoint> &,
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				Array<CaseMapping> (CodePointInfo::*),
				std::optional<CodePoint::Type> (CodePointInfo::*)
			) const;
			
			
			std::vector<CodePoint> impl (
				const CodePoint *,
				const CodePoint *,
				Array<CaseMapping> (CodePointInfo::*),
				std::optional<CodePoint::Type> (CodePointInfo::*)
			) const;
			
			
			bool folding_normalization_check (CodePoint) const noexcept;
			
			
			bool folding_requires_normalization (const CodePoint *, const CodePoint *) const noexcept;
			bool uppercasing_requires_normalization (const CodePoint *, const CodePoint *) const noexcept;
			
			
		public:
		
		
			/**
			 *	If \em true the CaseConverter will perform
			 *	full case conversions and foldings, otherwise
			 *	simple case conversions and foldings will be
			 *	performed.
			 *
			 *	Full case conversions and foldings yield better
			 *	results, but are more time consuming.
			 */
			bool Full;
		
		
			/**
			 *	Creates a new CaseConverter.
			 *
			 *	\param [in] full
			 *		The value to which to initialize Full.
			 *		Defaults to \em true.
			 */
			explicit CaseConverter (bool full) noexcept : locale(Locale::Get()), Full(full) {	}
			/**
			 *	Creates a new CaseConverter.
			 *
			 *	\param [in] locale
			 *		The locale based on which to perform
			 *		case conversions.  Optional.  Defaults
			 *		to the current locale.
			 *	\param [in] full
			 *		The value to which to initialize Full.
			 *		Defaults to \em true.
			 */
			explicit CaseConverter (const Locale & locale=Locale::Get(), bool full=true) noexcept : locale(locale), Full(full) {	}
			
			
			/**
			 *	Converts a range of code points to lower case.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of code points.
			 *	\param [in] end
			 *		An iterator to the end of the range of
			 *		code points.
			 *
			 *	\return
			 *		A vector containing the converted range.
			 */
			std::vector<CodePoint> ToLower (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Converts a range of code points to title case.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of code points.
			 *	\param [in] end
			 *		An iterator to the end of the range of
			 *		code points.
			 *
			 *	\return
			 *		A vector containing the converted range.
			 */
			std::vector<CodePoint> ToTitle (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Converts a range of code points to upper case.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of code points.
			 *	\param [in] end
			 *		An iterator to the end of the range of
			 *		code points.
			 *
			 *	\return
			 *		A vector containing the converted range.
			 */
			std::vector<CodePoint> ToUpper (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Performs case folding on a range of code points.
			 *
			 *	After case folding, case differences are eliminated.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of code points.
			 *	\param [in] end
			 *		An iterator to the end of the range of
			 *		code points.
			 *
			 *	\return
			 *		A vector containing the folded range.
			 */
			std::vector<CodePoint> Fold (const CodePoint * begin, const CodePoint * end) const;
	
	
	};


}
