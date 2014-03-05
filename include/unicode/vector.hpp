/**
 *	\file
 */
 
 
#pragma once


#include <optional>
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
	
	
	template <typename T>
	const T * Begin (const T * begin, const std::optional<std::vector<T>> & opt) noexcept {
	
		return opt ? Begin(*opt) : begin;
	
	}
	
	
	template <typename T>
	T * Begin (T * begin, std::optional<std::vector<T>> & opt) noexcept {
	
		return opt ? Begin(*opt) : begin;
	
	}
	
	
	template <typename T>
	const T * End (const T * end, const std::optional<std::vector<T>> & opt) noexcept {
	
		return opt ? End(*opt) : end;
	
	}
	
	
	template <typename T>
	T * End (T * end, std::optional<std::vector<T>> & opt) noexcept {
	
		return opt ? End(*opt) : opt;
	
	}
	
	
	/**
	 *	\endcond
	 */


}
