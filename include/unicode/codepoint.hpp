/**
 *	\file
 */
 
 
#pragma once


#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>


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
			
			
			CodePoint (const CodePoint &) = default;
			CodePoint (CodePoint &&) = default;
			CodePoint & operator = (const CodePoint &) = default;
			CodePoint & operator = (CodePoint &&) = default;
		
		
			/**
			 *	Creates a code point.
			 */
			constexpr CodePoint () noexcept : cp(0) {	}
			/**
			 *	Creates a code point from an integer.
			 *
			 *	\param [in] cp
			 *		An integer.
			 */
			constexpr CodePoint (Type cp) noexcept : cp(cp) {	}
			/**
			 *	Creates a code point from a character.
			 *
			 *	\param [in] c
			 *		A character.
			 */
			constexpr CodePoint (char c) noexcept : cp(static_cast<Type>(c)) {	}
			
			
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
			constexpr operator Type () const noexcept {
			
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
			 *	white space.
			 *
			 *	\param [in] locale
			 *		The locale from which to get information
			 *		about this code point.  Optional.  Defaults
			 *		to the current locale.
			 *	
			 *	\return
			 *		\em true if this code point is
			 *		white space, \em false otherwise.
			 */
			bool IsWhiteSpace (const Locale & locale=get_locale()) const noexcept;
	
	
	};
	
	
	template <typename T>
	constexpr typename std::enable_if<std::is_integral<T>::value,bool>::type operator == (CodePoint a, T b) noexcept {
	
		return static_cast<CodePoint::Type>(a)==b;
	
	}
	
	
	template <typename T>
	constexpr typename std::enable_if<std::is_integral<T>::value,bool>::type operator == (T a, CodePoint b) noexcept {
	
		return b==a;
	
	}
	
	
	/**
	 *	Encapsulates a compile time array.
	 */
	template <typename T>
	class Array {
	
	
		public:
		
		
			const T * Data;
			std::size_t Size;
			
			
			/**
			 *	Creates an empty array.
			 */
			constexpr Array () noexcept : Data(nullptr), Size(0) {	}
			/**
			 *	Creates an array.
			 *
			 *	\param [in] data
			 *		A pointer to the array.
			 *	\param [in] size
			 *		The size of the array pointed to by
			 *		\em data.
			 */
			constexpr Array (const T * data, std::size_t size) noexcept : Data(data), Size(size) {	}
			/**
			 *	Creates an array.
			 *
			 *	\tparam i
			 *		The size of the array to create.
			 *
			 *	\param [in] array
			 *		The array.
			 */
			template <std::size_t i>
			constexpr Array (const T (& arr) [i]) noexcept : Data(arr), Size(i) {	}
			
			
			Array (const Array &) = default;
			Array (Array &&) = default;
			Array & operator = (const Array &) = default;
			Array & operator = (Array &&) = default;
			
			
			constexpr const T * begin () const noexcept {
			
				return Data;
			
			}
			
			
			constexpr const T * end () const noexcept {
			
				return Data+Size;
			
			}
			
			
			constexpr std::size_t size () const noexcept {
			
				return Size;
			
			}
			
			
			constexpr const T & operator [] (std::size_t i) const noexcept {
			
				return Data[i];
			
			}
	
	
	};
	
	
	/**
	 *	Creates an array from a C style array.
	 *
	 *	\tparam T
	 *		The type of array to create.
	 *	\tparam i
	 *		The size of the array.
	 *
	 *	\param [in] arr
	 *		The array.
	 *
	 *	\return
	 *		An array object which encapsulates the
	 *		C style array.
	 */
	template <typename T, std::size_t i>
	constexpr Array<T> MakeArray (const T (& arr) [i]) noexcept {
	
		return Array<T>(arr);
	
	}
	
	
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
	
	
	/**
	 *	Determines line boundaries.
	 */
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
	
	
	/**
	 *	Determines word boundaries.
	 */
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
	
	
	/**
	 *	Determines sentence boundaries.
	 */
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
	
	
	/**
	 *	The values which the quick check properties
	 *	for various normalization forms may take on.
	 */
	enum class QuickCheck {
	
		/**
		 *	The code point-in-question may be present
		 *	in the specified normalization form.
		 */
		Yes,
		/**
		 *	Further processing is required to determine
		 *	whether or not the code point-in-question may
		 *	be present in the specified normalization form.
		 */
		Maybe,
		/**
		 *	The code point-in-question may not be present
		 *	in the specified normalization form.
		 */
		No
	
	};
	
	
	/**
	 *	The values the Numeric_Type Unicode property
	 *	may take on.
	 */
	enum class NumericType {
	
		Decimal,
		Digit,
		Numeric
	
	};
	
	
	/**
	 *	Encapsulates the Numeric_Type and Numeric_Value
	 *	Unicode properties.
	 */
	class Numeric {
	
	
		public:
		
		
			/**
			 *	The type of numeric value.
			 */
			NumericType Type;
			/**
			 *	The numeric value.  In the case of NumericType::Decimal
			 *	and NumericType::Digit this is guaranteed to be losslessly
			 *	convertible to an unsigned integral type.
			 */
			double Value;
	
	
	};
	
	
	/**
	 *	Represents a condition.
	 */
	class Condition {
	
	
		private:
		
		
			bool negated;
			const char * cond;
			
			
		public:
			
			
			/**
			 *	Creates a new Condition.
			 *
			 *	\param [in] negated
			 *		\em true if the sense of the condition
			 *		should be negated, \em false otherwise.
			 *	\param [in] cond
			 *		A string giving the Unicode condition.
			 */
			constexpr Condition (
				bool negated,
				const char * cond
			) noexcept : negated(negated), cond(cond) {	}
			

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
	
	
	/**
	 *	Encapsulates a full case mapping.
	 */
	class CaseMapping {
	
	
		public:
		
		
			/**
			 *	The conditions which must hold for this
			 *	case mapping to be applied.
			 */
			Array<Condition> Conditions;
			/**
			 *	The mapping.
			 */
			Array<CodePoint::Type> Mapping;
	
	
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
		
		
			/**
			 *	The code points which compose.
			 */
			Array<CodePoint::Type> CodePoints;
			/**
			 *	The code point to which the code points
			 *	given by CodePoints compose.
			 */
			CodePoint::Type Result;
			
	
	};


}
