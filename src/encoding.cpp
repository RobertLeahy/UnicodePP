#include <unicode/encoding.hpp>


namespace Unicode {


	Encoding::Action::Action (EncodingAction action, const Grapheme & replacement, const char * msg) noexcept
		:	What(action),
			Replacement(replacement),
			Message(msg)
	{	}


	static const char * unicode_strict="Stream does not conform to the Unicode standard";
	static const char * strict="Stream does not conform to the encoding standard";
	static const char * lossy="Stream cannot be losslessly represented by this encoding";
	static const char * lossy_unrepresentable="Lossy replacement cannot be represented";
	
	
	std::optional<Encoding::Action> Encoding::get_action (EncodingErrorType type) const noexcept {
	
		std::optional<Action> retr;
	
		switch (type) {
		
			case EncodingErrorType::UnicodeStrict:
				if (!UnicodeStrict) break;
				retr.emplace(
					UnicodeStrictAction,
					UnicodeStrictReplacement,
					unicode_strict
				);
				break;
				
			case EncodingErrorType::Strict:
				if (!Strict) break;
				retr.emplace(
					StrictAction,
					StrictReplacement,
					strict
				);
				break;
				
			default:
				retr.emplace(
					LossyAction,
					LossyReplacement,
					lossy
				);
				break;
		
		}
		
		return retr;
	
	}


	bool Encoding::Handle (
		std::vector<char> & buffer,
		const CodePoint * begin,
		const CodePoint * end,
		EncodingErrorType type,
		const char * what
	) const {
	
		auto action=get_action(type);
		if (!action) return true;
		
		switch (action->What) {
		
			default:
				break;
				
			case EncodingAction::Throw:
				throw EncodingError(
					action->Message,
					begin,
					type
				);
				
			case EncodingAction::Replace:
				if (
					(type==EncodingErrorType::Lossy) &&
					!CanRepresent(action->Replacement)
				) throw EncodingError(
					lossy_unrepresentable,
					begin,
					type
				);
				
				Encoder(
					buffer,
					action->Replacement.begin(),
					action->Replacement.end()
				);
				
				break;
		
		}
		
		return false;
	
	}
	
	
	bool Encoding::Handle (
		std::vector<CodePoint> & cps,
		const void * where,
		EncodingErrorType type,
		const char * what
	) const {
	
		auto action=get_action(type);
		if (!action) return true;
		
		switch (action->What) {
		
			default:
				break;
				
			case EncodingAction::Throw:
				throw EncodingError(
					action->Message,
					where,
					type
				);
				
			case EncodingAction::Replace:
				for (auto cp : action->Replacement) cps.push_back(cp);
				break;
		
		}
		
		return false;
	
	}


	Encoding::Encoding () noexcept
		:	UnicodeStrict(true),
			UnicodeStrictAction(EncodingAction::Throw),
			UnicodeStrictReplacement('?'),
			Strict(true),
			StrictAction(EncodingAction::Throw),
			StrictReplacement('?'),
			LossyAction(EncodingAction::Nothing),
			LossyReplacement(' '),
			OutputBOM(false)
	{	}
	
	
	std::vector<char> Encoding::Encode (const String & str) const {
	
		return Encode(str.CodePoints());
	
	}
	
	
	std::vector<char> Encoding::Encode (const std::vector<CodePoint> & cps) const {
	
		return Encode(&cps[0],&cps[0]+cps.size());
	
	}
	
	
	std::vector<char> Encoding::Encode (const CodePoint * begin, const CodePoint * end) const {
	
		std::vector<char> retr;
		Encoder(retr,begin,end);
		
		return retr;
	
	}
	
	
	String Encoding::Decode (const void * begin, const void * end) const {
	
		std::vector<CodePoint> retr;
		Decoder(retr,begin,end);
		
		return retr;
	
	}
	
	
	bool Encoding::CanRepresent (const CodePoint * begin, const CodePoint * end) const noexcept {
	
		while (begin!=end) if (!CanRepresent(*(begin++))) return false;
		
		return true;
	
	}
	
	
	bool Encoding::CanRepresent (const Grapheme & g) const noexcept {
	
		return CanRepresent(g.begin(),g.end());
	
	}
	
	
	bool Encoding::CanRepresent (const std::vector<CodePoint> & cps) const noexcept {
	
		return CanRepresent(&cps[0],&cps[0]+cps.size());
	
	}
	
	
	bool Encoding::CanRepresent (const String & str) const noexcept {
	
		return CanRepresent(str.CodePoints());
	
	}


}
