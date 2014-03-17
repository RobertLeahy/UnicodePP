#include <unicode/caseconverter.hpp>
#include <unicode/collator.hpp>
#include <unicode/comparer.hpp>
#include <unicode/endianencoding.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/string.hpp>
#include <unicode/utf8.hpp>
#include <unicode/utf16.hpp>
#include <unicode/utf32.hpp>
#include <algorithm>
#include <stdexcept>


namespace Unicode {


	[[noreturn]]
	static void no_encoding () {
	
		throw std::logic_error("No encoding for code units of that size");
	
	}


	std::vector<unsigned char> String::to_c_string (std::size_t cu_size) const {
	
		switch (cu_size) {
		
			//	UTF-8
			case 1:
				return UTF8{}.Encode(begin(),end());
			//	UTF-16
			case 2:{
				UTF16 encoder(EndianEncoding::Detect());
				encoder.OutputBOM=false;
				return encoder.Encode(begin(),end());
			}
			//	UTF-32
			case 4:{
				UTF32 encoder(EndianEncoding::Detect());
				encoder.OutputBOM=false;
				return encoder.Encode(begin(),end());
			}
			//	No known encoding
			default:
				no_encoding();
		
		}
	
	}


	template <typename T>
	T find_non_white_space (T begin, T end, const Locale & locale) noexcept {
	
		return std::find_if(
			std::move(begin),
			std::move(end),
			[&] (CodePoint cp) noexcept {
			
				return !cp.IsWhiteSpace();
			
			}
		);
	
	}


	void String::trim_front (const Locale & locale) noexcept {
	
		auto begin=cps.begin();
		auto end=find_non_white_space(begin,cps.end(),locale);
		cps.erase(begin,end);
	
	}
	
	
	void String::trim_rear (const Locale & locale) noexcept {
	
		auto begin=cps.rbegin();
		auto end=find_non_white_space(begin,cps.rend(),locale);
		cps.resize(cps.size()-static_cast<std::size_t>(end-begin));
	
	}
	
	
	void String::trim (const Locale & locale) noexcept {
	
		trim_rear(locale);
		trim_front(locale);
	
	}
	
	
	std::vector<CodePoint> String::to_upper (bool full) const {
	
		return CaseConverter(GetLocale(),full).ToUpper(begin(),end());
	
	}
	
	
	std::vector<CodePoint> String::to_lower (bool full) const {
	
		return CaseConverter(GetLocale(),full).ToLower(begin(),end());
	
	}
	
	
	std::vector<CodePoint> String::to_case_fold (bool full) const {
	
		return CaseConverter(GetLocale(),full).Fold(begin(),end());
	
	}
	
	
	bool String::is_nfd (const Locale & locale) const noexcept {
	
		return Normalizer(locale).IsNFD(begin(),end());
	
	}
	
	
	bool String::is_nfc (const Locale & locale) const noexcept {
	
		return Normalizer(locale).IsNFC(begin(),end());
	
	}
	
	
	std::vector<CodePoint> String::to_nfd (const Locale & locale) const {
	
		return Normalizer(locale).ToNFD(begin(),end());
	
	}
	
	
	std::vector<CodePoint> String::to_nfc (const Locale & locale) const {
	
		return Normalizer(locale).ToNFC(begin(),end());
	
	}
	
	
	template <typename T>
	std::size_t strlen (const T * str) {
	
		if (str==nullptr) return 0;
	
		std::size_t retr=0;
		for (;*str!=0;++str,++retr);
		
		return retr;
	
	}
	
	
	template <typename T>
	typename std::enable_if<
		sizeof(T)==1,
		std::vector<CodePoint>
	>::type decode (const T * begin, const T * end) {
	
		return UTF8{}.Decode(begin,end);
	
	}
	
	
	template <typename T>
	typename std::enable_if<
		sizeof(T)==2,
		std::vector<CodePoint>
	>::type decode (const T * begin, const T * end) {
	
		auto order=EndianEncoding::Detect();
		return UTF16(order,order).Decode(begin,end);
	
	}
	
	
	template <typename T>
	typename std::enable_if<
		sizeof(T)==4,
		std::vector<CodePoint>
	>::type decode (const T * begin, const T * end) {
	
		return std::vector<CodePoint>(begin,end);
	
	}
	
	
	template <typename T>
	[[noreturn]]
	typename std::enable_if<
		!((sizeof(T)==1) || (sizeof(T)==2) || (sizeof(T)==4)),
		std::vector<CodePoint>
	>::type decode (const T *, const T *) {
	
		no_encoding();
	
	}
	
	
	template <typename T>
	std::vector<CodePoint> decode (const T * str) {
	
		return decode(str,str+strlen(str));
	
	}
	
	
	String::String (const char * str)
		:	cps(decode(str)),
			locale(nullptr)
	{	}
	
	
	String & String::operator = (const char * str) {
	
		locale=nullptr;
		cps=decode(str);
		
		return *this;
	
	}
	
	
	String::String (const wchar_t * str)
		:	cps(decode(str)),
			locale(nullptr)
	{	}
	
	
	String & String::operator = (const wchar_t * str) {
	
		locale=nullptr;
		cps=decode(str);
		
		return *this;
	
	}
	
	
	String::String (const char16_t * str)
		:	cps(decode(str)),
			locale(nullptr)
	{	}
	
	
	String & String::operator = (const char16_t * str) {
	
		locale=nullptr;
		cps=decode(str);
		
		return *this;
	
	}
	
	
	String::String (const char32_t * str)
		:	cps(decode(str)),
			locale(nullptr)
	{	}
	
	
	String & String::operator = (const char32_t * str) {
	
		locale=nullptr;
		cps=decode(str);
		
		return *this;
	
	}
	
	
	CString<String::os_type> String::ToOSString () const {
	
		return ToCString<os_type>();
	
	}
	
	
	String String::TrimFront () const & {
	
		String retr(*this);
		retr.TrimFront();
		
		return retr;
	
	}
	
	
	String & String::TrimFront () & noexcept {
	
		trim_front(GetLocale());
		
		return *this;
	
	}
	
	
	String String::TrimFront () && noexcept {
	
		String retr(std::move(*this));
		retr.TrimFront();
		
		return retr;
	
	}
	
	
	String String::TrimRear () const & {
	
		String retr(*this);
		retr.TrimRear();
		
		return retr;
	
	}
	
	
	String & String::TrimRear () & noexcept {
	
		trim_rear(GetLocale());
		
		return *this;
	
	}
	
	
	String String::TrimRear () && noexcept {
	
		String retr(std::move(*this));
		retr.TrimRear();
		
		return retr;
	
	}
	
	
	String String::Trim () const & {
	
		String retr(*this);
		retr.Trim();
		
		return retr;
	
	}
	
	
	String & String::Trim () & noexcept {
	
		trim(GetLocale());
		
		return *this;
	
	}
	
	
	String String::Trim () && noexcept {
	
		String retr(std::move(*this));
		retr.Trim();
		
		return retr;
	
	}
	
	
	String String::ToUpper (bool full) const & {
	
		return to_upper(full);
	
	}
	
	
	String & String::ToUpper (bool full) & {
	
		cps=to_upper(full);
		
		return *this;
	
	}
	
	
	String String::ToLower (bool full) const & {
	
		return to_lower(full);
	
	}
	
	
	String & String::ToLower (bool full) & {
	
		cps=to_lower(full);
		
		return *this;
	
	}
	
	
	String String::ToCaseFold (bool full) const & {
	
		return to_case_fold(full);
	
	}
	
	
	String & String::ToCaseFold (bool full) & {
	
		cps=to_case_fold(full);
		
		return *this;
	
	}
	
	
	bool String::IsNFD () const noexcept {
	
		return is_nfd(GetLocale());
	
	}
	
	
	bool String::IsNFC () const noexcept {
	
		return is_nfc(GetLocale());
	
	}
	
	
	String String::ToNFD () const & {
	
		auto & locale=GetLocale();
		
		if (is_nfd(locale)) return *this;
		
		return to_nfd(locale);
	
	}
	
	
	String & String::ToNFD () & {
	
		auto & locale=GetLocale();
		
		if (!is_nfd(locale)) cps=to_nfd(locale);
		
		return *this;
	
	}
	
	
	String String::ToNFD () && {
	
		auto & locale=GetLocale();
		
		if (is_nfd(locale)) return std::move(*this);
		
		return to_nfd(locale);
	
	}
	
	
	String String::ToNFC () const & {
	
		auto & locale=GetLocale();
		
		if (is_nfc(locale)) return *this;
		
		return to_nfc(locale);
	
	}
	
	
	String & String::ToNFC () & {
	
		auto & locale=GetLocale();
		
		if (!is_nfc(locale)) cps=to_nfc(locale);
		
		return *this;
	
	}
	
	
	String String::ToNFC () && {
	
		auto & locale=GetLocale();
		
		if (is_nfc(locale)) return std::move(*this);
		
		return to_nfc(locale);
	
	}
	
	
	bool String::Equals (const String & other, bool case_sensitive) const {
	
		return Comparer(GetLocale(),case_sensitive).Compare(begin(),end(),other.begin(),other.end());
	
	}
	
	
	bool String::Equals (const char * str, bool case_sensitive) const {
	
		return Comparer(GetLocale(),case_sensitive).Compare(begin(),end(),str);
	
	}
	
	
	int String::Compare (const String & other) const {
	
		return Collator(GetLocale()).Compare(begin(),end(),other.begin(),other.end());
	
	}
	
	
	String & String::operator << (const String & str) & {
	
		cps.insert(
			cps.end(),
			str.cps.begin(),
			str.cps.end()
		);
		
		return *this;
	
	}
	
	
	String & String::operator << (CodePoint cp) & {
	
		cps.push_back(cp);
		
		return *this;
	
	}


}
