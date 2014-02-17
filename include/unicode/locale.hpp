/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <cstddef>
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
		
		
			template <typename T>
			class Array {
			
			
				public:
				
				
					T * Data;
					std::size_t Size;
					
					
					Array () noexcept : Data(nullptr), Size(0) {	}
					Array (T * data, std::size_t size) noexcept : Data(data), Size(size) {	}
					
					
					Array (const Array &) = default;
					Array (Array &&) = default;
					Array & operator = (const Array &) = default;
					Array & operator = (Array &&) = default;
					
					
					T * begin () noexcept {
					
						return Data;
					
					}
					
					
					const T * begin () const noexcept {
					
						return Data;
					
					}
					
					
					T * end () noexcept {
					
						return Data+Size;
					
					}
					
					
					const T * end () const noexcept {
					
						return Data+Size;
					
					}
					
					
					T & operator [] (std::size_t i) noexcept {
					
						return Data[i];
					
					}
					
					
					const T & operator [] (std::size_t i) const noexcept {
					
						return Data[i];
					
					}
			
			
			};
			
			
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
	
	
	};
	
	
	/**
	 *	The default locale.
	 */
	extern const Locale DefaultLocale;


}
