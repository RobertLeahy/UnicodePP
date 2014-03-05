/** 
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/locale.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/vector.hpp>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	Encapsulates a C-style string.
	 */
	class CString {
	
	
		private:
		
		
			std::vector<unsigned char> vec;
			
			
		public:
		
		
			/**
			 *	\cond
			 */
			 
			 
			CString (std::vector<unsigned char> vec) : vec(std::move(vec)) {
			
				vec.push_back(0);
			
			}
			
			
			/**
			 *	\endcond
			 */
			
			
			/**
			 *	Retrieves a pointer to a null-terminated string.
			 *
			 *	\return
			 *		A pointer to a null-terminated string.
			 */
			operator const char * () const noexcept {
			
				return reinterpret_cast<const char *>(vec.data());
			
			}
			
			
			/**
			 *	Determines the size of the string in bytes.
			 *
			 *	Unlike calling std::strlen on an actual C-style
			 *	string, this operation is O(1).
			 *
			 *	\return
			 *		The size of the string in bytes.
			 */
			std::size_t Size () const noexcept {
			
				return vec.size()-1;
			
			}
	
	
	};


	/**
	 *	Encapsulates a Unicode string.
	 */
	class String {
	
	
		private:
		
		
			std::vector<CodePoint> cps;
			const Locale * locale;
			
			
			void trim_front (const Locale &) noexcept;
			void trim_rear (const Locale &) noexcept;
			void trim (const Locale &) noexcept;
			
			
			std::vector<CodePoint> to_upper (bool) const;
			std::vector<CodePoint> to_lower (bool) const;
			std::vector<CodePoint> to_case_fold (bool) const;
			
			
			bool is_nfd (const Locale &) const noexcept;
			bool is_nfc (const Locale &) const noexcept;
			std::vector<CodePoint> to_nfd (const Locale &) const;
			std::vector<CodePoint> to_nfc (const Locale &) const;
			
			
		public:
		
		
			/**
			 *	Creates a new, empty string.
			 */
			String () noexcept : locale(nullptr) {	}
			/**
			 *	Creates a new, empty string which will use a
			 *	certain locale for its operations.
			 *
			 *	\param [in] locale
			 *		The locale to use.
			 */
			explicit String (const Locale & locale) noexcept : locale(&locale) {	}
			/**
			 *	Creates a new string containing a sequence of
			 *	Unicode code points.
			 *
			 *	\param [in] cps
			 *		A vector containing the sequence of code
			 *		points with which to initialize the string.
			 */
			String (std::vector<CodePoint> cps) noexcept : cps(std::move(cps)), locale(nullptr) {	}
			/**
			 *	Creates a new string containing a sequence of
			 *	Unicode code points, and which will use a certain
			 *	locale for its operations.
			 *
			 *	\param [in] cps
			 *		A vector containing the sequence of code
			 *		points with which to initialize the string.
			 *	\param [in] locale
			 *		The locale to use.
			 */
			String (std::vector<CodePoint> cps, const Locale & locale) noexcept : cps(std::move(cps)), locale(&locale) {	}
			
			
			String (const String &) = default;
			String (String &&) = default;
			String & operator = (const String &) = default;
			String & operator = (String &&) = default;
			
			
			/**
			 *	Overwrites this string with a sequence of code
			 *	points.
			 *
			 *	After this operation completes, the string's
			 *	internal locale will be reset to default.
			 *
			 *	\param [in] cps
			 *		A vector containing the sequence of code
			 *		points with which to overwrite the string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator = (std::vector<CodePoint> cps) noexcept {
			
				locale=nullptr;
				cps=std::move(cps);
				
				return *this;
			
			}
			
			
			/**
			 *	Creates a string from a C string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-8 characters.
			 */
			String (const char * str);
			/**
			 *	Overwrites this string with a C string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-8 characters.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator = (const char * str);
			
			
			/**
			 *	Retrieves an iterator to the beginning of the string.
			 *
			 *	\return
			 *		An iterator to the beginning of the string.
			 */
			const CodePoint * begin () const noexcept {
			
				return Begin(cps);
			
			}
			/**
			 *	Retrieves an iterator to the beginning of the string.
			 *
			 *	\return
			 *		An iterator to the beginning of the string.
			 */
			CodePoint * begin () noexcept {
			
				return Begin(cps);
			
			}
			/**
			 *	Retrieves an iterator to the end of the string.
			 *
			 *	\return
			 *		An iterator to the end of the string.
			 */
			const CodePoint * end () const noexcept {
			
				return End(cps);
			
			}
			/**
			 *	Retrieves an iterator to the end of the string.
			 *
			 *	\return
			 *		An iterator to the end of the string.
			 */
			CodePoint * end () noexcept {
			
				return End(cps);
			
			}
			
			
			/**
			 *	Retrieves the locale associated with this string.
			 *
			 *	\return
			 *		The locale associated with this string.  If
			 *		there is no locale associated with this string
			 *		the current locale is returned.
			 */
			const Locale & GetLocale () const noexcept {
			
				return (locale==nullptr) ? Locale::Get() : *locale;
			
			}
			/**
			 *	Associates a locale with this string, causing the
			 *	string to use that locale for all internal operations.
			 *
			 *	\param [in] locale
			 *		The locale.
			 */
			void SetLocale (const Locale & locale) noexcept {
			
				this->locale=&locale;
			
			}
			
			
			/**
			 *	Retrieves a reference to the vector of code points
			 *	which underlies this string.
			 *
			 *	\return
			 *		A reference to a vector of code points.
			 */
			std::vector<CodePoint> & CodePoints () noexcept {
			
				return cps;
			
			}
			/**
			 *	Retrieves a reference to the vector of code points
			 *	which underlies this string.
			 *
			 *	\return
			 *		A reference to a vector of code points.
			 */
			const std::vector<CodePoint> & CodePoints () const noexcept {
			
				return cps;
			
			}
			
			
			/**
			 *	Converts this string to a C string.
			 *
			 *	\param [in] utf8
			 *		If \em true the UTF-8 encoding will be used,
			 *		otherwise the string will be converted to
			 *		Latin-1.  Defaults to \em true.
			 *
			 *	\return
			 *		An object which is implicitly convertible to
			 *		const char *, but which manages the necessary
			 *		dynamically-allocated memory using the RAII
			 *		idiom.
			 */
			CString ToCString (bool utf8=true) const;
			
			
			/**
			 *	Removes all whitespace from the beginning of this
			 *	string.
			 *
			 *	\return
			 *		A copy of this string with all whitespace
			 *		removed from the beginning.
			 */
			String TrimFront () const &;
			/**
			 *	Removes all whitespace from the beginning of this
			 *	string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & TrimFront () & noexcept;
			/**
			 *	Removes all whitespace from the beginning of this
			 *	string.
			 *
			 *	\return
			 *		This string.
			 */
			String TrimFront () && noexcept;
			
			
			/**
			 *	Removes all whitespace from the end of this
			 *	string.
			 *
			 *	\return
			 *		A copy of this string with all whitespace
			 *		removed from the end.
			 */
			String TrimRear () const &;
			/**
			 *	Removes all whitespace from the end of this
			 *	string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & TrimRear () & noexcept;
			/**
			 *	Removes all whitespace from the end of this
			 *	string.
			 *
			 *	\return
			 *		This string.
			 */
			String TrimRear () && noexcept;
			
			
			/**
			 *	Removes all whitespace from both the beginning
			 *	and end of this string.
			 *
			 *	\return
			 *		A copy of this string with all whitespace
			 *		removed from the beginning and end.
			 */
			String Trim () const &;
			/**
			 *	Removes all whitespace from both the beginning
			 *	and end of this string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & Trim () & noexcept;
			/**
			 *	Removes all whitespace from both the beginning
			 *	and end of this string.
			 *
			 *	\return
			 *		This string.
			 */
			String Trim () && noexcept;
			
			
			/**
			 *	Converts this string to upper case.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A copy of this string with each character
			 *		converted to upper case.
			 */
			String ToUpper (bool full=true) const &;
			/**
			 *	Converts this string to upper case.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & ToUpper (bool full=true) &;
			
			
			/**
			 *	Converts this string to lower case.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A copy of this string with each character
			 *		converted to upper case.
			 */
			String ToLower (bool full=true) const &;
			/**
			 *	Converts this string to lower case.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & ToLower (bool full=true) &;
			
			
			/**
			 *	Performs case folding on this string.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A copy of this string with each character
			 *		case folded.
			 */
			String ToCaseFold (bool full=true) const &;
			/**
			 *	Performs case folding on this string.
			 *
			 *	\param [in] full
			 *		If \em true full case mappings will be
			 *		used.  Defaults to \em true.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & ToCaseFold (bool full=true) &;
			
			
			/**
			 *	Determines if this string is in Normal Form
			 *	Canonical Decomposition.
			 *
			 *	Note that this method is permitted to spuriously
			 *	return \em false, but will never spuriously
			 *	return \em true.
			 *
			 *	\return
			 *		\em true if this string is in NFD, \em false
			 *		otherwise.
			 */
			bool IsNFD () const noexcept;
			/**
			 *	Determines if this string is in Normal Form
			 *	Canonical Composition.
			 *
			 *	Note that this method is permitted to spuriously
			 *	return \em false, but will never spuriously return
			 *	\em true.
			 *
			 *	\return
			 *		\em true if this string is in NFC, \em false
			 *		otherwise.
			 */
			bool IsNFC () const noexcept;
			
			
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Decomposition.
			 *
			 *	\return
			 *		A copy of this string converted to NFD.
			 */
			String ToNFD () const &;
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Decomposition.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & ToNFD () &;
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Decomposition.
			 *
			 *	\return
			 *		This string.
			 */
			String ToNFD () &&;
			
			
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Composition.
			 *
			 *	\return
			 *		A copy of this string converted to NFC.
			 */
			String ToNFC () const &;
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Composition.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & ToNFC () &;
			/**
			 *	Converts this string to Normal Form Canonical
			 *	Composition.
			 *
			 *	\return
			 *		This string.
			 */
			String ToNFC () &&;
			
			
			/**
			 *	Determines whether or not this string and another
			 *	string are canonically equivalent.
			 *
			 *	The locale of this string will always be used.
			 *
			 *	\param [in] other
			 *		The string with which to compare this string.
			 *	\param [in] case_sensitive
			 *		If \em false the case of characters shall be
			 *		ignored when comparing this string and \em other.
			 *		Defaults to \em true.
			 *
			 *	\return
			 *		\em true if this string and \em other should be
			 *		considered equivalent, \em false otherwise.
			 */
			bool Equals (const String & other, bool case_sensitive=true) const;
			/**
			 *	Determines whether or not this string and another
			 *	string are canonically equivalent.
			 *
			 *	\param [in] str
			 *		A C style string with which to compare this
			 *		string.
			 *	\param [in] case_sensitive
			 *		If \em false the case of characters shall be
			 *		ignored when comparing this string and \em other.
			 *		Defaults to \em true.
			 *
			 *	\return
			 *		\em true if this string and \em other should be
			 *		considered equivalent, \em false otherwise.
			 */
			bool Equals (const char * str, bool case_sensitive=true) const;
			
			
			/**
			 *	Compares this string with another string, and
			 *	determines how they should be ordered.
			 *
			 *	The locale of this string will always be used.
			 *
			 *	\param [in] other
			 *		The string with which to compare this
			 *		string.
			 *
			 *	\return
			 *		Zero if this string and \em other should be
			 *		ordered the same.  A negative value if this
			 *		string should be ordered before \em other.  A
			 *		positive value if this string should be ordered
			 *		after \em other.
			 */
			int Compare (const String & other) const;
	
	
	};
	
	
	inline bool operator == (const String & a, const String & b) {
	
		return a.Equals(b);
	
	}
	
	
	inline bool operator != (const String & a, const String & b) {
	
		return !a.Equals(b);
	
	}
	
	
	inline bool operator == (const String & a, const char * b) {
	
		return a.Equals(b);
	
	}
	
	
	inline bool operator != (const String & a, const char * b) {
	
		return !a.Equals(b);
	
	}
	
	
	inline bool operator == (const char * a, const String & b) {
	
		return b.Equals(a);
	
	}
	
	
	inline bool operator != (const char * a, const String & b) {
	
		return !b.Equals(a);
	
	}
	
	
	inline bool operator < (const String & a, const String & b) {
	
		return a.Compare(b)<0;
	
	}
	
	
	inline bool operator <= (const String & a, const String & b) {
	
		return a.Compare(b)<=0;
	
	}
	
	
	inline bool operator > (const String & a, const String & b) {
	
		return a.Compare(b)>0;
	
	}
	
	
	inline bool operator >= (const String & a, const String & b) {
	
		return a.Compare(b)>=0;
	
	}
	
	
}


/**
 *	\cond
 */


namespace std {


	template <>
	struct hash<Unicode::String> {
	
	
		private:
		
		
			static size_t compute (const Unicode::String & str) noexcept {
			
				size_t retr=5381;
				for (auto cp : str) {
				
					retr*=33;
					retr^=static_cast<Unicode::CodePoint::Type>(cp);
				
				}
				
				return retr;
			
			}
	
	
		public:
		
		
			template <typename T>
			size_t operator () (T && str) const {
			
				return compute(str.ToNFD());
			
			}
	
	
	};


}


/**
 *	\endcond
 */
