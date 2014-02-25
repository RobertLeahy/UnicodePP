/**
 *	\file
 */
 
 
#pragma once
 

#include <unicode/codepoint.hpp>
#include <cstddef>
#include <fstream>
#include <optional>
#include <string>
#include <vector>
 
 
namespace Unicode {


	/**
	 *	Encapsulates a range of Unicode code
	 *	points.
	 */
	class Range {
	
	
		public:
		
		
			/**
			 *	The lower bound of the range.
			 *
			 *	Inclusive.
			 */
			CodePoint::Type Low;
			/**
			 *	The upper bound of the range.
			 *
			 *	Inclusive.
			 */
			CodePoint::Type High;
	
	
	};
	
	
	/**
	 *	Contains information about a condition as
	 *	read from a data file.
	 */
	class ConditionInfo {
	
	
		public:
		
		
			/**
			 *	\em true if the sense of the condition
			 *	is inverted, \em false otherwise.
			 */
			bool Negated;
			/**
			 *	The name of the condition.
			 */
			std::string Name;
	
	
	};


	/**
	 *	Encapsulates a single data item from a Unicode
	 *	data file.
	 */
	class Item {
	
	
		private:
		
		
			std::string str;
			
			
		public:
		
		
			Item () = delete;
			Item (std::string) noexcept;
			Item (const Item &) = default;
			Item (Item &&) = default;
			Item & operator = (const Item &) = default;
			Item & operator = (Item &&) = default;
			
			
			/**
			 *	Attempts to retrieve the code point associated
			 *	with this 
			 */
			std::optional<Unicode::CodePoint::Type> CodePoint () const noexcept;
			std::optional<std::vector<Unicode::CodePoint::Type>> CodePoints () const;
			std::optional<Unicode::Range> Range () const noexcept;
			std::vector<ConditionInfo> Conditions () const;
			
			
			const char * begin () const noexcept;
			const char * end () const noexcept;
			
			
			const std::string & Get () const noexcept {
			
				return str;
			
			}
	
	
	};
	
	
	/**
	 *	Encapsulates a non-comment line from a Unicode
	 *	data file.
	 */
	class Line {
	
	
		private:
		
		
			std::vector<Item> items;
			
			
		public:
		
		
			Line () = delete;
			Line (std::vector<Item>) noexcept;
			Line (const Line &) = default;
			Line (Line &&) = default;
			Line & operator = (const Line &) = default;
			Line & operator = (Line &&) = default;
			
			
			/**
			 *	\cond
			 */
			
			
			static std::optional<Line> Get (std::string str);
			
			
			/**
			 *	\endcond
			 */
			 
			 
			std::size_t size () const noexcept {
			
				return items.size();
			
			}
			
			
			Item & operator [] (std::size_t i) noexcept {
			
				return items[i];
			
			}
			
			
			const Item & operator [] (std::size_t i) const noexcept {
			
				return items[i];
			
			}
		
	
	};
	
	
	/**
	 *	Reads from a Unicode data file.
	 */
	class File {
	
	
		private:
		
		
			static constexpr std::size_t size=256;
		
		
			std::fstream file;
			const char * begin;
			const char * end;
			char buffer [size];
			
			
			void read ();
			
			
		public:
		
		
			File () = delete;
			File (const File &) = delete;
			File (File &&) = delete;
			File & operator = (const File &) = delete;
			File & operator = (File &&) = delete;
			
			
			/**
			 *	Opens a Unicode data file for reading.
			 *
			 *	\param [in] filename
			 *		A C string containing the name of
			 *		the file to open for reading.
			 */
			File (const char * filename);
		
		
			/**
			 *	Gets the next line from the file.
			 *
			 *	\return
			 *		The next line from the file unless
			 *		the end of the file has been
			 *		reached, in which case a disengaged
			 *		std::optional is returned.
			 */
			std::optional<Line> Get ();
	
	
	};


}
