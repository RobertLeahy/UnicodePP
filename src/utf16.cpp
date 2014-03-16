#include <unicode/utf16.hpp>


namespace Unicode {


	static const unsigned char bom_array []={0xFE,0xFF};
	static const ByteOrderMark bom(bom_array);
	
	
	void UTF16::Encoder (std::vector<unsigned char> & buffer, CodePoint cp) const {
	
		auto len=Count(cp);
		
		//	This shouldn't happen
		if (len==0) return;
		
		//	Inside BMP, trivial
		if (len==1) {
		
			Get(buffer,static_cast<CodeUnit>(cp));
			
			return;
		
		}
		
		//	Outside BMP, surrogates required
		
		CodePoint::Type cpt=cp;
		cpt-=0x10000;
		Get(
			buffer,
			static_cast<CodeUnit>((cpt>>10)+0xD800)
		);
		Get(
			buffer,
			static_cast<CodeUnit>((cpt&0x3FF)+0xDC00)
		);
	
	}
	
	
	static bool is_lead (UTF16::CodeUnit cu) noexcept {
	
		return (cu>=0xD800) && (cu<=0xDBFF);
	
	}
	
	
	static bool is_trail (UTF16::CodeUnit cu) noexcept {
	
		return (cu>=0xDC00) && (cu<=0xDFFF);
	
	}
	
	
	std::optional<EncodingErrorType> UTF16::Decoder (
		std::vector<CodePoint> & cps,
		const unsigned char * & begin,
		const unsigned char * end,
		std::optional<Endianness> order
	) const {
	
		//	If we weren't given a detected byte
		//	order, we have to fallback to the one
		//	specified
		if (!order) {
		
			//	If there's no fallback byte order,
			//	die
			if (!InputOrder) return EncodingErrorType::Endianness;
			
			order=*InputOrder;
		
		}
		
		//	Get the first code unit
		auto lead=Get<CodeUnit>(begin,end,*order);
		//	If there was no code unit, the stream
		//	ended unexpectedly
		if (!lead) return EncodingErrorType::UnexpectedEnd;
		
		//	If this is a lead surrogate, we have to get
		//	a trail surrogate and compose them
		if (is_lead(*lead)) {
		
			//	Get the trail surrogate
			auto trail=Get<CodeUnit>(begin,end,*order);
			//	If there was no code point, the stream
			//	ended unexpectedly
			if (!trail) return EncodingErrorType::UnexpectedEnd;
			
			//	If the trail surrogate isn't actually a trail
			//	surrogate, we die
			if (!is_trail(*trail)) return EncodingErrorType::Strict;
			
			//	Join the surrogates
			auto cp=static_cast<CodePoint::Type>(*lead-0xD800);
			cp<<=10;
			cp|=*trail-0xDC00;
			cp+=0x10000;
			
			cps.push_back(cp);
			
			return std::nullopt;
		
		}
		
		//	If it's between 0xDC00 and 0xDFFF (inclusive)
		//	it's an orphaned trail surrogate, and we
		//	die
		if (is_trail(*lead)) return EncodingErrorType::Strict;
		
		//	The code unit is literally the code point
		cps.push_back(static_cast<CodePoint::Type>(*lead));
		
		return std::nullopt;
	
	}
	
	
	ByteOrderMark UTF16::BOM () const noexcept {
	
		return bom;
	
	}
	
	
	bool UTF16::CanRepresent (CodePoint cp) const noexcept {
	
		return cp<=CodePoint::Max;
	
	}
	
	
	std::size_t UTF16::Count (CodePoint cp) const noexcept {
	
		if (cp<=0xFFFF) return 1;
		if (cp<=CodePoint::Max) return 2;
		
		return 0;
	
	}


}
