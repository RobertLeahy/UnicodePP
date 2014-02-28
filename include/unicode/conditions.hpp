/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>


namespace Unicode {


	/**
	 *	\cond
	 */
	
	
	namespace Conditions {
	
	
		bool FinalSigma (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
		bool AfterSoftDotted (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
		bool MoreAbove (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
		bool BeforeDot (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
		bool AfterI (const CodePoint *, const CodePoint *, const CodePoint *, const Locale &) noexcept;
	
	
	}
	
	
	/**
	 *	\endcond
	 */
	
	
}
