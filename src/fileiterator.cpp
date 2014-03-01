#include <unicode/ucd.hpp>


namespace Unicode {


	FileIterator::FileIterator (File & file, bool is_end) : file(file) {
	
		if (!is_end) line=file.Get();
	
	}
	
	
	bool FileIterator::operator == (const FileIterator & other) const noexcept {
	
		return static_cast<bool>(line)==static_cast<bool>(other.line);
	
	}
	
	
	bool FileIterator::operator != (const FileIterator & other) const noexcept {
	
		return !(*this==other);
	
	}
	
	
	const Line * FileIterator::operator -> () const noexcept {
	
		return &(*line);
	
	}
	
	
	const Line & FileIterator::operator * () const noexcept {
	
		return *line;
	
	}
	
	
	FileIterator & FileIterator::operator ++ () {
	
		line=file.Get();
		
		return *this;
	
	}
	
	
	FileIterator FileIterator::operator ++ (int) {
	
		auto retr=*this;
		
		line=file.Get();
		
		return retr;
	
	}


}
