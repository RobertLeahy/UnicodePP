/**
 *	\file
 */
 
 
#pragma once


#include <algorithm>
#include <functional>
#include <utility>


namespace Unicode {


	/**
	 *	Searches a sorted range of elements for the
	 *	first element equal to some value.
	 *
	 *	\param [in] begin
	 *		An iterator to the beginning of the
	 *		range to search.
	 *	\param [in] end
	 *		An iterator to the end of the range
	 *		to search.
	 *	\param [in] value
	 *		The value to search for.
	 *	\param [in] cmp
	 *		A function which takes a value of the
	 *		type of the underlying range as its first
	 *		argument, and a value of type \em T as its
	 *		second argument, and returns \em true if
	 *		the first value should be considered less
	 *		than the second value, \em false otherwise.
	 *	\param [in] eq
	 *		A function which takes a value of the type
	 *		of the underlying range as its first argument,
	 *		and a value of type \em T as its second argument,
	 *		and returns \em true if they should be
	 *		considered equal, \em false otherwise.
	 *
	 *	\return
	 *		An iterator to the first element equal to
	 *		\em value, or \em end if there was no such
	 *		value.
	 */
	template <typename Iter, typename T, typename Compare, typename Equality>
	Iter BinarySearch (Iter begin, Iter end, const T & value, Compare cmp, Equality eq) noexcept(
		noexcept(
			std::lower_bound(
				std::move(begin),
				end,
				value,
				std::move(cmp)
			)
		) &&
		noexcept(eq(*begin,value))
	) {
	
		auto iter=std::lower_bound(
			std::move(begin),
			end,
			value,
			std::move(cmp)
		);
		
		if (iter==end) return iter;
		
		if (!eq(*iter,value)) return end;
		
		return iter;
	
	}


	/**
	 *	Searches a sorted range of elements for the
	 *	first element equal to some value.
	 *
	 *	\param [in] begin
	 *		An iterator to the beginning of the
	 *		range to search.
	 *	\param [in] end
	 *		An iterator to the end of the range
	 *		to search.
	 *	\param [in] value
	 *		The value to search for.
	 *
	 *	\return
	 *		An iterator to the first element equal to
	 *		\em value, or \em end if there was no such
	 *		value.
	 */
	template <typename Iter, typename T>
	Iter BinarySearch (Iter begin, Iter end, const T & value) noexcept(
		noexcept(
			BinarySearch(
				std::move(begin),
				std::move(end),
				value,
				std::less<T>{},
				std::equal_to<T>{}
			)
		)
	) {
	
		return BinarySearch(
			std::move(begin),
			std::move(end),
			value,
			std::less<T>{},
			std::equal_to<T>{}
		);
	
	}


}
