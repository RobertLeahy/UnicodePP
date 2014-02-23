#include <unicode/data.hpp>
#include <iterator>
#include <stdexcept>
#include <utility>


namespace Unicode {


	void File::read () {
	
		begin=buffer;
		
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
		
		end=begin+file.gcount();
	
	}


	File::File (const char * filename)
		:	file(
				filename,
				std::ios::in
			),
			begin(nullptr),
			end(nullptr)
	{	}
	
	
	std::optional<Line> File::Get () {
	
		std::vector<char> line;
		bool cr=false;
		for (;;) {
		
			for (;begin!=end;++begin) {
			
				switch (*begin) {
				
					case '\r':
						if (cr) line.push_back('\r');
						else cr=true;
						break;
						
					case '\n':{
						auto retr=Line::Get(std::move(line));
						if (retr) return retr;
						line=std::vector<char>{};
					}break;
						
					default:
						if (cr) {
							cr=false;
							line.push_back('\r');
						}
						line.push_back(*begin);
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


}
