#include <unicode/regex.hpp>
#include <unicode/regexcompiler.hpp>
#include <unicode/regexerror.hpp>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>


namespace Unicode {


	constexpr std::size_t default_priority=std::numeric_limits<std::size_t>::max()/2;
	
	
	typedef RegexCompilerAcquisitionPolicy::Type parser_info;
	
	
	static std::vector<parser_info> & get_impl () {
	
		static std::vector<parser_info> vec;
		
		return vec;
	
	}
	
	
	static void add_impl (
		const RegexParser & parser,
		std::size_t priority=default_priority,
		std::optional<bool> character_class=std::nullopt
	) {
	
		auto & parsers=get_impl();
		parsers.insert(
			std::find_if(
				parsers.begin(),
				parsers.end(),
				[&] (const parser_info & t) noexcept {
				
					return priority<=t.Priority;
				
				}
			),
			{&parser,priority,character_class}
		);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser) {
	
		add_impl(parser);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, std::size_t priority) {
	
		add_impl(parser,priority);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, bool character_class) {
	
		add_impl(parser,default_priority,character_class);
	
	}
	
	
	void RegexCompilerAcquisitionPolicy::Add (const RegexParser & parser, std::size_t priority, bool character_class) {
	
		add_impl(parser,priority,character_class);
	
	}
	
	
	const std::vector<RegexCompilerAcquisitionPolicy::Type> & RegexCompilerAcquisitionPolicy::Get () {
	
		return get_impl();
	
	}
	
	
	bool RegexCompilerInvocationPolicy::Invoke (RegexCompilerBaseType & compiler, const parser_info & info) {
	
		auto & c=reinterpret_cast<RegexCompiler &>(compiler);
	
		if (info.CharacterClass && (*info.CharacterClass!=c.CharacterClass)) return false;
		
		return (*info.Parser)(c);
	
	}
	
	
	RegexCompiler::GroupInfo::GroupInfo () : Element(nullptr) {	}
	
	
	RegexCompiler::GroupInfo::GroupInfo (const RegexPatternElement & element) : Element(&element) {	}
	
	
	void RegexCompiler::get_capturing_group (GroupsInfo & info, const RegexPatternElement * & ptr) {
		
		if (info.Elements.size()==0) info.Pending.push_back(&ptr);
		else ptr=info.Elements[0];
		
	}
	
	
	template <typename T>
	const RegexPatternElement * RegexCompiler::get_capturing_group (const GroupsMapping<T> & map, const T & key) {
	
		auto iter=map.find(key);
		if (iter==map.end()) return nullptr;
		auto & elements=iter->second.Elements;
		return (elements.size()==0) ? nullptr : elements[0];
	
	}
	
	
	std::vector<RegexCompiler::GroupInfo>::iterator RegexCompiler::find_groups_end () noexcept {
	
		return std::find_if(
			groups.begin(),
			groups.end(),
			[] (const GroupInfo & info) noexcept {	return info.Element==nullptr;	}
		);
	
	}
	
	
	void RegexCompiler::add_capturing_group (const RegexPatternElement & element) {
	
		auto iter=find_groups_end();
		if (iter==groups.end()) {
		
			groups.emplace_back(element);
			
			return;
		
		}
		
		iter->Element=&element;
	
	}
	
	
	template <typename T>
	void RegexCompiler::add_capturing_group (T && key, GroupsMapping<typename std::decay<T>::type> & map, const RegexPatternElement & element) {
	
		add_capturing_group(element);
		
		map[std::forward<T>(key)].Elements.push_back(&element);
	
	}
	
	
	std::vector<RegexCompiler::GroupInfo>::iterator RegexCompiler::find_relative_capturing_group (std::ptrdiff_t dist) {
	
		auto iter=find_groups_end();
		
		if (dist==0) Raise("Cannot retrieve 0th relative capturing group");
		
		if (dist>0) {
		
			--dist;
			
			auto diff=dist-(groups.end()-iter);
			
			if (diff<0) return iter+dist;
			
			groups.resize(groups.size()+diff+1);
			
			return groups.end()-1;
		
		}
		
		if ((groups.begin()-iter)>dist) Raise("Cannot reference capturing group before beginning of pattern");
		
		return iter+dist;
	
	}
	
	
	RegexCompiler::Elements RegexCompiler::Compile (Iterator begin, Iterator end, RegexOptions options, const Unicode::Locale & locale) {
	
		std::size_t automatic=0;
		std::vector<GroupInfo> groups;
		GroupsMapping<std::size_t> numbered;
		GroupsMapping<String> named;
		RegexCompiler c(
			begin,
			begin,
			end,
			automatic,
			groups,
			numbered,
			named,
			options,
			Unicode::Check(options,RegexOptions::CultureInvariant) ? DefaultLocale : locale
		);
		c();
		
		c.complete();
		
		return c.Get();
	
	}
	
	
	void RegexCompiler::group_undeclared () const {
	
		Raise("Forward referenced capturing group was not declared");
	
	}
	
	
	template <typename T>
	void RegexCompiler::complete (GroupsMapping<T> & map) {
	
		for (auto & pair : map) {
		
			auto & info=pair.second;
			if (info.Pending.size()==0) continue;
			if (info.Elements.size()==0) group_undeclared();
			
			for (auto & pending : info.Pending) *pending=info.Elements[0];
		
		}
	
	}
	
	
	void RegexCompiler::complete () {
	
		complete(named);
		complete(numbered);
		
		for (auto & info : groups) {
		
			if (info.Pending.size()==0) continue;
			if (info.Element==nullptr) group_undeclared();
			
			for (auto & pending : info.Pending) *pending=info.Element;
		
		}
	
	}
	
	
	RegexCompiler::RegexCompiler (
		Iterator curr,
		Iterator begin,
		Iterator end,
		std::size_t & automatic,
		std::vector<GroupInfo> & groups,
		GroupsMapping<std::size_t> & numbered,
		GroupsMapping<String> & named,
		RegexOptions options,
		const Unicode::Locale & locale
	) noexcept
		:	RegexCompilerBase(curr,begin,end),
			automatic(automatic),
			groups(groups),
			numbered(numbered),
			named(named),
			Options(options),
			Locale(locale),
			CharacterClass(false)
	{	}
	
	
	RegexCompiler::RegexCompiler (const RegexCompiler & other) noexcept
		:	RegexCompiler(
				other.Current,
				other.Begin,
				other.End,
				other.automatic,
				other.groups,
				other.numbered,
				other.named,
				other.Options,
				other.Locale
			)
	{	}
	
	
	std::size_t RegexCompiler::GetCaptureNumber () noexcept {
	
		return ++automatic;
	
	}
	
	
	std::size_t RegexCompiler::GetLastCaptureNumber () noexcept {
	
		return automatic;
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (const String & key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(named[key],ptr);
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (String && key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(named[std::move(key)],ptr);
	
	}
	
	
	void RegexCompiler::GetCapturingGroup (std::size_t key, const RegexPatternElement * & ptr) {
	
		get_capturing_group(numbered[key],ptr);
	
	}
	
	
	void RegexCompiler::GetRelativeCapturingGroup (std::ptrdiff_t dist, const RegexPatternElement * & ptr) {
	
		auto iter=find_relative_capturing_group(dist);
		if (iter->Element==nullptr) iter->Pending.push_back(&ptr);
		else ptr=iter->Element;
	
	}
	
	
	const RegexPatternElement * RegexCompiler::GetCapturingGroup (const String & key) const {
	
		return get_capturing_group(named,key);
	
	}
	
	
	const RegexPatternElement * RegexCompiler::GetCapturingGroup (std::size_t key) const {
	
		return get_capturing_group(numbered,key);
	
	}
	
	
	void RegexCompiler::AddCapturingGroup (const String & key, const RegexPatternElement & element) {
	
		add_capturing_group(key,named,element);
	
	}
	
	
	void RegexCompiler::AddCapturingGroup (String && key, const RegexPatternElement & element) {
	
		add_capturing_group(std::move(key),named,element);
	
	}
	
	
	void RegexCompiler::AddCapturingGroup (std::size_t key, const RegexPatternElement & element) {
	
		add_capturing_group(key,numbered,element);
	
	}


}
