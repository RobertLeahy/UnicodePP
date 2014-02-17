/** 
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <cstddef>
#include <utility>
#include <vector>


namespace Unicode {


	class String {
	
	
		private:
		
		
			std::vector<CodePoint> cps;
			
			
			void trim_front () noexcept;
			void trim_rear () noexcept;
			
			
		public:
		
		
			/**
			 *	Creates an empty string.
			 */
			String () = default;
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
			String (std::vector<CodePoint> cps) noexcept : cps(std::move(cps)) {	}
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
	
	
	};


}
