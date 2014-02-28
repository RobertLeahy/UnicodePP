/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>


namespace Unicode {


	/**
	 *	Represents a locale, a certain set of rules
	 *	as to how Unicode code points should be
	 *	interpreted and treated.
	 */
	class Locale {
	
	
		public:
			
			
			template <typename T, std::size_t i>
			static Array<const T> Make (T (& arr) [i]) noexcept {
			
				return Array<const T>(arr,i);
			
			}
		
		
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
			
			
			Array<const CodePointInfo> Info;
			
			
			Array<const Composition> Compositions;
		
		
			const CodePointInfo * GetInfo (CodePoint cp) const noexcept;
			std::optional<CodePoint> GetComposition (CodePoint starter, CodePoint joiner) const noexcept;
	
	
	};
	
	
	/**
	 *	The default locale.
	 */
	extern const Locale DefaultLocale;


}
