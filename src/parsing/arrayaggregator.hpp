#pragma once


#include <cstddef>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>


class Array {


	public:
	
	
		std::size_t Offset;
		std::size_t Size;


};


template <typename T>
class ArrayAggregatorIterator {


	private:
	
	
		typedef typename std::vector<std::vector<T>>::const_iterator global_type;
		typedef typename std::vector<T>::const_iterator local_type;
	
	
		class Local {
		
		
			public:
			
			
				local_type Begin;
				local_type End;
				
				
				Local () = delete;
				Local (local_type begin, local_type end) noexcept : Begin(std::move(begin)), End(std::move(end)) {	}
		
		
		};
	
	
		global_type global;
		global_type end;
		std::optional<Local> local;
		
		
		void get_local () noexcept {
		
			local.emplace(
				global->begin(),
				global->end()
			);
		
		}
		
		
		void next () noexcept {
		
			++local->Begin;
			
			if (local->Begin==local->End) {
			
				local=std::nullopt;
			
				for (
					++global;
					(global!=end) &&
					(global->size()==0);
					++global
				);
				
				if (global!=end) get_local();
			
			}
		
		}
		
		
	public:
	
	
		ArrayAggregatorIterator (const std::vector<std::vector<T>> & vec, bool is_end) noexcept
			:	global(is_end ? vec.end() : vec.begin()), end(vec.end())
		{
		
			if (!(is_end || (vec.size()==0))) get_local();
		
		}
		
		
		bool operator == (const ArrayAggregatorIterator & other) const noexcept {
		
			bool retr=global==other.global;
			
			if (retr && local) retr=local->Begin==other.local->Begin;
			
			return retr;
		
		}
		
		
		bool operator != (const ArrayAggregatorIterator & other) const noexcept {
		
			return !(*this==other);
		
		}
		
		
		const T * operator -> () const noexcept {
		
			return &(*(local->Begin));
		
		}
		
		
		const T & operator * () const noexcept {
		
			return *(local->Begin);
		
		}
		
		
		ArrayAggregatorIterator & operator ++ () noexcept {
		
			next();
			
			return *this;
		
		}
		
		
		ArrayAggregatorIterator operator ++ (int) noexcept {
		
			auto retr=*this;
			
			next();
			
			return retr;
		
		}


};


template <typename T>
class ArrayAggregator {


	private:
	
	
		std::vector<
			std::vector<T>
		> elements;
		std::optional<
			std::vector<
				std::size_t
			>
		> resolved;
		
		
		void check () const {
		
			if (resolved) throw std::logic_error("Aggregator has been resolved");
		
		}
		
		
		void resolve () {
		
			if (resolved) return;
		
			resolved.emplace();
			auto & vec=*resolved;
			
			std::size_t offset=0;
			for (auto & v : elements) {
			
				vec.push_back(offset);
				offset+=v.size();
			
			}
		
		}
		
	
	public:
	
	
		typedef std::size_t Key;
		
		
		Key Add () {
		
			check();
			
			auto retr=elements.size();
			
			elements.emplace_back();
			
			return retr;
		
		}
	
	
		Key Add (T i) {
		
			check();
		
			auto retr=elements.size();
			
			std::vector<T> vec;
			vec.push_back(std::move(i));
			elements.push_back(std::move(vec));
			
			return retr;
		
		}
		
		
		Key Add (std::vector<T> vec) {
		
			check();
		
			auto retr=elements.size();
			
			elements.emplace_back(
				std::make_move_iterator(vec.begin()),
				std::make_move_iterator(vec.end())
			);
			
			return retr;
		
		}
		
		
		void Add (Key key, T i) {
		
			check();
		
			elements[key].push_back(std::move(i));
		
		}
		
		
		void Add (Key key, std::vector<T> vec) {
		
			check();
		
			auto & v=elements[key];
			
			v.insert(
				v.end(),
				std::make_move_iterator(vec.begin()),
				std::make_move_iterator(vec.end())
			);	
		
		}
		
		
		Array Get (Key key) {
		
			resolve();
			
			return Array{
				(*resolved)[key],
				elements[key].size()
			};
		
		}
		
		
		std::vector<T> & operator [] (Key key) {
		
			check();
			
			return elements[key];
		
		}
		
		
		const std::vector<T> & operator [] (Key key) const noexcept {
		
			return elements[key];
		
		}
		
		
		ArrayAggregatorIterator<T> begin () const noexcept {
		
			return ArrayAggregatorIterator<T>(elements,false);
		
		}
		
		
		ArrayAggregatorIterator<T> end () const noexcept {
		
			return ArrayAggregatorIterator<T>(elements,true);
		
		}


};
