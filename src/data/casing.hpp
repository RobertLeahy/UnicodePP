#pragma once


#include "arrayaggregator.hpp"
#include "codeoutput.hpp"
#include <unicode/codepoint.hpp>
#include <unicode/data.hpp>
#include <cstddef>
#include <string>
#include <optional>
#include <vector>


class CasingParser {


	public:
	
	
		class Result {
		
		
			public:
		
			
				Array LowercaseMappings;
				Array TitlecaseMappings;
				Array UppercaseMappings;
				Array CaseFoldings;
				std::optional<Unicode::CodePoint::Type> SimpleCaseFolding;
		
		
		};


	private:
	
	
		typedef ArrayAggregator<Unicode::CodePoint::Type> cps_type;
		typedef cps_type::Key cps_key;
		typedef ArrayAggregator<Unicode::ConditionInfo> conds_type;
		typedef conds_type::Key conds_key;
	
	
		class Casing {
		
		
			public:
			
			
				std::optional<conds_key> Conditions;
				cps_key Mapping;
		
		
		};
		
		
		typedef ArrayAggregator<Casing> casings_type;
		typedef casings_type::Key casings_key;
		
		
		class Entry {
		
		
			public:
			
			
				typedef cps_key Type;
		
		
				Unicode::CodePoint::Type CodePoint;
				
				
				Type Lower;
				Type Title;
				Type Upper;
				Type Folding;
				std::optional<Unicode::CodePoint::Type> SimpleFolding;
				
				
				Entry (Unicode::CodePoint::Type cp) noexcept : CodePoint(cp) {	}
		
		};
		
		
		cps_type & cps;
		casings_type casings;
		conds_type & conds;
		
		
		std::vector<Entry> entries;
		
		
		std::optional<conds_key> tr;
		
		
		Unicode::File casing;
		Unicode::File folding;
		
		
		CodeOutput & out;
		
		
		//	Gets an entry for a specific code
		//	point
		Entry & get (Unicode::CodePoint::Type);
		//	Gets the key for turkic conditions
		conds_key get_tr ();
		//	Finds an entry for a specific code
		//	point
		std::vector<Entry>::iterator find (Unicode::CodePoint::Type) noexcept;
		
		
		//	Adds a casing
		void add_casing (cps_key, std::optional<conds_key>, std::vector<Unicode::CodePoint::Type>);
		//	Retrieves casings from an item
		//	at a specific index
		std::vector<Unicode::CodePoint::Type> get_casing (const Unicode::Line &, std::size_t);
		//	Processes an individual line from
		//	SpecialCasing.txt
		void get_casing (const Unicode::Line &);
		//	Gets information from SpecialCasing.txt
		void get_casings ();
		
		
		//	Process a complex or conditional case
		//	folding
		void get_folding (Entry &, const Unicode::Item &, bool);
		//	Process a simple case folding
		void get_folding (Entry &, const Unicode::Item &);
		//	Processes an individual line from
		//	CaseFolding.txt
		void get_folding (const Unicode::Line &);
		//	Gets information from CaseFolding.txt
		void get_foldings ();
		
		
	public:
	
	
		CasingParser (
			std::string base,
			ArrayAggregator<Unicode::CodePoint::Type> & cps,
			ArrayAggregator<Unicode::ConditionInfo> & conds,
			CodeOutput & out
		);
		
		
		void Get ();
		
		
		void Output ();
		
		
		Result Get (Unicode::CodePoint::Type);


};
