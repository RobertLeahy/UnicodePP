#pragma once


#include <cstddef>
#include <fstream>
#include <string>
#include <type_traits>
#include <utility>


class NewlineT {	};


extern const NewlineT Newline;


class CodeOutput {


	private:
	
	
		std::fstream out;
		std::size_t indent;
		bool line;
		
		
	public:
	
	
		CodeOutput () = delete;
		CodeOutput (const std::string &);
		
		
		void BeginIndent ();
		void EndIndent ();
		
		
		void WhiteSpace ();
		
		
		void Header (const std::string &, bool local=false);
		
		
		void End ();
		
		
		void WriteString (const std::string &);
		
		
		void BeginNamespace (const std::string &);
		void EndNamespace ();
		
		
		void BeginArray (const std::string &, const std::string &);
		void EndArray ();
		
		
		template <typename T>
		typename std::enable_if<
			!std::is_same<
				typename std::decay<T>::type,
				NewlineT
			>::value,
			CodeOutput
		>::type & operator << (T && obj) {
		
			line=true;
		
			out << std::forward<T>(obj);
			
			return *this;
		
		}
		
		
		CodeOutput & operator << (const NewlineT &);


};
