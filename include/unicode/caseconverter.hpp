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
			
			
			std::optional<Array<CodePoint>> get_best (
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				const Array<CaseMapping> &
			) const noexcept;
			
			
			Array<CodePoint> get (
				const CodePoint *,
				const CodePoint *,
				const CodePoint *,
				std::optional<CodePoint::Type> (CodePointInfo::*),
				Array<CaseMapping> (CodePointInfo::*)
			) const noexcept;
			
			
			std::vector<CodePoint> impl (
				const CodePoint *,
				const CodePoint *,
				std::optional<CodePoint::Type> (CodePointInfo::*),
				Array<CaseMapping> (CodePointInfo::*)
			) const;
			
			
			bool folding_normalization_check (CodePoint) const noexcept;
			
			
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
			 *	Strings which contain COMBINING GREEK
			 *	YPOGEGRAMMENI (U+0345) must be transformed
			 *	to Normal Form Canonical Composition before
			 *	being uppercased.
			 *
			 *	This method detects this.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		\em true if the string bounded by \em begin
			 *		and \em end must be normalized before uppercasing.
			 *		\em false otherwise.
			 */
			bool UppercasingRequiresNormalization (const CodePoint * begin, const CodePoint * end) const noexcept;
			/**
			 *	Strings which contain code points which contain
			 *	COMBINING GREEK YPOGEGRAMMENI (U+0345), or any
			 *	code points which contain COMBINING GREEK
			 *	YPOGEGRAMMENI (U+0345) as part of their canonical
			 *	decomposition must be transformed to Normal Form
			 *	Canonical Decomposition before being case folded.
			 *
			 *	This method detects this.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *
			 *	\return
			 *		\em true if the string bounded by \em begin
			 *		and \em end must be normalized before case
			 *		folding.  \em false otherwise.
			 */
			bool CaseFoldingRequiresNormalization (const CodePoint * begin, const CodePoint * end) const noexcept;
			
			
			/**
			 *	Converts a particular character in a particular
			 *	context to lowercase.
			 *
			 *	The returned array may point to \em loc, so it is
			 *	not safe to persist the returned array when \em loc
			 *	is no longer valid.
			 *
			 *	\param [in] loc
			 *		The code point in question.
			 *	\param [in] begin
			 *		An iterator to the beginning of the string which
			 *		contains \em loc.
			 *	\param [in] end
			 *		An iterator to the end of the string which contains
			 *		\em loc.
			 *
			 *	\return
			 *		An array containing the code point or code points
			 *		which are the result of lowercasing the code point
			 *		pointed to by \em loc.
			 */
			Array<CodePoint> ToLower (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept;
			/**
			 *	Converts a particular character in a particular
			 *	context to titlecase.
			 *
			 *	The returned array may point to \em loc, so it is
			 *	not safe to persist the returned array when \em loc
			 *	is no longer valid.
			 *
			 *	\param [in] loc
			 *		The code point in question.
			 *	\param [in] begin
			 *		An iterator to the beginning of the string which
			 *		contains \em loc.
			 *	\param [in] end
			 *		An iterator to the end of the string which contains
			 *		\em loc.
			 *
			 *	\return
			 *		An array containing the code point or code points
			 *		which are the result of titlecasing the code point
			 *		pointed to by \em loc.
			 */
			Array<CodePoint> ToTitle (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept;
			/**
			 *	Converts a particular character in a particular
			 *	context to uppercase.
			 *
			 *	It is assumed that the string bounded by \em begin
			 *	and \em end is in the proper normalization form for
			 *	uppercasing.
			 *
			 *	The returned array may point to \em loc, so it is
			 *	not safe to persist the returned array when \em loc
			 *	is no longer valid.
			 *
			 *	\param [in] loc
			 *		The code point in question.
			 *	\param [in] begin
			 *		An iterator to the beginning of the string which
			 *		contains \em loc.
			 *	\param [in] end
			 *		An iterator to the end of the string which contains
			 *		\em loc.
			 *
			 *	\return
			 *		An array containing the code point or code points
			 *		which are the result of uppercasing the code point
			 *		pointed to by \em loc.
			 */
			Array<CodePoint> ToUpper (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept;
			/**
			 *	Case folds a particular character in a particular
			 *	context.
			 *
			 *	It is assumed that the string bounded by \em begin
			 *	and \em end is in the proper normalization form for
			 *	case folding.
			 *
			 *	The returned array may point to \em loc, so it is
			 *	not safe to persist the returned array when \em loc
			 *	is no longer valid.
			 *
			 *	\param [in] loc
			 *		The code point in question.
			 *	\param [in] begin
			 *		An iterator to the beginning of the string which
			 *		contains \em loc.
			 *	\param [in] end
			 *		An iterator to the end of the string which contains
			 *		\em loc.
			 *
			 *	\return
			 *		An array containing the code point or code points
			 *		which are the result of case folding the code point
			 *		pointed to by \em loc.
			 */
			Array<CodePoint> Fold (const CodePoint * loc, const CodePoint * begin, const CodePoint * end) const noexcept;
			
			
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
