#include <unicode/encoding.hpp>


namespace Unicode {


	static const char * get_error_message (EncodingErrorType type) noexcept {
	
		switch (type) {
		
			case EncodingErrorType::UnicodeStrict:
				return "The stream does not conform to the Unicode standard";
			case EncodingErrorType::Strict:
				return "The stream does not conform to the encoding standard";
			case EncodingErrorType::Lossy:
				return "The stream cannot be exactly represented";
			case EncodingErrorType::UnexpectedEnd:
				return "The stream ended unexpectedly";
			case EncodingErrorType::Endianness:
				return "There was a problem with the stream's endianness";
			default:
				return "ERROR";
		
		}
	
	}
	
	
	void EncodingAction::raise (const void * where) const {
	
		throw EncodingError(
			type,
			where,
			get_error_message(type)
		);
	
	}


	std::optional<CodePoint> EncodingAction::Execute (const void * where) const {
	
		switch (action) {
		
			case Action::Nothing:
				return std::optional<CodePoint>{};
			case Action::Replace:
				return cp;
			default:
				raise(where);
		
		}
	
	}
	
	
	void EncodingAction::Throw (const void * where) const {
	
		raise(where);
	
	}


}
