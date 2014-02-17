#include <unicode/utf8.hpp>
#include <iterator>
#include <limits>


namespace Unicode {


	static const unsigned char bom_bytes []={0xEFU,0xBBU,0xBFU};
	static const ByteOrderMark bom(
		std::begin(bom_bytes),
		std::end(bom_bytes)
	);


	static char to_char (UTF8::CodeUnit c) noexcept {
	
		union {
			UTF8::CodeUnit in;
			char out;
		};
		in=c;
		
		return out;
	
	}
	
	
	static CodePoint::Type get_integer (std::size_t bytes, CodePoint cp) noexcept {
	
		return static_cast<CodePoint::Type>(cp)>>(6*bytes);
	
	}
	
	
	static UTF8::CodeUnit get_lead (std::size_t bytes, CodePoint cp) noexcept {
	
		auto i=get_integer(bytes,cp);
		
		UTF8::CodeUnit retr=0;
		for (std::size_t n=0;n<bytes;++n) {
			retr>>=1;
			retr&=128;
		
		}
		
		return retr|static_cast<UTF8::CodeUnit>(i);
	
	}
	
	
	static UTF8::CodeUnit get_trail (std::size_t bytes, CodePoint cp) noexcept {
	
		return static_cast<UTF8::CodeUnit>(
			get_integer(bytes,cp)&0x3FU
		)|128;
	
	}
	
	
	void UTF8::to_bytes (std::vector<char> & buffer, CodePoint cp) const {
	
		//	Number of bytes we're going
		//	to use to represent this code
		//	point
		auto num=Count(cp);
		
		//	Code points that take only one
		//	byte are trivially representable
		if (num==1) {
		
			buffer.push_back(
				to_char(
					static_cast<CodeUnit>(cp)
				)
			);
		
			return;
		
		}
		
		buffer.push_back(
			to_char(
				get_lead(num,cp)
			)
		);
		
		for (--num;(num--)>0;) buffer.push_back(
			to_char(
				get_trail(num,cp)
			)
		);
	
	}
	
	
	static std::size_t count (UTF8::CodeUnit lead) noexcept {
	
		std::size_t retr=0;
		while ((lead&128)!=0) {
		
			++retr;
			lead<<=1;
		
		}
		
		return retr;
	
	}
	
	
	static CodePoint::Type get_lead_bits (std::size_t bytes, UTF8::CodeUnit lead) noexcept {
	
		lead<<=bytes;
		lead>>=bytes;
		
		return lead;
	
	}
	
	
	static CodePoint::Type get_trail_bits (UTF8::CodeUnit trail) noexcept {
	
		return trail&127;
	
	}
	
	
	bool UTF8::from_bytes (std::vector<CodePoint> & cps, const CodeUnit * & begin, const CodeUnit * end) const {
	
		//	Control returns here if an error
		//	is encountered with the current byte,
		//	after moving to the next byte
		start:
		
		//	Bounds check
		if (begin==end) return false;
	
		//	Determine number of code units in
		//	the next code point
		auto num=count(*begin);
		//	This code unit is the only
		//	code unit and contains literally
		//	the code point
		if (num==0) {
		
			cps.push_back(*begin);
			++begin;
			return true;
			
		}
		
		//	ILLEGAL UTF-8, CONTINUATION
		//	BYTE WHERE UNEXPECTED
		if (num==1) {
		
			Handle(
				cps,
				begin,
				EncodingErrorType::Strict
			);
			
			//	Skip to next byte
			++begin;
			goto start;
		
		}
		
		//	CANNOT REPRESENT
		if (num>6) {
		
			Handle(
				cps,
				begin,
				EncodingErrorType::Lossy
			);
			
			//	Skip to next byte
			++begin;
			goto start;
		
		}
		
		//	Not valid UTF-8
		if ((num>4) && !Handle(
			cps,
			begin,
			EncodingErrorType::Strict
		)) {
		
			++begin;
			goto start;
			
		}
		
		//	Check to see if there are enough
		//	code points left in the stream
		if (static_cast<std::size_t>(end-begin)<num) {
		
			Handle(
				cps,
				begin,
				EncodingErrorType::Strict
			);
			
			return false;
		
		}
		
		//	Build a code point
		CodePoint::Type cp;
		cp=get_lead_bits(num,*(begin++));
		for (--num;(num--)>0;) {
		
			//	Check for a valid continuation
			//	byte
			if (!(
				(((*begin)&0xC0)==128) ||
				Handle(
					cps,
					begin,
					EncodingErrorType::Strict
				)
			)) goto start;
			
			//	Get bits
			cp<<=6;
			cp|=get_trail_bits(*(begin++));
		
		}
		
		cps.push_back(cp);
		
		return true;
	
	}
	
	
	void UTF8::Encoder (std::vector<char> & buffer, const CodePoint * begin, const CodePoint * end) const {
	
		//	Output BOM if necessary
		if (
			(buffer.size()==0) &&
			OutputBOM
		) bom.Get(buffer);
	
		for (;begin!=end;++begin) {
		
			//	Unicode strictness -- report an error
			//	if the code point is not valid
			//	Unicode
			if (!(begin->IsValid() || Handle(
				buffer,
				begin,
				end,
				EncodingErrorType::UnicodeStrict
			))) continue;
			
			//	UTF-8 strictness, even though UTF-8
			//	can use up to 6 bytes to represent
			//	code points, the standard restricts it
			//	to only 4, therefore report an error
			//	if this code point would take more than
			//	4 bytes to represent
			if (!((Count(*begin)<=4) || Handle(
				buffer,
				begin,
				end,
				EncodingErrorType::Strict
			))) continue;
			
			//	Lossiness.  Some invalid Unicode code points
			//	(all the way up to 32 bits) cannot be
			//	represented by UTF-8, therefore report an
			//	error if this code point is
			//	unrepresentable
			if (!CanRepresent(*begin)) {
			
				//	There's no way to proceed, so unconditionally
				//	skip this code point
				Handle(
					buffer,
					begin,
					end,
					EncodingErrorType::Lossy
				);
				
				continue;
			
			}
			
			//	Encode
			to_bytes(buffer,*begin);
		
		}
	
	}
	
	
	void UTF8::Decoder (std::vector<CodePoint> & cps, const void * begin, const void * end) const {
	
		if (cps.size()==0) SkipBOM(begin,end,bom);
	
		auto b=reinterpret_cast<const CodeUnit *>(begin);
		auto e=reinterpret_cast<const CodeUnit *>(end);
	
		CodePoint cp;
		while (from_bytes(cps,b,e));
	
	}
	
	
	bool UTF8::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<=Max;
	
	}
	
	
	std::size_t UTF8::Count (CodePoint cp) const noexcept {
	
		if (cp<=0x7FU) return 1;
		if (cp<=0x7FFU) return 2;
		if (cp<=0xFFFFU) return 3;
		if (cp<=0x1FFFFFU) return 4;
		if (cp<=0x3FFFFFFU) return 5;
		if (cp<=Max) return 6;
		
		return 0;
	
	}


}
