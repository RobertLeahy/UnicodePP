/**
 *	\file
 */
 
 
#pragma once
 

#include <unicode/codepoint.hpp>
#include <cstddef>
#include <cstdint>
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
	 *	Contains information about a collation element
	 *	as read from a data file.
	 */
	class CollationElementInfo {
	
	
		public:
		
		
			/**
			 *	\em true if the collation element is
			 *	variable.
			 */
			bool Variable;
			/**
			 *	The weights which make up the collation
			 *	element.
			 */
			std::vector<std::uint16_t> Weights;
	
	
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
			std::vector<CollationElementInfo> CollationElements () const;
			
			
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
	
	
	class File;
	
	
	/**
	 *	Iterates over the lines in a Unicode data
	 *	file.
	 */
	class FileIterator {
	
	
		private:
		
		
			File & file;
			std::optional<Line> line;
			
			
		public:
		
		
			/**
			 *	\cond
			 */
		
		
			FileIterator (File &, bool);
			
			
			/**
			 *	\endcond
			 */
			
			
			/**
			 *	Compares this iterator for equality with
			 *	another iterator.
			 *
			 *	This comparison is only meaningful if either
			 *	this iterator, or \em other is an end iterator.
			 *
			 *	\param [in] other
			 *		The other iterator.
			 *
			 *	\return
			 *		\em true if this iterator and \em other
			 *		should be considered equal, \em false
			 *		otherwise.
			 */
			bool operator == (const FileIterator & other) const noexcept;
			/**
			 *	Compares this iterator for inequality with
			 *	another iterator.
			 *
			 *	This comparison is only meaningful if either
			 *	this iterator, or \em other is an end iterator.
			 *
			 *	\param [in] other
			 *		The other iterator.
			 *
			 *	\return
			 *		\em true if this iterator and \em other
			 *		should not be considered equal, \em false
			 *		otherwise.
			 */
			bool operator != (const FileIterator & other) const noexcept;
			
			
			/**
			 *	Dereferences this iterator.
			 *
			 *	If this iterator points past the end of
			 *	the file, the behaviour of this function is
			 *	undefined.
			 *
			 *	\return
			 *		A pointer to the current line.
			 */
			const Line * operator -> () const noexcept;
			/**
			 *	Dereferences this iterator.
			 *
			 *	If this iterator points past the end of
			 *	the file, the behaviour of this function is
			 *	undefined.
			 *
			 *	\return
			 *		A reference to the current line.
			 */
			const Line & operator * () const noexcept;
			
			
			/**
			 *	Increments this iterator so it points to the
			 *	next line in the underlying file.
			 *
			 *	\return
			 *		A reference to this iterator.
			 */
			FileIterator & operator ++ ();
			/**
			 *	Retrieves a copy of this iterator, and then
			 *	increments this iterator.
			 *
			 *	Using the returned iterator for any purpose
			 *	other than dereferencing results in undefined
			 *	behaviour.
			 *
			 *	\return
			 *		A copy of this iterator before incrementing
			 *		it.
			 */
			FileIterator operator ++ (int);
	
	
	};
	
	
	/**
	 *	Reads from a Unicode data file.
	 */
	class File {
	
	
		private:
		
		
			static constexpr std::size_t size=256;
		
		
			std::fstream file;
			const char * b_iter;
			const char * e_iter;
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
			
			
			/**
			 *	Fetches an iterator to the current point
			 *	in the file.
			 *
			 *	\return
			 *		An iterator to the current point in the
			 *		file.
			 */
			FileIterator begin ();
			/**
			 *	Fetches an iterator to the end of the file.
			 *
			 *	\return
			 *		An iterator to the end of the file.
			 */
			FileIterator end ();
	
	
	};


}
