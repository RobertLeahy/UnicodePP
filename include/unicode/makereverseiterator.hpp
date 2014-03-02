/**
 *	\file
 */
 
 
#pragma once


#include <iterator>
#include <type_traits>
#include <utility>


namespace Unicode {


	/**
	 *	Makes an iterator into a reverse iterator.
	 *
	 *	\tparam T
	 *		The type of the iterator-in-question.
	 *
	 *	\param [in] iter
	 *		The iterator-in-question.
	 *
	 *	\return
	 *		\em iter reversed.
	 */
	template <typename T>
	std::reverse_iterator<T> MakeReverseIterator (T iter) noexcept(
		std::is_nothrow_constructible<
			std::reverse_iterator<T>,
			T
		>::value
	) {
	
		return std::reverse_iterator<T>(std::move(iter));
	
	}


}
