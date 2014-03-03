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


	class CString {
	
	
		private:
		
		
			std::vector<unsigned char> vec;
			
			
		public:
			
			
			CString (std::vector<unsigned char> vec) : vec(std::move(vec)) {
			
				vec.push_back(0);
			
			}
			
			
			operator const char * () const noexcept {
			
				return reinterpret_cast<const char *>(vec.data());
			
			}
			
			
			std::size_t Size () const noexcept {
			
				return vec.size()-1;
			
			}
	
	
	};


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
		
		
			explicit String (const Locale * locale=nullptr) noexcept : locale(locale) {	}
			String (std::vector<CodePoint> cps, const Locale * locale=nullptr) noexcept : cps(std::move(cps)), locale(locale) {	}
			
			
			String (const String &) = default;
			String (String &&) = default;
			String & operator = (const String &) = default;
			String & operator = (String &&) = default;
			
			
			String & operator = (std::vector<CodePoint> cps) noexcept {
			
				locale=nullptr;
				cps=std::move(cps);
				
				return *this;
			
			}
			
			
			String (const char * str);
			String & operator = (const char * str);
			
			
			const CodePoint * begin () const noexcept {
			
				return Begin(cps);
			
			}
			
			
			CodePoint * begin () noexcept {
			
				return Begin(cps);
			
			}
			
			
			const CodePoint * end () const noexcept {
			
				return End(cps);
			
			}
			
			
			CodePoint * end () noexcept {
			
				return End(cps);
			
			}
			
			
			const Locale & GetLocale () const noexcept {
			
				return (locale==nullptr) ? Locale::Get() : *locale;
			
			}
			
			
			void SetLocale (const Locale & locale) noexcept {
			
				this->locale=&locale;
			
			}
			
			
			std::vector<CodePoint> & CodePoints () noexcept {
			
				return cps;
			
			}
			
			
			const std::vector<CodePoint> & CodePoints () const noexcept {
			
				return cps;
			
			}
			
			
			CString ToCString (bool utf8=true) const;
			
			
			String TrimFront () const &;
			String & TrimFront () & noexcept;
			String TrimFront () && noexcept;
			
			String TrimRear () const &;
			String & TrimRear () & noexcept;
			String TrimRear () && noexcept;
			
			String Trim () const &;
			String & Trim () & noexcept;
			String Trim () && noexcept;
			
			
			String ToUpper (bool full=true) const &;
			String & ToUpper (bool full=true) &;
			
			String ToLower (bool full=true) const &;
			String & ToLower (bool full=true) &;
			
			String ToCaseFold (bool full=true) const &;
			String & ToCaseFold (bool full=true) &;
			
			
			bool IsNFD () const noexcept;
			bool IsNFC () const noexcept;
			
			
			String ToNFD () const &;
			String & ToNFD () &;
			String ToNFD () &&;
			
			String ToNFC () const &;
			String & ToNFC () &;
			String ToNFC () &&;
			
			
			bool Equals (const String & other, bool case_sensitive=true) const;
			bool Equals (const char * str, bool case_sensitive=true) const;
	
	
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
