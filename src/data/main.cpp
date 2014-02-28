#include "parser.hpp"


int main (void) {

	Parser parser(
		"C:\\Users\\rleahy\\Documents\\C++\\Unicode\\UCD\\",
		"C:\\Users\\rleahy\\Documents\\C++\\Unicode\\bin\\test.cpp"
	);
	
	parser.Get();
	
	parser.Output();

}
