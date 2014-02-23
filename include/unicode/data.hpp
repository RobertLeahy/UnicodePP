/**
 *	\file
 */
 
 
#pragma once
 

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <optional>
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
			std::uint32_t Low;
			/**
			 *	The upper bound of the range.
			 *
			 *	Inclusive.
			 */
			std::uint32_t High;
	
	
	};
	
	
	/**
	 *	Encapsulates a condition.
	 */
	class Condition {
	
	
		private:
		
		
			std::vector<char> name;
	
	
		public:
		
		
			Condition () = delete;
			Condition (bool, std::vector<char>);
			Condition (const Condition &) = default;
			Condition (Condition &&) = default;
			Condition & operator = (const Condition &) = default;
			Condition & operator = (Condition &&) = default;
		
		
			/**
			 *	\em true if the sense of the condition
			 *	is inverted, \em false otherwise.
			 */
			bool Negated;
			
			
			operator const char * () const noexcept {
			
				return &name[0];
			
			}
	
	
	};


	/**
	 *	Encapsulates a single data item from a Unicode
	 *	data file.
	 */
	class Item {
	
	
		private:
		
		
			std::vector<char> str;
			
			
		public:
		
		
			Item () = delete;
			Item (std::vector<char>);
			Item (const Item &) = default;
			Item (Item &&) = default;
			Item & operator = (const Item &) = default;
			Item & operator = (Item &&) = default;
			
			
			/**
			 *	Attempts to retrieve the code point associated
			 *	with this 
			 */
			std::optional<std::uint32_t> CodePoint () const noexcept;
			std::optional<std::vector<std::uint32_t>> CodePoints () const;
			std::optional<Unicode::Range> Range () const noexcept;
			std::vector<Condition> Conditions () const;
			
			
			const char * begin () const noexcept;
			const char * end () const noexcept;
			
			
			/**
			 *	Retrieves the C string this item encapsulates.
			 *
			 *	\return
			 *		A C string.
			 */
			operator const char * () const noexcept {
			
				return &str[0];
			
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
			
			
			static std::optional<Line> Get (std::vector<char> str);
			
			
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
