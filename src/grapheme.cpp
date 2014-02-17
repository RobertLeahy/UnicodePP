#include <unicode/grapheme.hpp>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>


namespace Unicode {


	void Grapheme::destroy () noexcept {
	
		if (mode!=Mode::Owning) return;
		
		std::free(ptr);
		ptr=nullptr;
	
	}
	
	
	static std::size_t multiply (std::size_t a, std::size_t b) {
	
		//	Check to make sure multiplication is
		//	safe
		if (
			(a==0) ||
			(b==0) ||
			(a==1) ||
			(b==1) ||
			((std::numeric_limits<std::size_t>::max()/a)>=b)
		) return a*b;
		
		throw std::overflow_error("Overflow attempting to allocate memory");
	
	}
	
	
	void Grapheme::allocate () {
	
		if ((ptr=reinterpret_cast<CodePoint *>(
			std::malloc(
				multiply(
					count,
					sizeof(CodePoint)
				)
			)
		))==nullptr) throw std::bad_alloc{};
	
	}
	
	
	void Grapheme::copy (const Grapheme & other) {
	
		mode=other.mode;
		count=other.count;
		
		switch (mode) {
		
			case Mode::Single:
				cp=other.cp;
				break;
				
			case Mode::NonOwning:
				c_ptr=other.c_ptr;
				break;
				
			default:
				allocate();
				std::memcpy(
					ptr,
					other.ptr,
					count
				);
				break;
		
		}
	
	}
	
	
	void Grapheme::move (Grapheme && other) noexcept {
	
		mode=other.mode;
		count=other.count;
		
		switch (mode) {
		
			case Mode::Single:
				cp=other.cp;
				break;
				
			case Mode::NonOwning:
				c_ptr=other.c_ptr;
				break;
				
			default:
				ptr=other.ptr;
				other.ptr=nullptr;
				break;
		
		}
	
	}


	Grapheme::Grapheme () noexcept : mode(Mode::NonOwning), ptr(nullptr), count(0) {	}
	
	
	Grapheme::~Grapheme () noexcept {
	
		destroy();
	
	}
	
	
	Grapheme::Grapheme (const Grapheme & other) {
	
		copy(other);
	
	}
	
	
	Grapheme::Grapheme (Grapheme && other) noexcept {
	
		move(std::move(other));
	
	}
	
	
	Grapheme & Grapheme::operator = (const Grapheme & other) {
	
		if (&other!=this) {
		
			destroy();
			
			copy(other);
		
		}
	
		return *this;
	
	}
	
	
	Grapheme & Grapheme::operator = (Grapheme && other) noexcept {
	
		if (&other!=this) {
		
			destroy();
			
			move(std::move(other));
		
		}
		
		return *this;
	
	}
	
	
	Grapheme::Grapheme (char c) noexcept : mode(Mode::Single), cp(static_cast<CodePoint::Type>(c)), count(1) {	}
	
	
	const CodePoint * Grapheme::begin () const noexcept {
	
		return (mode==Mode::Single) ? &cp : ptr;
	
	}
	
	
	const CodePoint * Grapheme::end () const noexcept {
	
		return (mode==Mode::Single) ? (&cp+1) : (ptr+count);
	
	}


}
