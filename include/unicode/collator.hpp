/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <optional>
#include <utility>
#include <vector>


namespace Unicode {


	enum class VariableOrdering {
	
		NonIgnorable,
		Blanked,
		Shifted,
		ShiftTrimmed
	
	};
	
	
	class CollationElementContainer;


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
		
		
			Collator (const Locale & locale=Locale::Get()) noexcept
				:	locale(locale),
					vo(VariableOrdering::NonIgnorable)
			{	}
			
			
			std::vector<CodePoint::Type> GetSortKey (const CodePoint * begin, const CodePoint * end) const;
			
			
			int Compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const;
	
	
	};


}
