#include <unicode/ucd.hpp>
#include <iterator>
#include <stdexcept>
#include <utility>


namespace Unicode {


	void File::read () {
	
		b_iter=buffer;
		
		file.read(buffer,size);
		
		if (
			!file.eof() &&
			(
				file.fail() ||
				file.bad()
			)
		) throw std::runtime_error(
			"Error while reading file"
		);
		
		e_iter=b_iter+file.gcount();
	
	}


	File::File (const char * filename)
		:	file(
				filename,
				std::ios::in
			),
			b_iter(nullptr),
			e_iter(nullptr)
	{	}
	
	
	std::optional<Line> File::Get () {
	
		std::string line;
		bool cr=false;
		for (;;) {
		
			for (;b_iter!=e_iter;++b_iter) {
			
				switch (*b_iter) {
				
					case '\r':
						if (cr) line.push_back('\r');
						else cr=true;
						break;
						
					case '\n':{
						auto retr=Line::Get(std::move(line));
						if (retr) return retr;
						line=std::string{};
					}break;
						
					default:
						if (cr) {
							cr=false;
							line.push_back('\r');
						}
						line.push_back(*b_iter);
						break;
				
				}
			
			}
			
			if (file.eof()) {
			
				if (line.size()==0) return std::optional<Line>{};
				
				return Line::Get(std::move(line));
			
			}
			
			read();
			
		}
	
	}
	
	
	FileIterator File::begin () {
	
		return FileIterator(*this,false);
	
	}
	
	
	FileIterator File::end () {
	
		return FileIterator(*this,true);
	
	}


}
