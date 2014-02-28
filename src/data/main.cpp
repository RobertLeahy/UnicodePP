#include "parser.hpp"
#include <cstdlib>
#include <iostream>


int main (int argc, char ** argv) {

	if (argc<3) {
	
		std::cout << "Expected 2 command line arguments, " << (argc-1) << " provided" << std::endl;
		
		return EXIT_FAILURE;
	
	}
	
	Parser parser(
		argv[1],
		argv[2]
	);
	
	std::cout << "Reading Unicode character database..." << std::endl;
	
	parser.Get();
	
	std::cout << "Done!" << std::endl;
	
	std::cout << "Writing C++ source file..." << std::endl;
	
	parser.Output();
	
	std::cout << "Done!" << std::endl;

}
