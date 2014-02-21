/** 
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/normalizer.hpp>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>


namespace Unicode {


	class String {
	
	
		private:
		
		
			std::vector<CodePoint> cps;
			const Locale * locale;
			
			
			CodePoint * ptr_begin () noexcept;
			const CodePoint * ptr_begin () const noexcept;
			CodePoint * ptr_end () noexcept;
			const CodePoint * ptr_end () const noexcept;
			
			
			void trim_front (const Locale &) noexcept;
			void trim_rear (const Locale &) noexcept;
			
			
		public:
		
		
			/**
			 *	Creates an empty string.
			 */
			String () noexcept(std::is_nothrow_constructible<std::vector<CodePoint>>::value) : locale(nullptr) {	}
			String (const String &) = default;
			String (String &&) = default;
			String & operator = (const String &) = default;
			String & operator = (String &&) = default;
			
			
			/**
			 *	Creates a string containing a certain
			 *	vector of code points.
			 *
			 *	\param [in] cps
			 *		A vector of Unicode code points.
			 */
			String (std::vector<CodePoint> cps) noexcept : cps(std::move(cps)), locale(nullptr) {	}
			/**
			 *	Assigns a vector of code points to this
			 *	string.
			 *
			 *	\param [in] cps
			 *		A vector of Unicode code points.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator = (std::vector<CodePoint> cps) noexcept {
			
				this->cps=std::move(cps);
				locale=nullptr;
				
				return *this;
			
			}
			
			
			String (const char * str);
			String & operator = (const char * str);
			
			
			std::vector<char> ToCString () const;
			
			
			/**
			 *	Determines the number of code points in
			 *	this string.
			 *
			 *	\return
			 *		The number of code points in this
			 *		string.
			 */
			std::size_t Size () const noexcept {
			
				return cps.size();
			
			}
			
			
			/**
			 *	Retrieves a reference to the vector of
			 *	code points underlying this string.
			 *
			 *	\return
			 *		A reference to a vector of code points.
			 */
			const std::vector<CodePoint> & CodePoints () const & noexcept {
			
				return cps;
			
			}
			
			
			/**
			 *	Retrieves a reference to the vector of
			 *	code points underlying this string.
			 *
			 *	\return
			 *		A reference to a vector of code points.
			 */
			std::vector<CodePoint> & CodePoints () & noexcept {
			
				return cps;
			
			}
			
			
			/**
			 *	Retrieves the vector of code points
			 *	underlying this string.
			 *
			 *	\return
			 *		A vector of code points.
			 */
			std::vector<CodePoint> CodePoints () && noexcept {
			
				return std::move(cps);
			
			}
			
			
			/**
			 *	Retrieves this string's locale.
			 *
			 *	\return
			 *		A reference to this string's locale.
			 */
			const Locale & GetLocale () const noexcept {
			
				return (locale==nullptr) ? Locale::Get() : *locale;
			
			}
			
			
			/**
			 *	Sets this string's locale.
			 *
			 *	\param [in] locale
			 *		This string's new locale.
			 */
			void SetLocale (const Locale & locale) noexcept {
			
				this->locale=&locale;
			
			}
			
			
			/**
			 *	Clears this string's locale, causing it
			 *	to use the current locale.
			 */
			void ClearLocale () noexcept {
			
				locale=nullptr;
			
			}
			
			
			/**
			 *	Trims whitespace from the beginning and
			 *	end of this string.
			 *
			 *	\return
			 *		A new string, which is a copy of
			 *		this string with all leading and
			 *		trailing whitespace removed.
			 */
			String Trim () const;
			/**
			 *	Trims whitespace from the beginning and
			 *	end of this string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & Trim () noexcept;
			
			
			/**
			 *	Places this string in a Unicode normal form.
			 *
			 *	\param [in] nf
			 *		The normal form in which to place this
			 *		string.  Defaults to the default specified
			 *		by the Normalizer class.
			 *
			 *	\return
			 *		A new string, which is a copy of this
			 *		string transformed to the desired normal
			 *		form.
			 */
			String Normalize (NormalForm nf=Normalizer::Default) const &;
			/**
			 *	Places this string in a Unicode normal form.
			 *
			 *	\param [in] nf
			 *		The normal form in which to place this
			 *		string.  Defaults to the default specified
			 *		by the Normalizer class.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & Normalize (NormalForm nf=Normalizer::Default) &;
			/**
			 *	Places this string in a Unicode normal form.
			 *
			 *	\param [in] nf
			 *		The normal form in which to place this
			 *		string.  Defaults to the default specified
			 *		by the Normalizer class.
			 *
			 *	\return
			 *		A new string, which is a copy of this
			 *		string transformed to the desired normal
			 *		form.
			 */
			String Normalize (NormalForm nf=Normalizer::Default) &&;
			
			
			
	
	
	};


}
