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


	std::optional<CodePoint> Encoding::check (const CodePoint & cp) const {
	
		CodePoint retr=cp;
		const void * loc=&cp;
		
		//	Is this code point valid Unicode?  If not, are
		//	Unicode strict errors being ignored?
		if (!(retr.IsValid() || UnicodeStrict.Ignored())) {
		
			auto repl=UnicodeStrict.Execute(loc);
			
			if (!repl) return std::nullopt;
			
			retr=*repl;
		
		}
		
		//	Can the encoder represent this code point?
		if (!CanRepresent(retr)) {
		
			//	NO
			
			//	Are lossy errors being ignored?  If so just
			//	delete this code point from the stream
			if (Lossy.Ignored()) return std::nullopt;
			
			//	Lossy errors not ignored -- handle it
			
			auto repl=Lossy.Execute(loc);
			
			if (!repl) return std::nullopt;
			
			//	Can the replacement be represented?
			//	If not, WE MUST THROW
			if (!CanRepresent(*repl)) Lossy.Throw(loc);
			
			retr=*repl;
		
		}
		
		return retr;
	
	}
	
	
	std::optional<CodePoint> Encoding::handle (EncodingErrorType type, const void * where) const {
	
		//	Endianness errors are irrecoverable
		if (type==EncodingErrorType::Endianness) Endianness.Throw(where);
	
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
	
		//	Detect order (if applicable)
		std::optional<Unicode::Endianness> order;
		auto b=reinterpret_cast<const unsigned char *>(begin);
		if (DetectBOM) order=GetBOM(
			b,
			reinterpret_cast<const unsigned char *>(end)
		);
		begin=b;
		
		//	Decode
		std::vector<CodePoint> retr;
		Stream(retr,begin,end,order);
		
		//	Handle unexpected end
		if (begin!=end) {
		
			auto repl=handle(EncodingErrorType::UnexpectedEnd,begin);
			
			if (repl) retr.push_back(*repl);
		
		}
		
		return retr;
	
	}
	
	
	std::optional<CodePoint> Encoding::Stream (const void * & begin, const void * end, std::optional<Unicode::Endianness> order) const {
	
		//	Convert iterators as appropriate
		auto b=reinterpret_cast<const unsigned char *>(begin);
		auto e=reinterpret_cast<const unsigned char *>(end);
	
		while (b!=e) {
		
			//	Cache start location
			auto start=b;
			
			//	Decode
			CodePoint cp;
			auto error=Decoder(cp,b,e,order);
			
			//	If the decoder didn't advance the iterator, do so to
			//	avoid a potential infinite loop
			if (b==start) ++b;
			
			//	Handle error (if any)
			if (error) {
			
				//	If an unexpected end is encountered, that's fine.
				//
				//	This is a stream, which means there's liable to be
				//	more data to come, so unexpected end isn't an issue,
				//	it just means that we can't extract data right now.
				//
				//	However, we don't want to advance the iterator past
				//	the spot where we just tried to extract a character,
				//	so we set begin to where it was before we last tried
				//	to decode, and then return a disengaged optional
				if (*error==EncodingErrorType::UnexpectedEnd) {
				
					begin=start;
					
					return std::nullopt;
				
				}
				
				//	Handle this error
				auto repl=handle(*error,start);
				
				//	If there's a replacement to be made, make it
				if (repl) {
				
					begin=b;
					
					return *repl;
				
				}
				
				//	Otherwise there's no code point, so just loop
				//	again
				continue;
			
			//	If there's no error, the code point could still be invalid
			//	Unicode, check this unless Unicode strict errors are being
			//	ignored
			} else if (!(UnicodeStrict.Ignored() || cp.IsValid())) {
			
				auto repl=handle(EncodingErrorType::UnicodeStrict,start);
				
				if (repl) {
				
					begin=b;
					
					return *repl;
				
				}
				
				continue;
			
			}
			
			//	Return the recovered code point and advance
			//	the iterator
			
			begin=b;
			
			return cp;
			
		}
		
		//	Falling through to here means the end of the
		//	buffer was reached gracefully, meaning there's
		//	no character to return, but the begin iterator
		//	should be fast forwarded all the way to the end
		
		begin=end;
		
		return std::nullopt;
	
	}
	
	
	void Encoding::Stream (std::vector<CodePoint> & cps, const void * & begin, const void * end, std::optional<Unicode::Endianness> order) const {
	
		for (;;) {
		
			auto cp=Stream(begin,end,order);
			
			if (!cp) return;
			
			cps.push_back(*cp);
		
		}
	
	}
	
	
	ByteOrderMark Encoding::BOM () const noexcept {
	
		return ByteOrderMark{};
	
	}


}
