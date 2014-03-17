/**
 *	\file
 */
 
 
#pragma once


#include <unicode/string.hpp>
#include <iostream>


namespace Unicode {


	/**
	 *	Outputs a Unicode string to an output stream.
	 *
	 *	\param [in] os
	 *		The output stream.
	 *	\param [in] str
	 *		The Unicode string.
	 *
	 *	\return
	 *		\em os.
	 */
	template <typename CharT, typename Traits>
	std::basic_ostream<CharT,Traits> & operator << (std::basic_ostream<CharT,Traits> & os, const String & str) {
	
		return os << str.ToString<CharT>();
	
	}


}
