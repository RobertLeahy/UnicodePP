#pragma once


#include "arrayaggregator.hpp"
#include "casing.hpp"
#include "codeoutput.hpp"
#include <unicode/codepoint.hpp>
#include <unicode/data.hpp>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>


class Parser {


	private:
		
		
		//	Similar to the Unicode::CodePointInfo structure, but
		//	contains more convenient types for our purposes
		class Info {
		
		
			private:
			
			
				bool in_range (Unicode::CodePoint::Type, Unicode::CodePoint::Type) const noexcept;
		
		
			public:
			
			
				//	So we can find/identify this structure
				Unicode::CodePoint::Type CodePoint;
			
			
				//	General
				std::string Name;
				std::string Alias;
				std::string Abbreviation;
				std::optional<std::size_t> Block;
				Unicode::GeneralCategory GeneralCategory;
				std::optional<std::size_t> Script;
				bool WhiteSpace;
				bool Alphabetic;
				bool NoncharacterCodePoint;
				bool DefaultIgnorableCodePoint;
				bool Deprecated;
				bool LogicalOrderException;
				bool VariationSelector;
				
				//	Case
				bool Uppercase;
				bool Lowercase;
				//	The full case mapping fields, and
				//	case folding fields aren't needed
				//	as they're handled by the CasingParser
				std::optional<Unicode::CodePoint::Type> SimpleLowercaseMapping;
				std::optional<Unicode::CodePoint::Type> SimpleTitlecaseMapping;
				std::optional<Unicode::CodePoint::Type> SimpleUppercaseMapping;
				bool SoftDotted;
				bool Cased;
				bool CaseIgnorable;
				
				//	Numeric
				std::optional<Unicode::Numeric> Numeric;
				bool HexDigit;
				bool ASCIIHexDigit;
				
				//	Normalization
				std::size_t CanonicalCombiningClass;
				ArrayAggregator<Unicode::CodePoint::Type>::Key DecompositionMapping;
				bool CompositionExclusion;
				bool FullCompositionExclusion;
				Unicode::QuickCheck NFCQuickCheck;
				Unicode::QuickCheck NFDQuickCheck;
				
				//	Shaping and Rendering
				Unicode::LineBreak LineBreak;
				Unicode::GraphemeClusterBreak GraphemeClusterBreak;
				Unicode::SentenceBreak SentenceBreak;
				Unicode::WordBreak WordBreak;
				
				//	Bidirectional
				Unicode::BidirectionalClass BidirectionalClass;
				bool BidirectionalControl;
				bool Mirrored;
				
				//	Miscellaneous
				bool Math;
				bool QuotationMark;
				bool Dash;
				bool Hyphen;
				bool STerm;
				bool TerminalPunctuation;
				bool Diacritic;
				bool Extender;
				bool GraphemeBase;
				bool GraphemeLink;
				
				
				Info (Unicode::CodePoint::Type) noexcept;
				
				
				void Derive () noexcept;
		
		
		};
		
		
		static const std::pair<bool (Parser::Info::*),const char *> prop_map [];
	
	
		std::vector<Info> info;
		
		
		ArrayAggregator<Unicode::CodePoint::Type> cps;
		ArrayAggregator<Unicode::ConditionInfo> conds;
		
		
		std::vector<std::string> blocks;
		std::unordered_map<std::string,std::size_t> blocks_map;
		std::vector<std::string> scripts;
		std::unordered_map<std::string,std::size_t> scripts_map;
	
	
		//	Input files
		Unicode::File data;
		Unicode::File prop_list;
		Unicode::File b;
		Unicode::File s;
		Unicode::File name_aliases;
		Unicode::File composition_exclusions;
		Unicode::File line_break;
		Unicode::File grapheme_break;
		Unicode::File sentence_break;
		Unicode::File word_break;
		Unicode::File derived_normalization_props;
		
		
		//	Output file
		CodeOutput out;
		
		
		CasingParser casing;
		
		
		//	Gets a lower bound
		std::vector<Info>::iterator lower_bound (Unicode::CodePoint::Type) noexcept;
		//	Attempts to find an entry
		std::vector<Info>::iterator find (Unicode::CodePoint::Type) noexcept;
		//	Gets an entry
		Info & get (Unicode::CodePoint::Type);
		//	Gets a code point from an item, throwing if it
		//	can't
		static Unicode::CodePoint::Type get_cp (const Unicode::Item &);
		//	Gets an entry based on a particular item in a line
		Info & get (const Unicode::Item &);
		//	Performs an action once for each element in
		//	a range that exists
		template <typename T, typename... Args>
		void foreach (
			const Unicode::Range & range,
			T && callback,
			Args &&... args
		) noexcept(noexcept(callback(std::declval<Info &>(),std::forward<Args>(args)...))) {
		
			auto iter=lower_bound(range.Low);
			
			for (
				auto end=info.end();
				(iter!=end) &&
				(iter->CodePoint<=range.High);
				++iter
			) callback(*iter,std::forward<Args>(args)...);
		
		}
		//	Performs an action once for each extant code point
		//	which exists in a file
		template <typename T>
		void foreach (Unicode::File & file, std::size_t per_line, T && callback) {
		
			for (auto & line : file) {
			
				if (line.size()<per_line) throw std::runtime_error("Bad line");
				
				auto range=line[0].Range();
				if (range) {
				
					foreach(*range,callback,line);
					
					continue;
				
				}
				
				auto iter=find(get_cp(line[0]));
				if (iter==info.end()) continue;
				
				callback(*iter,line);
			
			}
		
		}
		
		
		//	Gets a decomposition mapping
		ArrayAggregator<Unicode::CodePoint::Type>::Key get_decomposition_mapping (const Unicode::Item &);
		//	Gets an individual line from UnicodeData.txt
		void get_data (const Unicode::Line &);
		//	Gets data from UnicodeData.txt
		void get_data ();
		

		//	Gets data from PropList.txt
		void get_prop_list ();
		
		
		//	Applies a block to a certain range of
		//	code points
		void get_blocks (const Unicode::Range &, std::size_t) noexcept;
		//	Gets a single line from Blocks.txt
		void get_blocks (const Unicode::Line &);
		//	Gets data from Blocks.txt
		void get_blocks ();
		
		
		//	Applies a script to a certain range of
		//	code points
		void get_scripts (const Unicode::Range &, std::size_t) noexcept;
		//	Gets a single line from Scripts.txt
		void get_scripts (const Unicode::Line &);
		//	Gets data from Scripts.txt
		void get_scripts ();
		
		
		//	Gets a single line from NameAliases.txt
		void get_name_aliases (const Unicode::Line &);
		//	Gets data from NameAliases.txt
		void get_name_aliases ();
		
		
		//	Gets data from CompositionExclusions.txt
		void get_composition_exclusions ();
		
		
		//	Gets data from LineBreak.txt
		void get_line_break ();
		

		//	Gets data from GraphemeBreakProperty.txt
		void get_grapheme_cluster_break ();
		
		
		//	Gets data from SentenceBreakProperty.txt
		void get_sentence_break ();
		
		
		//	Gets data from WordBreakProperty.txt
		void get_word_break ();
		
		
		//	Helper for getting DerivedNormalizationProps.txt
		static void get_derived_normalization_props (Info &, bool, const std::string &);
		//	Gets data from DerivedNormalizationProps.txt
		void get_derived_normalization_props ();
		
		
		//	Outputs integers as hex
		template <typename T>
		typename std::enable_if<
			std::is_integral<typename std::decay<T>::type>::value
		>::type output (T num) {
		
			out << std::hex << "0x" << num << std::dec;
		
		}
		//	Outputs strings as C strings
		void output (const std::string &);
		//	Outputs optionals
		template <typename T>
		void output (const std::optional<T> & opt) {
		
			if (opt) output(*opt);
			else out << "std::nullopt";
		
		}
		//	Outputs booleans
		void output (bool);
		//	Outputs a condition
		void output (const Unicode::ConditionInfo &);
		//	Outputs an array
		void output (const Array &, const std::string &);
		//	Outputs a numeric
		void output (const Unicode::Numeric &);
		//	Outputs appropriate code to begin the next item
		//	in an array or object
		void next (bool newline=true);
		
		
		//	Outputs code points
		void output_cps ();
		//	Outputs blocks
		void output_blocks ();
		//	Outputs scripts
		void output_scripts ();
		//	Outputs conditions
		void output_conds ();
		//	Outputs arrays
		void output_arrays();
		
		
		//	Outputs a CodePointInfo object
		void output_code_point_info_inner (const Info &);
		//	Outputs an entry
		void output_code_point_info (const Info &);
		//	Outputs CodePointInfo structures
		void output_code_point_info ();
		
		
	public:
	
	
		Parser (const std::string &, const std::string &);
		
		
		void Get ();
		
		
		void Output ();


};
