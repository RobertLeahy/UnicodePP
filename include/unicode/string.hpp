/** 
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/converter.hpp>
#include <unicode/locale.hpp>
#include <unicode/vector.hpp>
#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	Encapsulates and manages memory for a C-style
	 *	string.
	 */
	template <typename T>
	class CString {
	
	
		private:
		
		
			std::vector<unsigned char> vec;
			
			
			//	Gets a pointer to the beginning of this
			//	string as the appropriate type
			const T * get () const noexcept {
			
				return reinterpret_cast<const T *>(vec.data());
			
			}
			
			
			//	Gets the length in bytes, excluding the
			//	null terminater
			std::size_t size () const noexcept {
			
				return vec.size()-sizeof(T);
			
			}
			
			
		public:
		
		
			/**
			 *	\cond
			 */
		
		
			CString (std::vector<unsigned char> vec) : vec(std::move(vec)) {
			
				//	Null terminate
				for (std::size_t i=0;i<sizeof(T);++i) this->vec.push_back(0);
			
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
			operator const T * () const noexcept {
			
				return get();
			
			}
			
			
			/**
			 *	Determines the size of the string in characters.
			 *
			 *	Unlike calling std::strlen on an actual C-style
			 *	string, this operation is O(1).
			 *
			 *	\return
			 *		The size of the string in characters.
			 */
			std::size_t Size () const noexcept {
			
				return size()/sizeof(T);
			
			}
			
			
			/** 
			 *	Gets a begin iterator for this string.
			 *
			 *	\return
			 *		A pointer typed begin iterator for this
			 *		string.
			 */
			const T * begin () const noexcept {
			
				return get();
			
			}
			
			
			/**
			 *	Gets an end iterator for this string.
			 *
			 *	\return
			 *		A pointer typed end iterator for this
			 *		string.
			 */
			const T * end () const noexcept {
			
				return get()+Size();
			
			}
	
	
	};


	/**
	 *	Encapsulates a Unicode string.
	 */
	class String {
	
	
		private:
		
		
			std::vector<CodePoint> cps;
			const Locale * locale;
			
			
			std::vector<unsigned char> to_c_string (std::size_t) const;
			void from_c_string (const void *, std::size_t);
			
			
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
			
			
			//	The type of character used by the operating
			//	system
			typedef
			#ifdef _WIN32
			wchar_t
			#else
			char
			#endif
			os_type;
			
			
			template <typename T>
			using decay=typename std::decay<T>::type;
			
			
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
				this->cps=std::move(cps);
				
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
			 *	Creates a string from a wide C string.
			 *
			 *	The encoding used to decode the string varies
			 *	depending on the width of wchar_t on this platform:
			 *
			 *	-	If the width of wchar_t is 8 bits, UTF-8
			 *		will be used.
			 *	-	If the width of wchar_t is 16 bits, UTF-16
			 *		will be used.
			 *	-	If the width of wchar_t is 32 bits, the string
			 *		will be copied directly.
			 *
			 *	Otherwise an exception will be thrown.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		wide characters.
			 */
			String (const wchar_t * str);
			/**
			 *	Overwrites this string with a wide C string.
			 *
			 *	The encoding used to decode the string varies
			 *	depending on the width of wchar_t on this platform:
			 *
			 *	-	If the width of wchar_t is 8 bits, UTF-8
			 *		will be used.
			 *	-	If the width of wchar_t is 16 bits, UTF-16
			 *		will be used.
			 *	-	If the width of wchar_t is 32 bits, the string
			 *		will be copied directly.
			 *
			 *	Otherwise an exception will be thrown.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		wide characters.
			 */
			String & operator = (const wchar_t * str);
			/**
			 *	Creates a string from a UTF-16 string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-16 characters.
			 */
			String (const char16_t * str);
			/**
			 *	Overwrites this string with a UTF-16 string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-16 characters.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator = (const char16_t * str);
			/**
			 *	Creates a string from a UTF-32 string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-32 characters.
			 */
			String (const char32_t * str);
			/**
			 *	Overwrites this string with a UTF-32 string.
			 *
			 *	\param [in] str
			 *		A pointer to a null-terminated sequence of
			 *		UTF-32 characters.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator = (const char32_t * str);
			/**
			 *	Creates a string from a C++-style string.
			 *
			 *	The encoding that is used to decode the string is
			 *	chosen based on the width of the string's characters
			 *	(i.e. \em T).
			 *
			 *	-	If \c sizeof(T)==1 UTF-8 is used
			 *	-	If \c sizeof(T)==2 UTF-16 is used
			 *	-	If \c sizeof(T)==4 UTF-32 is used
			 *
			 *	If the width of \em T does not match any of the
			 *	criteria above, an exception is thrown.
			 *
			 *	\tparam T
			 *		The type of the characters of the string.
			 *
			 *	\param [in] str
			 *		The string.
			 */
			template <typename T>
			String (const std::basic_string<T> & str) : String(str.c_str()) {	}
			/**
			 *	Overwrites this string with a a C++-style string.
			 *
			 *	The encoding that is used to decode the string is
			 *	chosen based on the width of the string's characters
			 *	(i.e. \em T).
			 *
			 *	-	If \c sizeof(T)==1 UTF-8 is used
			 *	-	If \c sizeof(T)==2 UTF-16 is used
			 *	-	If \c sizeof(T)==4 UTF-32 is used
			 *
			 *	If the width of \em T does not match any of the
			 *	criteria above, an exception is thrown.
			 *
			 *	\tparam T
			 *		The type of the characters of the string.
			 *
			 *	\param [in] str
			 *		The string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			template <typename T>
			String & operator = (const std::basic_string<T> & str) {
			
				locale=nullptr;
				from_c_string(str.c_str(),sizeof(T));
				
				return *this;
			
			}
			
			
			/**
			 *	Creates a string by converting some object to a
			 *	string.
			 *
			 *	\tparam T
			 *		The type of the object to convert.
			 *
			 *	\param [in] obj
			 *		The object to convert.
			 */
			template <typename T, typename=typename std::enable_if<IsConvertible<T>::Value>::type>
			String (const T & obj) : cps(Converter<T>{}(obj)), locale(nullptr) {	}
			/**
			 *	Creates a string by converting some object to a string
			 *	according to the rules of some locale, which then becomes
			 *	the locale of the newly-created string.
			 *
			 *	\tparam T
			 *		The type of the object to convert.
			 *
			 *	\param [in] obj
			 *		The object to convert.
			 *	\param [in] locale
			 *		The locale.
			 */
			template <typename T, typename=typename std::enable_if<IsConvertible<T>::Value>::type>
			String (const T & obj, const Locale & locale)
				:	cps(Converter<T>(locale)(obj)),
					locale(&locale)
			{	}
			/**
			 *	Overwrites this string with the result of converting
			 *	some object to a string.
			 *
			 *	\tparam T
			 *		The type of object to convert.
			 *
			 *	\param [in] obj
			 *		The object to convert.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			template <typename T>
			typename std::enable_if<IsConvertible<T>::Value,String &>::type operator = (const T & obj) {
			
				locale=nullptr;
				cps=Converter<T>{}(obj);
				
				return *this;
			
			}
			
			
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
			 *	Determines how many code points are in this string.
			 *
			 *	\return
			 *		The number of code points in this string.
			 */
			std::size_t Size () const noexcept {
			
				return cps.size();
			
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
			 *	Swaps this string with another string.
			 *
			 *	\param [in] other
			 *		The string to swap with this string.
			 */
			void swap (String & other) noexcept {
			
				std::swap(cps,other.cps);
				std::swap(locale,other.locale);
			
			}
			
			
			/**
			 *	Converts this string to a C-style string.
			 *
			 *	\tparam T
			 *		The type of character that shall be used
			 *		for the C string.  An encoding appropriate
			 *		to the character size will be chosen.  If one
			 *		cannot be found, an exception will be thrown.
			 *		Defaults to char if not specified.
			 *
			 *	\return
			 *		An object which is implicitly convertible to
			 *		const T *, but which manages the necessary
			 *		dynamically-allocated memory using the RAII
			 *		idiom.
			 */
			template <typename T=char>
			CString<decay<T>> ToCString () const {
			
				typedef decay<T> type;
				
				return CString<type>(to_c_string(sizeof(type)));
			
			}
			
			
			/**
			 *	Converts this string to a C-style string for consumption
			 *	by the underlying operating system's APIs.
			 *
			 *	For Windows this results in a UTF-16LE encoded C-style
			 *	string with a character type of wchar_t, for all other
			 *	systems this results in a UTF-8 encoded C-style string
			 *	with a character type of char.
			 *
			 *	\return
			 *		An object which is implicitly convertible to the
			 *		underlying operating systems' string type, but which
			 *		manages the necessary dynamically-allocated memory
			 *		using the RAII idiom.
			 */
			CString<os_type> ToOSString () const;
			
			
			/**
			 *	Converts this string to a C++-style string.
			 *
			 *	\tparam T
			 *		The type of character that shall be used
			 *		for the C++ string.  An encoding appropriate
			 *		to the character size will be chosen.  If one
			 *		cannot be found, an exception will be thrown.
			 *		Defaults to char (resulting in a return type
			 *		of std::string) if not specified.
			 *	
			 *	\return
			 *		A C++-style string.
			 */
			template <typename T=char>
			std::basic_string<decay<T>> ToString () const {
			
				typedef decay<T> type;
			
				auto vec=to_c_string(sizeof(type));
				
				return std::basic_string<type>(
					reinterpret_cast<const type *>(Begin(vec)),
					reinterpret_cast<const type *>(End(vec))
				);
			
			}
			
			
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
			
			
			/**
			 *	Appends another string to the end of this string.
			 *
			 *	\param [in] str
			 *		The string to append to the end of this string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator << (const String & str) &;
			/**
			 *	Appends a code point to the end of this string.
			 *
			 *	\param [in] cp
			 *		The code point to append to the end of this
			 *		string.
			 *
			 *	\return
			 *		A reference to this string.
			 */
			String & operator << (CodePoint cp) &;
	
	
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
	
	
	inline String operator + (String a, const String & b) {
	
		a << b;
		
		return a;
	
	}
	
	
	inline String operator + (String a, CodePoint b) {
	
		a << b;
		
		return a;
	
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
	
	
	inline void swap (Unicode::String & a, Unicode::String & b) noexcept {
	
		a.swap(b);
	
	}


}


/**
 *	\endcond
 */
