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
	 *	The various schemes provided by the Unicode
	 *	Collation Algorithm for handling variable
	 *	collation elements.
	 */
	enum class VariableOrdering {
	
		NonIgnorable,
		Blanked,
		Shifted,
		ShiftTrimmed
	
	};
	
	
	/**
	 *	\cond
	 */
	
	
	class CollationElementContainer;
	
	
	/**
	 *	\endcond
	 */


	/**
	 *	Collates Unicode strings.
	 */
	class Collator {
	
	
		private:
		
		
			const Locale & locale;
			VariableOrdering vo;
			//	Null means all levels, zero means
			//	identical, otherwise it's numerical
			std::optional<std::size_t> strength;
			
			
			bool is_backwards () const noexcept;
			void add_weight (std::vector<CodePoint::Type> &, std::size_t, const CollationElementContainer &, bool &) const;
			template <typename Iter>
			void add_level (std::vector<CodePoint::Type> &, std::size_t, Iter, Iter) const;
			std::optional<std::vector<CodePoint>> prepare (const CodePoint *, const CodePoint *) const;
			void get_identical (std::vector<CodePoint::Type> &, const CodePoint *, const CodePoint *) const;
			std::size_t get_max (std::size_t) const noexcept;
			std::vector<CodePoint::Type> get_sort_key (const CodePoint *, const CodePoint *) const;
			
			
		public:
		
		
			/**
			 *	Creates a new Collator which collates
			 *	strings according to a certain locale.
			 *
			 *	\param [in] locale
			 *		The locale.  If not provided defaults
			 *		to the current locale.
			 */
			Collator (const Locale & locale=Locale::Get()) noexcept
				:	locale(locale),
					vo(VariableOrdering::NonIgnorable)
			{	}
			
			
			/**
			 *	Instructs the Collator as to how to handle
			 *	variable collation elements.
			 *
			 *	\param [in] ordering
			 *		A member of the VariableOrdering enumeration
			 *		which specifies a scheme for handling variable
			 *		collation elements.
			 */
			void SetVariableOrdering (VariableOrdering ordering) noexcept {
			
				vo=ordering;
			
			}
			
			
			/**
			 *	Sets the strength of the sort keys the collator
			 *	generates.
			 *
			 *	\param [in] strength
			 *		The strength of the sort keys the collator
			 *		will generate.  If set to a non-zero number,
			 *		the collator will generate sort keys up to
			 *		that level.  If set to a zero number, the
			 *		collator will generate sort keys as long as
			 *		necessary, plus the identical level.  If
			 *		disengaged as many levels as needed will be
			 *		generated, but not the identical level.
			 */
			void SetStrength (std::optional<std::size_t> strength) {
			
				this->strength=strength;
			
			}
			
			
			/**
			 *	Retrieves a sort key for a string of Unicode
			 *	characters.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a string of
			 *		Unicode characters.
			 *	\param [in] end
			 *		An iterator to the end of a string of Unicode
			 *		characters.
			 *
			 *	\return
			 *		The sort key.
			 */
			std::vector<CodePoint::Type> GetSortKey (const CodePoint * begin, const CodePoint * end) const;
			
			
			/** 
			 *	Compares two strings of Unicode characters.
			 *
			 *	\param [in] begin_a
			 *		An iterator to the beginning of the first string
			 *		of Unicode characters.
			 *	\param [in] end_a
			 *		An iterator to the end of the first string of
			 *		Unicode characters.
			 *	\param [in] begin_b
			 *		An iterator to the beginning of the second string
			 *		of Unicode characters.
			 *	\param [in] end_b
			 *		An iterator to the end of the second string of
			 *		Unicode characters.
			 *
			 *	\return
			 *		A negative value if the first string should be
			 *		ordered before the second string.  Zero if the
			 *		first and second strings should be ordered the
			 *		same.  A positive value if the first string should
			 *		be ordered after the second string.
			 */
			int Compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const;
	
	
	};


}
