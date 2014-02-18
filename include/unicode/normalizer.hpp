/**
 *	\file
 */
 
 
#pragma once


#include <unicode/locale.hpp>
#include <vector>


namespace Unicode {


	/**
	 *	The various Unicode normalization forms.
	 */
	enum class NormalForm {
	
		C,	/**<	Normal Form, Canonical Composition	*/
		D	/**<	Normal Form, Canonical Decomposition	*/
	
	};


	/**
	 *	Normalizes strings of Unicode code
	 *	points.
	 */
	class Normalizer {
	
	
		private:
		
		
			NormalForm nf;
			const Locale * locale;
			
			
			std::vector<CodePoint> nfd (const CodePoint *, const CodePoint *, const Locale &) const;
			bool is_nfd (const CodePoint *, const CodePoint *, const Locale &) const noexcept;
			std::vector<CodePoint> nfc (const CodePoint *, const CodePoint *, const Locale &) const;
			
			
		public:
		
		
			/**
			 *	The default normal form.
			 *
			 *	This is the normal form that the normalizer
			 *	shall use if default constructed.
			 */
			static constexpr NormalForm Default=NormalForm::C;
		
		
			/**
			 *	Creates a new Normalizer.
			 *
			 *	\param [in] nf
			 *		The desired normal form.  If omitted
			 *		defaults to Default.
			 */
			Normalizer (NormalForm nf=Default) noexcept : nf(nf), locale(nullptr) {	}
			/**
			 *	Creates a new Normalizer.
			 *
			 *	\param [in] nf
			 *		The desired normal form.  If omitted
			 *		defaults to Default.
			 *	\param [in] locale
			 *		The locale this normalizer should use.
			 */
			Normalizer (NormalForm nf, const Locale & locale) noexcept : nf(nf), locale(&locale) {	}
			
			
			/**
			 *	Retrieves the normalizer's current locale.
			 *
			 *	\return
			 *		The locale.
			 */
			const Locale & GetLocale () const noexcept {
			
				return (locale==nullptr) ? Locale::Get() : *locale;
			
			}
			
			
			/**
			 *	Sets the normalizer's current locale.
			 *
			 *	\param [in] locale
			 *		The locale.
			 */
			void SetLocale (const Locale & locale) noexcept {
			
				this->locale=&locale;
			
			}
			
			
			/**
			 *	Clears the normalizer's internal locale,
			 *	causing it to use the default locale.
			 */
			void ClearLocale () noexcept {
			
				locale=nullptr;
			
			}
			
			
			/**
			 *	Normalizes a string of Unicode code points.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a string
			 *		of Unicode code points.
			 *	\param [in] end
			 *		An iterator to the end of a string of
			 *		Unicode code points.
			 *
			 *	\return
			 *		A vector containing the range bounded by
			 *		\em begin and \em end places in the
			 *		normal form associated with this object.
			 */
			std::vector<CodePoint> Normalize (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Checks to see if a string of Unicode code points
			 *	is normalized.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a string
			 *		of Unicode code points.
			 *	\param [in] end
			 *		An iterator to the end of a string of
			 *		Unicode code points.
			 *
			 *	\return
			 *		\em true if the range bounded by \em begin
			 *		and \em end is in the normal form
			 *		associated with this object, \em false
			 *		otherwise.
			 */
			bool IsNormalized (const CodePoint * begin, const CodePoint * end) const;
	
	
	};


}
 