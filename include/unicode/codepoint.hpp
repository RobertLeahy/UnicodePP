/**
 *	\file
 */
 
 
#pragma once


#include <cstdint>


namespace Unicode {


	class CodePointInfo;
	class Locale;


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
	
	
	};
	
	
	/**
	 *	Possible categories for a Unicode code point.
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
	enum class BidirectionalType {

		L,		/**<	Left-to-Right	*/
		LRE,	/**<	Left-to-Right Embedding	*/
		LRO,	/**<	Left-to-Right Override	*/
		R,		/**<	Right-to-Left	*/
		AL,		/**<	Right-to-Left Arabic	*/
		RLE,	/**<	Right-to-Left Embedding	*/
		RLO,	/**<	Right-to-Left Override	*/
		PDF,	/**<	Pop Directional Format	*/
		EN,		/**<	European Number	*/
		ES,		/**<	European Number Separator	*/
		ET,		/**<	European Number Terminator	*/
		AN,		/**<	Arabic Number	*/
		CS,		/**<	Common Number Separator	*/
		NSM,	/**<	Nonspacing Mark	*/
		BN,		/**<	Boundary Neutral	*/
		B,		/**<	Paragraph Separator	*/
		S,		/**<	Segment Separator	*/
		WS,		/**<	Whitespace	*/
		ON,		/**<	Other Neutrals	*/

	};
	
	
	/**
	 *	Determines grapheme cluster boundaries.
	 */
	enum class GraphemeBreak {
	
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
		Any
	
	};
	
	
	/**
	 *	Contains information about a Unicode code point.
	 */
	class CodePointInfo {


		public:
		
		
			/**
			 *	The code point.
			 */
			CodePoint CP;
			/**
			 *	The Unicode category.
			 */
			GeneralCategory Category;
			/**
			 *	The code point's combining class, or 0 if it is not a combining mark.
			 */
			std::size_t CanonicalCombiningClass;
			/**
			 *	The length of the sequence of code points into which this code point
			 *	canonically decomposes, or 0 if it does not decompose.
			 */
			std::size_t DecompositionMappingLength;
			/**
			 *	The code points into which this code point canonically decomposes, or
			 *	nullptr if it does not decompose.
			 */
			const CodePoint * DecompositionMapping;
			/**
			 *	Whether or not this code point corresponds to a decimal digit value.
			 */
			bool HasDecimalDigitValue;
			/**
			 *	The decimal digit value to which this code point corresponds,
			 *	if any.
			 */
			std::size_t DecimalDigitValue;
			/**
			 *	Whether or not this code point corresponds to a digit value.
			 */
			bool HasDigitValue;
			/**
			 *	The digit value to which this code point corresponds, if any.
			 */
			std::size_t DigitValue;
			/**
			 *	Whether or not this code point corresponds to a numeric value.
			 */
			bool HasNumericValue;
			/**
			 *	The numeric value to which this code point corresponds, if any.
			 */
			double NumericValue;
			/**
			 *	Whether or not this code point has an upper case equivalent.
			 */
			bool HasUpperCase;
			/**
			 *	The upper case equivalent of this code point, if any.
			 */
			CodePoint UpperCase;
			/**
			 *	Whether or not this code point has a lower case equivalent.
			 */
			bool HasLowerCase;
			/**
			 *	The lower case equivalent of this code point, if any.
			 */
			CodePoint LowerCase;
			/**
			 *	Whether or not this code point has a title case equivalent.
			 */
			bool HasTitleCase;
			/**
			 *	The title case equivalent of this code point, if any.
			 */
			CodePoint TitleCase;
			/**
			 *	A pointer to a null-terminated string of ASCII characters indicating
			 *	the Unicode block to which the code point belongs.
			 */
			const char * Block;
			/**
			 *	A pointer to a null-terminated string of ASCII characters indicating
			 *	the Unicode script to which the code point belongs.
			 */
			const char * Script;
			/**
			 *	The length of the collation elements collection for this code point.
			 */
			std::size_t CollationElementsLength;
			/**
			 *	A collection of integers representing the collation elements for this
			 *	code point.
			 */
			const std::uint32_t * CollationElements;
			/**
			 *	The bidirectional type of this code point.
			 */
			BidirectionalType Bidirectionality;
			/**
			 *	A string describing the character.
			 */
			const char * Description;
			/**
			 *	The value of the code point's Grapheme_Cluster_Break
			 *	property.
			 */
			GraphemeBreak Break;


	};
	
	
	/**
	 *	Contains information about a Unicode
	 *	composition.
	 */
	class Composition {
	
	
		public:
		
		
			CodePoint Starter;
			CodePoint Combiner;
			CodePoint Result;
	
	
	};


}
