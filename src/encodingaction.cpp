#include <unicode/encoding.hpp>


namespace Unicode {


	static const char * get_error_message (EncodingErrorType type) noexcept {
	
		//	TODO: Make this more meaningful
		return "ERROR";
	
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
