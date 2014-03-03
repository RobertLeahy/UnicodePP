/**
 *	\file
 */
 
 
#pragma once


#include <vector>


namespace Unicode {


	/**
	 *	\cond
	 */


	template <typename T>
	const T * Begin (const std::vector<T> & vec) noexcept {
	
		return vec.data();
	
	}
	
	
	template <typename T>
	T * Begin (std::vector<T> & vec) noexcept {
	
		return vec.data();
	
	}
	
	
	template <typename T>
	const T * End (const std::vector<T> & vec) noexcept {
	
		return vec.data()+vec.size();
	
	}
	
	
	template <typename T>
	T * End (std::vector<T> & vec) noexcept {
	
		return vec.data()+vec.size();
	
	}
	
	
	/**
	 *	\endcond
	 */


}
