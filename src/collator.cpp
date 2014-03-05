#include <unicode/collator.hpp>
#include <unicode/makereverseiterator.hpp>
#include <unicode/normalizer.hpp>
#include <unicode/vector.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>


namespace Unicode {


	namespace {
	
	
		enum class CollationTableSearchResult {
		
			//	There are possible matches,
			//	but no match at this point
			Proceed,
			//	There's a match at this point,
			//	but other, longer possible matches
			//	as well
			Match,
			//	There's a match at this point, and
			//	that's the longest match possible
			Done,
			//	There's no match at this point, and
			//	longer matches are not possible
			Fail
		
		};
	
	
		class CollationTableSearch {
		
		
			private:
			
			
				const CollationTableEntry * begin;
				const CollationTableEntry * end;
				std::size_t pos;
				
				
			public:
			
			
				CollationTableSearch () = delete;
				CollationTableSearch (const Locale & locale) noexcept
					:	begin(locale.CollationTable.begin()),
						end(locale.CollationTable.end()),
						pos(0)
				{	}
				
				
				//	Gets the match at this point
				const CollationTableEntry * Get () const noexcept {
				
					return begin;
				
				}
				
				
				//	Attempts to add a code point to the match
				CollationTableSearchResult operator () (CodePoint cp) noexcept {
				
					//	Narrow the search range
					begin=std::lower_bound(
						begin,
						end,
						cp,
						[&] (const CollationTableEntry & cte, CodePoint cp) noexcept {
						
							return (cte.CodePoints.Size<=pos) ? true : (cte.CodePoints[pos]<cp);
						
						}
					);
					end=std::upper_bound(
						begin,
						end,
						cp,
						[&] (CodePoint cp, const CollationTableEntry & cte) noexcept {
						
							return (cte.CodePoints.Size<=pos) ? false : (cp<cte.CodePoints[pos]);
						
						}
					);
					
					//	If there are no matches, fail out
					if (begin==end) return CollationTableSearchResult::Fail;
					
					//	Is the match at this point exact?
					if (begin->CodePoints.Size==(pos+1)) return (begin==(end-1)) ? CollationTableSearchResult::Done : CollationTableSearchResult::Match;
					
					//	Otherwise there are possible matches, but
					//	no exact one at this point
					return CollationTableSearchResult::Proceed;
				
				}
				
				
				//	Proceeds to the next code point
				void Next () noexcept {
				
					++pos;
				
				}
		
		
		};
		
		
	}
	
	
	class CollationElementContainer {
	
	
		private:
			
			
			const CollationElement * ptr;
			//	For generated collation elements
			std::uint16_t primary;
			unsigned char lower [2];
			
			
		public:
		
		
			CollationElementContainer () = delete;
			CollationElementContainer (std::uint16_t primary, unsigned char secondary, unsigned char tertiary) noexcept : ptr(nullptr), primary(primary) {
			
				lower[0]=secondary;
				lower[1]=tertiary;
			
			}
			CollationElementContainer (const CollationElement & e) noexcept : ptr(&e) {	}
			
			
			std::size_t Size () const noexcept {
			
				return (ptr==nullptr) ? 3 : ptr->Weights.Size;
			
			}
			
			
			bool Variable () const noexcept {
			
				return (ptr==nullptr) ? false : ptr->Variable;
			
			}
			
			
			std::optional<std::uint16_t> operator [] (std::size_t i) const noexcept {
			
				if (ptr==nullptr) {
				
					switch (i) {
					
						case 0:
							return primary;
						case 1:
							return lower[0];
						case 2:
							return lower[1];
						default:
							break;
					
					}
				
				} else if (i<ptr->Weights.Size) {
				
					return ptr->Weights[i];
				
				}
				
				return std::nullopt;
			
			}
	
	
	};
	
	
	namespace {
	
	
		class CollationElements {
		
		
			public:
			
			
				//	Maximum level
				std::size_t Maximum;
				//	Collation elements
				std::vector<CollationElementContainer> CollationElements;
		
		
		};
	
	
		class CollationElementRetriever {
		
		
			private:
			
			
				const Locale & locale;
				//	Combining marks that have been removed from
				//	the stream
				std::vector<const CodePoint *> removed;
				//	Current location
				const CodePoint * begin;
				//	End of string
				const CodePoint * end;
				
				
				//	Determines if a certain location is the location
				//	of a code point which has been removed as part of
				//	a match formed under S2.1.3
				bool is_removed (bool remove=false) noexcept {
				
					auto end=removed.end();
					auto iter=std::find(removed.begin(),end,begin);
					if (iter==end) return false;
					
					if (remove) removed.erase(iter);
					
					return true;
				
				}
				
				
				const CollationTableEntry * longest_initial_substring (CollationTableSearch & matcher) {
				
					const CodePoint * start=begin;
					const CollationTableEntry * match=nullptr;
					for (;begin!=end;++begin,matcher.Next()) {
					
						switch (matcher(*begin)) {
						
							case CollationTableSearchResult::Done:
								return matcher.Get();
							case CollationTableSearchResult::Fail:
								begin=start;
								return match;
							case CollationTableSearchResult::Match:
								start=begin;
								match=matcher.Get();
							case CollationTableSearchResult::Proceed:
							default:
								break;
						
						}
					
					}
					
					begin=start;
					return match;
				
				}
				
				
				const CollationTableEntry * s_2_1_3 (CollationTableSearch & matcher) {
				
					matcher.Next();
					removed.push_back(begin);
					
					return matcher.Get();
				
				}
				
				
				const CollationTableEntry * get_match () {
				
					CollationTableSearch matcher(locale);
					
					//	S2.1
					//
					//	Find the longest initial substring S at each point that
					//	has a match in the table.
					auto match=longest_initial_substring(matcher);
					if (match==nullptr) return match;
					
					//	S2.1.1
					//
					//	If there are any non-starters following S, process
					//	each non-starter C.
					auto start=begin;
					std::size_t ccc=0;
					for (++begin;begin!=end;++begin) {
					
						if (is_removed()) continue;
					
						auto cpi=begin->GetInfo(locale);
						if ((cpi==nullptr) || (cpi->CanonicalCombiningClass==0)) break;
						
						//	S2.1.2
						//
						//	If C is not blocked from S, find if S+C has a
						//	match in the table.
						if (ccc<cpi->CanonicalCombiningClass) {
						
							ccc=cpi->CanonicalCombiningClass;
							
							auto result=matcher(*begin);
							switch (result) {
							
								case CollationTableSearchResult::Proceed:
								case CollationTableSearchResult::Fail:
								default:
									break;
								//	S2.1.3
								//
								//	If there is a match, replace S by S+C, and remove
								//	C.
								case CollationTableSearchResult::Match:
									match=s_2_1_3(matcher);
									break;
								case CollationTableSearchResult::Done:
									match=s_2_1_3(matcher);
									goto exit_loop;
							
							}
							
						}
					
					}
					
					exit_loop:
					
					//	Since we process "at each point", we return to where
					//	we started.  Next time this function is called the
					//	begin iterator will have been incremented, and we will
					//	process the next point
					begin=start;
					
					return match;
				
				}
				
				
				bool get_match (CollationElements & ces) {
				
					auto entry=get_match();
					if (entry==nullptr) return false;
					
					for (auto & ce : entry->CollationElements) {
					
						ces.CollationElements.emplace_back(ce);
						
						if (ces.Maximum<ce.Weights.Size) ces.Maximum=ce.Weights.Size;
						
					}
					
					return true;
				
				}
				
				
				std::uint16_t get_base () {
				
					//	Get information on the current code point
					auto cpi=begin->GetInfo(locale);
					if (cpi!=nullptr) {
					
						//	TODO: Implement
						//
						//	Need the Unified_Ideograph property
					
					}
					
					return 0xFBC0;
				
				}
				
				
				void derive (CollationElements & ces) {
				
					CodePoint::Type cp=*begin;
					
					ces.CollationElements.emplace_back(
						static_cast<std::uint16_t>(
							(cp>>15)+get_base()
						),
						0x20,
						2
					);
					ces.CollationElements.emplace_back(
						static_cast<std::uint16_t>(
							(cp&0x7FFF)|0x8000
						),
						0,
						0
					);
					
					if (ces.Maximum<3) ces.Maximum=3;
				
				}
				
				
				void get (CollationElements & ces) {
				
					//	Try and get a match, if that fails
					//	we must derive
					if (!get_match(ces)) derive(ces);
				
				}
				
				
			public:
			
			
				CollationElementRetriever (const Locale & locale, const CodePoint * begin, const CodePoint * end) noexcept
					:	locale(locale),
						begin(begin),
						end(end)
				{	}
			
			
				//	Retrieves the next series of collation elements
				//
				//	If the end of the string is reached, false is
				//	returned, otherwise true is returned
				CollationElements Get () {
				
					CollationElements ces;
					//	Start with a maximum level of zero
					ces.Maximum=0;
				
					for (;begin!=end;++begin) {
					
						//	Skip this if it's been removed
						if (is_removed(begin)) continue;
						
						//	Get the next match
						get(ces);
					
					}
					
					return ces;
				
				}
		
		
		};
	
	
	}
	
	
	std::optional<std::vector<CodePoint>> Collator::prepare (const CodePoint * begin, const CodePoint * end) const {
	
		Normalizer n(locale);
		
		if (n.IsNFD(begin,end)) return std::nullopt;
		
		return n.ToNFD(begin,end);
	
	}
	
	
	bool Collator::is_backwards () const noexcept {
	
		return locale.Backwards;
	
	}
	
	
	static void append (std::vector<CodePoint::Type> & key, std::optional<std::uint16_t> weight) {
	
		if (weight && (*weight!=0)) key.push_back(*weight);
	
	}
	
	
	static bool is_zero (std::optional<std::uint16_t> weight) noexcept {
	
		return !weight || (*weight==0);
	
	}
	
	
	static void blanked (std::vector<CodePoint::Type> & key, std::size_t level, const CollationElementContainer & ce, bool variable) {
	
		if (
			ce.Variable() ||
			(
				is_zero(ce[0]) &&
				variable
			)
		) return;
		
		append(key,ce[level]);
	
	}
	
	
	static void shifted (std::vector<CodePoint::Type> & key, std::size_t level, const CollationElementContainer & ce, bool variable) {
	
		//	At levels above 4, the processing is identical
		//	to blanked
		if (level!=3) {
		
			blanked(key,level,ce,variable);
			
			return;
		
		}
		
		//	Handle level 4
		std::optional<std::uint16_t> weight;
		if (is_zero(ce[0])) {
		
			if (ce.Variable() || is_zero(ce[2]) || variable) return;
			
			weight=0xFFFF;
		
		} else if (ce.Variable()) {
		
			weight=ce[0];
		
		} else {
		
			weight=0xFFFF;
		
		}
		
		append(key,weight);
	
	}
	
	
	void Collator::add_weight (std::vector<CodePoint::Type> & key, std::size_t level, const CollationElementContainer & ce, bool & variable) const {
	
		switch (vo) {
		
			//	Identical to table entries
			case VariableOrdering::NonIgnorable:
				append(key,ce[level]);
				return;
			//	All weights for variable collation elements,
			//	and collation elements following variable
			//	collation elements, are zero
			case VariableOrdering::Blanked:
				blanked(key,level,ce,variable);
				break;
			//	Special rules are applied
			default:
				shifted(key,level,ce,variable);
				break;
		
		}
		
		if (ce.Variable()) {
		
			variable=true;
			
			return;
			
		}
		
		if (!variable) return;
		
		auto weight=ce[0];
		if (weight && (*weight!=0)) {
		
			variable=false;
			
			return;
		
		}
		
		weight=ce[2];
		variable=weight && (*weight!=0);
	
	}
	
	
	static void trim (std::vector<CodePoint::Type> & key) noexcept {
	
		auto e=key.end();
		auto begin=MakeReverseIterator(e);
		auto end=MakeReverseIterator(key.begin());
		for (;(begin!=end) && (*begin==0xFFFF);++begin);
		
		key.erase(begin.base(),e);
	
	}
	
	
	template <typename Iter>
	void Collator::add_level (std::vector<CodePoint::Type> & key, std::size_t level, Iter begin, Iter end) const {
	
		bool variable=false;
		for (;begin!=end;++begin) add_weight(key,level,*begin,variable);
		
		//	If this is the quaternary level, AND the
		//	variable ordering is ShiftTrimmed, we trim
		//	all trailing 0xFFFF weights
		if (
			(level==3) &&
			(vo==VariableOrdering::ShiftTrimmed)
		) trim(key);
	
	}
	
	
	void Collator::get_identical (std::vector<CodePoint::Type> & key, const CodePoint * begin, const CodePoint * end) const {
	
		std::copy(begin,end,std::back_inserter(key));
	
	}
	
	
	std::size_t Collator::get_max (std::size_t max) const noexcept {
	
		if (max<4) switch (vo) {
		
			case VariableOrdering::Shifted:
			case VariableOrdering::ShiftTrimmed:
				max=4;
			default:
				break;
		
		}
		
		if (!strength || (*strength==0)) return max;
		
		return *strength;
	
	}
	
	
	std::vector<CodePoint::Type> Collator::get_sort_key (const CodePoint * begin, const CodePoint * end) const {
	
		//	Get the collation elements
		auto ces=CollationElementRetriever(locale,begin,end).Get();
		
		//	Form the sort key
		
		std::vector<CodePoint::Type> key;
		
		//	Loop for each level
		auto max=get_max(ces.Maximum);
		for (std::size_t level=0;level<max;++level) {
		
			//	Add level separator
			if (level!=0) key.push_back(0);
		
			auto begin=ces.CollationElements.begin();
			auto end=ces.CollationElements.end();
		
			if ((level==1) && is_backwards()) add_level(
				key,
				level,
				MakeReverseIterator(end),
				MakeReverseIterator(begin)
			);
			else add_level(
				key,
				level,
				begin,
				end
			);
		
		}
		
		//	Add identical level if necessary
		if (strength && (*strength==0)) get_identical(key,begin,end);
		
		return key;
	
	}
	
	
	std::vector<CodePoint::Type> Collator::GetSortKey (const CodePoint * begin, const CodePoint * end) const {
	
		//	4.1 Normalize of UTS#10 requires that
		//	the string be placed in Normal Form
		//	Canonical Decomposition
		auto prepared=prepare(begin,end);
		
		return get_sort_key(
			Begin(begin,prepared),
			End(end,prepared)
		);
	
	}
	
	
	int Collator::Compare (const CodePoint * begin_a, const CodePoint * end_a, const CodePoint * begin_b, const CodePoint * end_b) const {
	
		auto a=GetSortKey(begin_a,end_a);
		auto b=GetSortKey(begin_b,end_b);
		
		auto a_b=a.begin();
		auto a_e=a.end();
		auto b_b=b.begin();
		auto b_e=b.end();
		for (;!((a_b==a_e) || (b_b==b_e));++a_b,++b_b) {
		
			if (*a_b<*b_b) return -1;
			if (*a_b>*b_b) return 1;
		
		}
		
		if (a_b==a_e) {
		
			if (b_b==b_e) return 0;
			
			return -1;
			
		}
		
		return 1;
	
	}


}
