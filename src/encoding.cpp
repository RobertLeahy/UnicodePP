#include <unicode/encoding.hpp>
#include <unicode/vector.hpp>


namespace Unicode {


	const EncodingAction & Encoding::get (EncodingErrorType type) const noexcept {
	
		switch (type) {
		
			case EncodingErrorType::UnicodeStrict:
				return UnicodeStrict;
			case EncodingErrorType::Strict:
			default:
				return Strict;
			case EncodingErrorType::Lossy:
				return Lossy;
			case EncodingErrorType::UnexpectedEnd:
				return UnexpectedEnd;
			case EncodingErrorType::Endianness:
				return Endianness;
		
		}
	
	}


	std::optional<CodePoint> Encoding::check (CodePoint cp) const {
	
		//	TODO: Implement
		return cp;
	
	}
	
	
	std::optional<CodePoint> Encoding::handle (EncodingErrorType type, const void * where) const {
	
		auto & action=get(type);
		
		if (action.Ignored()) return std::nullopt;
		
		return action.Execute(where);
	
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
			Endianness(EncodingErrorType::Endianness),
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
	
		std::optional<Unicode::Endianness> order;
		if (DetectBOM) order=GetBOM(b,e);
		
		std::vector<CodePoint> retr;
		while (b!=e) {
		
			//	This is the position within the code point
			//	result vector we started at, so we can check
			//	each code point decoded to make sure it's
			//	acceptable Unicode
			std::size_t pos=retr.size();
		
			//	Track the start position, so if the decoder
			//	fails to advance the iterator we can do it
			auto start=b;
			
			//	Decode
			auto error=Decoder(retr,b,e,order);
			
			//	Handle error (if any)
			if (error) {
			
				auto repl=handle(*error,b);
				
				if (repl) retr.push_back(*repl);
			
			}
			
			//	If the iterator was not advanced, advance
			//	it
			if (b==start) ++b;
			
			//	Scan and check code points
			for (std::size_t i=pos;i<retr.size();) {
			
				auto & cp=retr[i];
				
				//	If the code point is valid, or strict Unicode
				//	errors are being ignored, move on
				if (cp.IsValid() || UnicodeStrict.Ignored()) {
				
					++i;
					
					continue;
				
				}
				
				//	Code point isn't valid, handle the error
				auto repl=handle(EncodingErrorType::UnicodeStrict,b);
				
				//	If there's a replacement, make it
				if (repl) {
				
					cp=*repl;
					
					++i;
					
					continue;
				
				}
				
				//	Otherwise remove this code point
				retr.erase(retr.begin()+i);
			
			}
		
		}
		
		return retr;
	
	}


}
