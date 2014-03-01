/**
 *	\file
 */
 
 
#pragma once


#include <cstddef>
#include <cstdint>
#include <optional>


namespace Unicode {


	/**
	 *	\cond
	 */


	class CodePointInfo;
	class Locale;
	
	
	/**
	 *	\endcond
	 */


	/**
	 *	A single Unicode code point.
	 */
	class CodePoint {
	
	
		public:
		
		
			/**
			 *	An integer type which may represent
			 *	all Unicode code points.
			 */
			typedef std::uint32_t Type;
	
	
		private:
		
		
			Type cp;
			
			
			static const Locale & get_locale () noexcept;
			
			
		public:
		
		
			/**
			 *	The largest valid Unicode code point
			 *	value.
			 */
			static constexpr Type Max=0x10FFFFU;
		
		
			CodePoint () = default;
			CodePoint (const CodePoint &) = default;
			CodePoint (CodePoint &&) = default;
			CodePoint & operator = (const CodePoint &) = default;
			CodePoint & operator = (CodePoint &&) = default;
		
		
			/**
			 *	Creates a code point from an integer.
			 *
			 *	\param [in] cp
			 *		An integer.
			 */
			CodePoint (Type cp) noexcept : cp(cp) {	}
			
			
			/**
			 *	Assigns to this code point from an
			 *	integer.
			 *
			 *	\param [in] cp
			 *		An integer.
			 *
			 *	\return
			 *		A reference to this code point.
			 */
			CodePoint & operator = (Type cp) noexcept {
			
				this->cp=cp;
				
				return *this;
			
			}
			
			
			/**
			 *	Retrieves an integer representing this
			 *	code point.
			 *
			 *	\return
			 *		An integer.
			 */
			operator Type () const noexcept {
			
				return cp;
			
			}
			
			
			/**
			 *	Determines if this code point is valid
			 *	Unicode.
			 *
			 *	\return
			 *		\em true if this code point is valid,
			 *		\em false otherwise.
			 */
			bool IsValid () const noexcept;
			
			
			/** 
			 *	Retrieves a pointer to a CodePointInfo
			 *	structure containing information about this
			 *	code point.
			 *
			 *	\param [in] locale
			 *		The locale from which to get information
			 *		about this code point.  Optional.  Defaults
			 *		to the current locale.
			 *
			 *	\return
			 *		A pointer to a CodePointInfo structure
			 *		containing information about this code
			 *		point if one could be found, \em nullptr
			 *		otherwise.
			 */
			const CodePointInfo * GetInfo (const Locale & locale=get_locale()) const noexcept;
			
			
			/**
			 *	Determines whether this code point is
			 *	whitespace.
			 *
			 *	A code point is whitespace if it is a
			 *	member of one of the Unicode general
			 *	categories Zs, Zl, or Zp.
			 *
			 *	\param [in] locale
			 *		The locale from which to get information
			 *		about this code point.  Optional.  Defaults
			 *		to the current locale.
			 *	
			 *	\return
			 *		\em true if this code point is
			 *		whitespace, \em false otherwise.
			 */
			bool IsWhitespace (const Locale & locale=get_locale()) const noexcept;
			
			
			bool IsBreak (CodePoint next, const Locale & locale=get_locale()) const noexcept;
			
			
			std::optional<CodePoint> DoesCompose (CodePoint cp, const Locale & locale=get_locale()) const noexcept;
			
			
			bool operator == (char c) const noexcept {
			
				union {
					char in;
					unsigned char out;
				};
				in=c;
				
				return cp==static_cast<Type>(out);
			
			}
			
			
			bool operator == (Type t) const noexcept {
			
				return cp==t;
			
			}
	
	
	};
	
	
	template <typename T>
	class Array {
	
	
		public:
		
		
			const T * Data;
			std::size_t Size;
			
			
			constexpr Array () noexcept : Data(nullptr), Size(0) {	}
			constexpr Array (const T * data, std::size_t size) noexcept : Data(data), Size(size) {	}
			
			
			Array (const Array &) = default;
			Array (Array &&) = default;
			Array & operator = (const Array &) = default;
			Array & operator = (Array &&) = default;
			
			
			const T * begin () const noexcept {
			
				return Data;
			
			}
			
			
			const T * end () const noexcept {
			
				return Data+Size;
			
			}
			
			
			std::size_t size () const noexcept {
			
				return Size;
			
			}
			
			
			const T & operator [] (std::size_t i) const noexcept {
			
				return Data[i];
			
			}
	
	
	};
	
	
	/**
	 *	Possible general categories for a Unicode code point.
	 */
	enum class GeneralCategory {

		Lu,	/**<	Letter, Uppercase	*/
		Ll,	/**<	Letter, Lowercase	*/
		Lt,	/**<	Letter, Titlecase	*/
		Mn,	/**<	Mark, Non-Spacing	*/
		Mc,	/**<	Mark, Spacing Combining	*/
		Me,	/**<	Mark, Enclosing	*/
		Nd,	/**<	Number, Decimal Digit	*/
		Nl,	/**<	Number, Letter	*/
		No,	/**<	Number, Other	*/
		Zs,	/**<	Separator, Space	*/
		Zl,	/**<	Separator, Line	*/
		Zp,	/**<	Separator, Paragraph	*/
		Cc,	/**<	Other, Control	*/
		Cf,	/**<	Other, Format	*/
		Cs,	/**<	Other, Surrogate	*/
		Co,	/**<	Other, Private Use	*/
		Cn,	/**<	Other, Not Assigned	*/
		Lm,	/**<	Letter, Modifier	*/
		Lo,	/**<	Letter, Other	*/
		Pc,	/**<	Punctuation, Connector	*/
		Pd,	/**<	Punctuation, Dash	*/
		Ps,	/**<	Punctuation, Open	*/
		Pe,	/**<	Punctuation, Close	*/
		Pi,	/**<	Punctuation, Initial quote (may behave like GeneralCategory::Ps or GeneralCategory::Pe depending on usage)	*/
		Pf,	/**<	Punctuation, Final quote (may behave like GeneralCategory::Ps or GeneralCategory:Pe depending on usage)	*/
		Po,	/**<	Punctuation, Other	*/
		Sm,	/**<	Symbol, Math	*/
		Sc,	/**<	Symbol, Currency	*/
		Sk,	/**<	Symbol, Modifier	*/
		So	/**<	Symbol, Other	*/

	};


	/**
	 *	Possible bidirectional character types for a
	 *	Unicode code point.
	 */
	enum class BidirectionalClass {
	
		L,
		R,
		AL,
		EN,
		ES,
		ET,
		AN,
		CS,
		NSM,
		BN,
		B,
		S,
		WS,
		ON,
		LRE,
		LRO,
		RLE,
		RLO,
		PDF,
		LRI,
		RLI,
		FSI,
		PDI

	};
	
	
	/**
	 *	Determines grapheme cluster boundaries.
	 */
	enum class GraphemeClusterBreak {
	
		CR,
		LF,
		Control,
		Extend,
		RegionalIndicator,
		Prepend,
		SpacingMark,
		L,
		V,
		T,
		LV,
		LVT,
		XX
	
	};
	
	
	enum class LineBreak {
	
		BK,
		CR,
		LF,
		CM,
		NL,
		SG,
		WJ,
		ZW,
		GL,
		SP,
		B2,
		BA,
		BB,
		HY,
		CB,
		CL,
		CP,
		EX,
		IN,
		NS,
		OP,
		QU,
		IS,
		NU,
		PO,
		PR,
		SY,
		AI,
		AL,
		CJ,
		H2,
		H3,
		HL,
		ID,
		JL,
		JV,
		JT,
		RI,
		SA,
		XX
	
	};
	
	
	enum class WordBreak {
	
		CR,
		LF,
		Newline,
		Extend,
		RegionalIndicator,
		Format,
		Katakana,
		HebrewLetter,
		ALetter,
		SingleQuote,
		DoubleQuote,
		MidNumLet,
		MidLetter,
		MidNum,
		Numeric,
		ExtendNumLet,
		XX
	
	};
	
	
	enum class SentenceBreak {
	
		CR,
		LF,
		Extend,
		Sep,
		Format,
		Sp,
		Lower,
		Upper,
		OLetter,
		Numeric,
		ATerm,
		SContinue,
		STerm,
		Close,
		XX
	
	};
	
	
	enum class QuickCheck {
	
		Yes,
		Maybe,
		No
	
	};
	
	
	enum class NumericType {
	
		Decimal,
		Digit,
		Numeric
	
	};
	
	
	class Numeric {
	
	
		public:
		
		
			NumericType Type;
			double Value;
	
	
	};
	
	
	/**
	 *	Represents a condition.
	 */
	class Condition {
	
	
		private:
		
		
			bool negated;
			bool (* func) (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
			const char * lang;
			
			
		public:
		
		
			/**
			 *	Creates a new Condition.
			 *
			 *	\param [in] negated
			 *		\em true if the sense of the condition
			 *		should be negated, \em false otherwise.
			 *	\param [in] func
			 *		A pointer to a function which implements
			 *		the condition.
			 */
			constexpr Condition (
				bool negated,
				bool (* func) (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept
			) noexcept : negated(negated), func(func), lang(nullptr) {	}
			
			
			/**
			 *	Creates a new Condition.
			 *
			 *	\param [in] negated
			 *		\em true if the sense of the condition
			 *		should be negated, \em false otherwise.
			 *	\param [in] lang
			 *		The language for which the condition should
			 *		be true.
			 */
			constexpr Condition (
				bool negated,
				const char * lang
			) noexcept : negated(negated), func(nullptr), lang(lang) {	}
			

			/**
			 *	Evaluates the condition.
			 *
			 *	\param [in] loc
			 *		An iterator to the current location
			 *		in the string.
			 *	\param [in] begin
			 *		An iterator to the beginning of the
			 *		string.
			 *	\param [in] end
			 *		An iterator to the end of the string.
			 *	\param [in] locale
			 *		The current locale.
			 *
			 *	\return
			 *		\em true if the condition holds, \em false
			 *		otherwise.
			 */
			bool operator () (const CodePoint * loc, const CodePoint * begin, const CodePoint * end, const Locale & locale) const noexcept;
	
	
	};
	
	
	class CaseMapping {
	
	
		public:
		
		
			Array<Condition> Conditions;
			Array<CodePoint::Type> Mapping;
			
			
			operator bool () const noexcept {
			
				return Mapping.size()==0;
			
			}
	
	
	};
	
	
	/**
	 *	Contains information about a Unicode code point.
	 */
	class CodePointInfo {


		public:
		
		
			Unicode::CodePoint::Type CodePoint;
		
			//	General
			const char * Name;
			const char * Alias;
			const char * Abbreviation;
			const char * Block;
			Unicode::GeneralCategory GeneralCategory;
			const char * Script;
			bool WhiteSpace;
			bool Alphabetic;
			bool NoncharacterCodePoint;
			bool DefaultIgnorableCodePoint;
			bool Deprecated;
			bool LogicalOrderException;
			bool VariationSelector;
			
			//	Case
			bool Uppercase;
			bool Lowercase;
			Array<CaseMapping> LowercaseMappings;
			Array<CaseMapping> TitlecaseMappings;
			Array<CaseMapping> UppercaseMappings;
			Array<CaseMapping> CaseFoldings;
			std::optional<Unicode::CodePoint::Type> SimpleLowercaseMapping;
			std::optional<Unicode::CodePoint::Type> SimpleTitlecaseMapping;
			std::optional<Unicode::CodePoint::Type> SimpleUppercaseMapping;
			std::optional<Unicode::CodePoint::Type> SimpleCaseFolding;
			bool SoftDotted;
			bool Cased;
			bool CaseIgnorable;
			
			//	Numeric
			std::optional<Unicode::Numeric> Numeric;
			bool HexDigit;
			bool ASCIIHexDigit;
			
			//	Normalization
			std::size_t CanonicalCombiningClass;
			Array<Unicode::CodePoint::Type> DecompositionMapping;
			bool CompositionExclusion;
			bool FullCompositionExclusion;
			QuickCheck NFCQuickCheck;
			QuickCheck NFDQuickCheck;
			
			//	Shaping and Rendering
			Unicode::LineBreak LineBreak;
			Unicode::GraphemeClusterBreak GraphemeClusterBreak;
			Unicode::SentenceBreak SentenceBreak;
			Unicode::WordBreak WordBreak;
			
			//	Bidirectional
			Unicode::BidirectionalClass BidirectionalClass;
			bool BidirectionalControl;
			bool Mirrored;
			
			//	Miscellaneous
			bool Math;
			bool QuotationMark;
			bool Dash;
			bool Hyphen;
			bool STerm;
			bool TerminalPunctuation;
			bool Diacritic;
			bool Extender;
			bool GraphemeBase;
			bool GraphemeLink;


	};
	
	
	/**
	 *	Contains information about a Unicode
	 *	composition.
	 */
	class Composition {
	
	
		public:
		
		
			CodePoint Starter;
			CodePoint Joiner;
			CodePoint Result;
	
	
	};


}
