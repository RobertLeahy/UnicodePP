#pragma once


#include "arrayaggregator.hpp"
#include "codeoutput.hpp"
#include <unicode/codepoint.hpp>
#include <unicode/ucd.hpp>
#include <cstdint>
#include <string>
#include <vector>


class DUCETParser {


	private:
	
	
		typedef ArrayAggregator<Unicode::CodePoint::Type> cps_type;
		typedef cps_type::Key cps_key;
		
		
		typedef ArrayAggregator<std::uint16_t> weights_type;
		typedef weights_type::Key weights_key;
		
		
		class CollationElement {
		
		
			public:
			
			
				bool Variable;
				weights_key Weights;
		
		
		};
		
		
		typedef ArrayAggregator<CollationElement> elements_type;
		typedef elements_type::Key elements_key;
		
		
		class Entry {
		
		
			public:
			
			
				cps_key CodePoints;
				elements_key CollationElements;
		
		
		};
		
		
		std::vector<Entry> entries;
		
		
		cps_type cps;
		weights_type weights;
		elements_type elements;
		
		
		Unicode::File keys;
		
		
		CodeOutput out;
		
		
		//	Processes a line from the file
		void process (const Unicode::Line &);
		//	Sorts the table
		void sort ();
		
		
		void output (const Array &, const std::string &);
		
		
		void output_cps ();
		void output_weights ();
		void output_elements ();
		void output_table ();
		
		
	public:
	
	
		DUCETParser (const std::string &, const std::string &);
		
		
		void Get ();
		
		
		void Output ();


};
