#include <unicode/encoding.hpp>
#include <unicode/vector.hpp>


namespace Unicode {


	std::optional<CodePoint> Encoding::check (CodePoint cp) const {
	
		//	TODO: Implement
		return cp;
	
	}


	void Encoding::GetBOM (std::vector<unsigned char> & buffer) const {
	
		BOM().Get(buffer);
	
	}
	
	
	std::optional<Endianness> Encoding::GetBOM (const unsigned char * & begin, const unsigned char * end) const {
	
		const void * b=begin;
		auto retr=BOM().Detect(b,end);
		begin=reinterpret_cast<const unsigned char *>(b);
		
		return retr;
	
	}


	Encoding::Encoding (bool output_bom, bool detect_bom) noexcept
		:	UnicodeStrict(EncodingErrorType::UnicodeStrict),
			Strict(EncodingErrorType::Strict),
			Lossy(EncodingErrorType::Lossy),
			UnexpectedEnd(EncodingErrorType::UnexpectedEnd),
			OutputBOM(output_bom),
			DetectBOM(detect_bom)
	{	}
	
	
	Encoding::~Encoding () noexcept {	}
	
	
	std::vector<unsigned char> Encoding::Encode (const CodePoint * begin, const CodePoint * end) const {
	
		std::vector<unsigned char> retr;
		
		if (OutputBOM) GetBOM(retr);
		
		for (;begin!=end;++begin) {
		
			auto cp=check(*begin);
			if (!cp) continue;
			
			Encoder(retr,*cp);
		
		}
		
		return retr;
	
	}
	
	
	std::vector<unsigned char> Encoding::Encode (const std::vector<CodePoint> & vec) const {
	
		return Encode(Begin(vec),End(vec));
	
	}
	
	
	std::vector<unsigned char> Encoding::Encode (const String & str) const {
	
		return Encode(str.CodePoints());
	
	}
	
	
	std::vector<CodePoint> Encoding::Decode (const void * begin, const void * end) const {
	
		auto b=reinterpret_cast<const unsigned char *>(begin);
		auto e=reinterpret_cast<const unsigned char *>(end);
	
		std::optional<Endianness> order;
		if (DetectBOM) order=GetBOM(b,e);
		
		std::vector<CodePoint> retr;
		while (b!=e) {
		
			auto error=Decoder(retr,b,e,order);
			
			//	TODO: Implement
			if (error) continue;
		
		}
		
		return retr;
	
	}


}
