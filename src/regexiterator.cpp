#include <unicode/regex.hpp>
#include <new>


namespace Unicode {


	void RegexIterator::destroy () noexcept {
	
		if (!reversed) return;
		
		reversed=false;
		r.~r_type();
	
	}
	
	
	void RegexIterator::copy (const RegexIterator & other) noexcept {
	
		if (reversed) new (&r) r_type (other.r);
		else i=other.i;
	
	}
	
	
	void RegexIterator::advance (std::ptrdiff_t dist) noexcept {
	
		if (reversed) r+=dist;
		else i+=dist;
	
	}
	
	
	void RegexIterator::retreat (std::ptrdiff_t dist) noexcept {
	
		if (reversed) r-=dist;
		else i-=dist;
	
	}
	
	
	RegexIterator::RegexIterator () noexcept : reversed(false), i(nullptr) {	}
	
	
	RegexIterator::RegexIterator (const RegexIterator & other) noexcept : reversed(other.reversed) {
	
		copy(other);
	
	}
	
	
	RegexIterator & RegexIterator::operator = (const RegexIterator & other) noexcept {
	
		if (&other!=this) {
		
			destroy();
		
			reversed=other.reversed;
			
			copy(other);
		
		}
		
		return *this;
	
	}
	
	
	RegexIterator::~RegexIterator () noexcept {
	
		destroy();
	
	}
	
	
	RegexIterator::RegexIterator (const CodePoint * i, bool reversed) noexcept : reversed(reversed) {
	
		if (reversed) new (&r) r_type (i);
		else this->i=i;
	
	}
	
	
	bool RegexIterator::Reversed () const noexcept {
	
		return reversed;
	
	}
	
	
	void RegexIterator::Reverse () & noexcept {
	
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
	
	
	RegexIterator RegexIterator::Reverse () const & noexcept {
	
		auto retr=*this;
		retr.Reverse();
		
		return retr;
	
	}
	
	
	const CodePoint * RegexIterator::Base () const noexcept {
	
		return reversed ? r.base() : i;
	
	}
	
	
	const CodePoint & RegexIterator::operator * () const noexcept {
	
		return reversed ? *r : *i;
	
	}
	
	
	const CodePoint * RegexIterator::operator -> () const noexcept {
	
		return reversed ? &(*r) : i;
	
	}
	
	
	RegexIterator & RegexIterator::operator ++ () noexcept {
	
		advance(1);
	
		return *this;
	
	}
	
	
	RegexIterator RegexIterator::operator ++ (int) noexcept {
	
		auto retr=*this;
		
		advance(1);
		
		return retr;
	
	}
	
	
	RegexIterator & RegexIterator::operator -- () noexcept {
	
		retreat(1);
	
		return *this;
	
	}
	
	
	RegexIterator RegexIterator::operator -- (int) noexcept {
	
		auto retr=*this;
		
		retreat(1);
		
		return retr;
	
	}
	
	
	RegexIterator & RegexIterator::operator += (std::ptrdiff_t dist) noexcept {
	
		advance(dist);
		
		return *this;
	
	}
	
	
	RegexIterator & RegexIterator::operator -= (std::ptrdiff_t dist) noexcept {
	
		retreat(dist);
		
		return *this;
	
	}
	
	
	RegexIterator RegexIterator::operator + (std::ptrdiff_t dist) const noexcept {
	
		auto retr=*this;
		
		retr+=dist;
		
		return retr;
	
	}
	
	
	std::ptrdiff_t RegexIterator::operator - (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return 0;
			
			return r-other.r;
		
		}
		
		if (other.reversed) return 0;
		
		return i-other.i;
	
	}
	
	
	RegexIterator RegexIterator::operator - (std::ptrdiff_t dist) const noexcept {
	
		auto retr=*this;
		
		retr-=dist;
		
		return retr;
	
	}
	
	
	const CodePoint & RegexIterator::operator [] (std::size_t n) const noexcept {
	
		return reversed ? r[n] : i[n];
	
	}
	
	
	bool RegexIterator::operator < (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r<other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i<other.i;
	
	}
	
	
	bool RegexIterator::operator > (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r>other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i>other.i;
	
	}
	
	
	bool RegexIterator::operator <= (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r<=other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i<=other.i;
	
	}
	
	
	bool RegexIterator::operator >= (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r>=other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i>=other.i;
	
	}
	
	
	bool RegexIterator::operator == (const RegexIterator & other) const noexcept {
	
		if (reversed) {
		
			if (!other.reversed) return false;
			
			return r==other.r;
		
		}
		
		if (other.reversed) return false;
		
		return i==other.i;
	
	}
	
	
	bool RegexIterator::operator != (const RegexIterator & other) const noexcept {
	
		return !(*this==other);
	
	}


}
