#include <unicode/regex.hpp>
#include <new>


namespace Unicode {


	//
	//	RegexState::Iterator
	//


	void RegexState::Iterator::destroy () noexcept {
	
		if (!reversed) return;
		
		reversed=false;
		r.~r_type();
	
	}
	
	
	void RegexState::Iterator::copy (const Iterator & other) noexcept {
	
		if (reversed) new (&r) r_type (other.r);
		else i=other.i;
	
	}
	
	
	void RegexState::Iterator::advance (std::ptrdiff_t dist) noexcept {
	
		if (reversed) r+=dist;
		else i+=dist;
	
	}
	
	
	void RegexState::Iterator::retreat (std::ptrdiff_t dist) noexcept {
	
		if (reversed) r-=dist;
		else i-=dist;
	
	}
	
	
	RegexState::Iterator::Iterator () noexcept : reversed(false), i(nullptr) {	}
	
	
	RegexState::Iterator::Iterator (const Iterator & other) noexcept : reversed(other.reversed) {
	
		copy(other);
	
	}
	
	
	RegexState::Iterator & RegexState::Iterator::operator = (const Iterator & other) noexcept {
	
		if (&other!=this) {
		
			destroy();
		
			reversed=other.reversed;
			
			copy(other);
		
		}
		
		return *this;
	
	}
	
	
	RegexState::Iterator::~Iterator () noexcept {
	
		destroy();
	
	}
	
	
	RegexState::Iterator::Iterator (const CodePoint * i, bool reversed) noexcept : reversed(reversed) {
	
		if (reversed) new (&r) r_type (i);
		else this->i=i;
	
	}
	
	
	bool RegexState::Iterator::Reversed () const noexcept {
	
		return reversed;
	
	}
	
	
	void RegexState::Iterator::Reverse () & noexcept {
	
		if (reversed) {
		
			reversed=false;
			
			auto temp=r;
			r.~r_type();
			i=temp.base();
		
		} else {
		
			reversed=true;
			
			auto temp=i;
			new (&r) r_type (temp);
		
		}
	
	}
	
	
	RegexState::Iterator RegexState::Iterator::Reverse () const & noexcept {
	
		auto retr=*this;
		retr.Reverse();
		
		return retr;
	
	}
	
	
	const CodePoint * RegexState::Iterator::Base () const noexcept {
	
		return reversed ? r.base() : i;
	
	}
	
	
	const CodePoint & RegexState::Iterator::operator * () const noexcept {
	
		return reversed ? *r : *i;
	
	}
	
	
	const CodePoint * RegexState::Iterator::operator -> () const noexcept {
	
		return reversed ? &(*r) : i;
	
	}
	
	
	RegexState::Iterator & RegexState::Iterator::operator ++ () noexcept {
	
		advance(1);
	
		return *this;
	
	}
	
	
	RegexState::Iterator RegexState::Iterator::operator ++ (int) noexcept {
	
		auto retr=*this;
		
		advance(1);
		
		return retr;
	
	}
	
	
	RegexState::Iterator & RegexState::Iterator::operator -- () noexcept {
	
		retreat(1);
	
		return *this;
	
	}
	
	
	RegexState::Iterator RegexState::Iterator::operator -- (int) noexcept {
	
		auto retr=*this;
		
		retreat(1);
		
		return retr;
	
	}
	
	
	RegexState::Iterator & RegexState::Iterator::operator += (std::ptrdiff_t dist) noexcept {
	
		advance(dist);
		
		return *this;
	
	}
	
	
	RegexState::Iterator & RegexState::Iterator::operator -= (std::ptrdiff_t dist) noexcept {
	
		retreat(dist);
		
		return *this;
	
	}
	
	
	RegexState::Iterator RegexState::Iterator::operator + (std::ptrdiff_t dist) const noexcept {
	
		auto retr=*this;
		
		retr+=dist;
		
		return retr;
	
	}
	
	
	std::ptrdiff_t RegexState::Iterator::operator - (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return 0;
			
			return r-other.r;
		
		}
		
		if (other.reversed) return 0;
		
		return i-other.i;
	
	}
	
	
	RegexState::Iterator RegexState::Iterator::operator - (std::ptrdiff_t dist) const noexcept {
	
		auto retr=*this;
		
		retr-=dist;
		
		return retr;
	
	}
	
	
	const CodePoint & RegexState::Iterator::operator [] (std::size_t n) const noexcept {
	
		return reversed ? r[n] : i[n];
	
	}
	
	
	bool RegexState::Iterator::operator < (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r<other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i<other.i;
	
	}
	
	
	bool RegexState::Iterator::operator > (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r>other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i>other.i;
	
	}
	
	
	bool RegexState::Iterator::operator <= (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r<=other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i<=other.i;
	
	}
	
	
	bool RegexState::Iterator::operator >= (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r>=other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i>=other.i;
	
	}
	
	
	bool RegexState::Iterator::operator == (const Iterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r==other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i==other.i;
	
	}
	
	
	bool RegexState::Iterator::operator != (const Iterator & other) const noexcept {
	
		return !(*this==other);
	
	}
	
	
	//
	//	RegexState
	//
	
	
	RegexState::RegexState (const CodePoint * begin, const CodePoint * end, bool reversed) noexcept
		:	b(reversed ? end : begin,reversed),
			l(b),
			e(reversed ? begin : end,reversed)
	{	}
	
	
	RegexState::operator bool () const noexcept {
	
		return b!=e;
	
	}
	
	
	RegexState::Iterator RegexState::begin () const noexcept {
	
		return l;
	
	}
	
	
	void RegexState::begin (Iterator i) noexcept {
	
		l=i;
	
	}
	
	
	const CodePoint * RegexState::Begin () const noexcept {
	
		return Reversed() ? e.Base() : b.Base();
	
	}
	
	
	RegexState::Iterator RegexState::end () const noexcept {
	
		return e;
	
	}
	
	
	const CodePoint * RegexState::End () const noexcept {
	
		return Reversed() ? b.Base() : e.Base();
	
	}
	
	
	const CodePoint & RegexState::operator * () const noexcept {
	
		return *l;
	
	}
	
	
	const CodePoint * RegexState::operator -> () const noexcept {
	
		return &(*l);
	
	}
	
	
	RegexState & RegexState::operator ++ () noexcept {
	
		return *this+=1;
	
	}
	
	
	RegexState & RegexState::operator -- () noexcept {
	
		return *this-=1;
	
	}
	
	
	RegexState & RegexState::operator += (std::ptrdiff_t dist) noexcept {
	
		l+=dist;
		
		return *this;
	
	}
	
	
	RegexState & RegexState::operator -= (std::ptrdiff_t dist) noexcept {
	
		l-=dist;
		
		return *this;
	
	}
	
	
	std::size_t RegexState::Remaining () const noexcept {
	
		return static_cast<std::size_t>(e-l);
	
	}
	
	
	bool RegexState::Reversed () const noexcept {
	
		return b.Reversed();
	
	}
	

}
