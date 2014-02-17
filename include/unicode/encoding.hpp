/** 
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/error.hpp>
#include <unicode/grapheme.hpp>
#include <unicode/string.hpp>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>


namespace Unicode {


	/**
	 *	Represents an endianness.  I.e.
	 *	a sub-word byte ordering.
	 */
	enum class Endianness {
	
		Big,	/**<	Big endian	*/
		Little	/**<	Little endian	*/
	
	};


	/**
	 *	Represents a Unicode byte order mark.
	 */
	class ByteOrderMark {
	
	
		private:
		
		
			const unsigned char * b;
			const unsigned char * e;
			
			
			template <typename T>
			static std::reverse_iterator<typename std::decay<T>::type> reverse (T && iter) noexcept {
			
				return std::reverse_iterator<typename std::decay<T>::type>(
					std::forward<T>(iter)
				);
			
			}
			
			
		public:
		
		
			ByteOrderMark () noexcept : b(nullptr), e(nullptr) {	}
			/**
			 *	Creates a new ByteOrderMark.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a
			 *		sequence of bytes which gives the
			 *		big-endian representation of the
			 *		byte order mark.
			 *	\param [in] end
			 *		An iterator to the end of a sequence
			 *		of bytes which gives the big-endian
			 *		representation of the byte order
			 *		mark.
			 */
			ByteOrderMark (const void * begin, const void * end) noexcept
				:	b(reinterpret_cast<const unsigned char *>(begin)),
					e(reinterpret_cast<const unsigned char *>(end))
			{	}
			
			
			ByteOrderMark (const ByteOrderMark &) = default;
			ByteOrderMark (ByteOrderMark &&) = default;
			ByteOrderMark & operator = (const ByteOrderMark &) = default;
			ByteOrderMark & operator = (ByteOrderMark &&) = default;
			

			/**
			 *	Attempts to detect the endianness of a
			 *	sequence of bytes.
			 *
			 *	\param [in,out] begin
			 *		A reference to a pointer to the
			 *		beginning of the sequence of bytes.
			 *		If the endianness is detected this
			 *		pointer will point to the first byte
			 *		that is not part of the byte order
			 *		mark.
			 *	\param [in] end
			 *		A pointer to the end of the sequence
			 *		of bytes.
			 *
			 *	\return
			 *		An engaged optional containing the
			 *		endianness of the sequence if the
			 *		endianness could be detected,
			 *		\em false otherwise.
			 */
			std::optional<Endianness> Detect (const void * & begin, const void * end) const noexcept;
			/**
			 *	Attempts to detect the endianness of a
			 *	sequence of bytes.
			 *
			 *	\param [in] begin
			 *		A pointer to the beginning of the
			 *		sequence of bytes.
			 *	\param [in] end
			 *		A pointer to the end of the sequence
			 *		of bytes.
			 *
			 *	\return
			 *		An engaged optional containing the
			 *		endianness of the sequence if the
			 *		endianness could be detected,
			 *		\em false otherwise.
			 */
			std::optional<Endianness> Detect (const void * const & begin, const void * end) const noexcept {
			
				const void * b=begin;
				return Detect(b,end);
			
			}
			
			
			/**
			 *	Adds the byte order mark to the back
			 *	of a buffer of bytes.
			 *
			 *	\tparam T
			 *		The type of container being appended
			 *		to.
			 *
			 *	\param [in,out] container
			 *		The container of type \em T.
			 *	\param [in] order
			 *		The endianness of the BOM to output.
			 */
			template <typename T>
			void Get (T & container, Endianness order=Endianness::Big) const {
			
				auto inserter=std::back_inserter(container);
				
				if (order==Endianness::Big) std::copy(b,e,std::move(inserter));
				else std::copy(reverse(e),reverse(b),std::move(inserter));
			
			}
		
		
			const unsigned char * begin () const noexcept {
			
				return b;
			
			}
			
			
			const unsigned char * end () const noexcept {
			
				return e;
			
			}
			
			
			std::size_t Count () const noexcept {
			
				return static_cast<std::size_t>(e-b);
			
			}
	
	
	};


	/**
	 *	The various types of encoding
	 *	error.
	 */
	enum class EncodingErrorType {
	
		UnicodeStrict,	/**<	The input or output stream did not strictly conform to the Unicode standard.	*/
		Strict,			/**<	The input or output stream did not strictly conform to the encoding standard.	*/
		Lossy			/**<	The encoder encountered a code point that it could not represent.	*/
	
	};


	/**
	 *	The type of error that shall be thrown
	 *	when an error is encountered encoding
	 *	or decoding.
	 */
	class EncodingError : public Error {
	
	
		private:
		
		
			const void * w;
			EncodingErrorType t;
	
	
		public:
		
		
			/**
			 *	Creates a new EncodingError.
			 *
			 *	\param [in] what
			 *		A string describing the error
			 *		that was encountered.
			 *	\param [in] where
			 *		The index at which the encoding
			 *		error was encountered.
			 *	\param [in] type
			 *		The type of error that occurred.
			 */
			EncodingError (const char * what, const void * where, EncodingErrorType type) noexcept;
			
			
			/**
			 *	Returns the offset where the encoding
			 *	error was encountered.
			 *
			 *	\return
			 *		The location the error was encountered.
			 */
			const void * where () const noexcept;
			
			
			/**
			 *	Returns the type of error that occurred.
			 *
			 *	\return
			 *		The type of error that occurred.
			 */
			EncodingErrorType type () const noexcept;
	
	
	};


	/**
	 *	Specifies actions a Unicode encoder
	 *	may take when encountering some
	 *	condition.
	 */
	enum class EncodingAction {
	
		Nothing,	/**<	Specifies that the encoder shall do nothing.	*/
		Throw,		/**<	Specifies that the encoder shall throw an exception.	*/
		Replace		/**<	Specifies that the encoder shall make some replacement.	*/
	
	};


	/**
	 *	The base class for all Unicode encodings.
	 */
	class Encoding {
	
	
		private:
		
		
			class Action {
			
			
				public:
			
			
					EncodingAction What;
					const Grapheme & Replacement;
					const char * Message;
					
					
					Action (EncodingAction, const Grapheme &, const char *) noexcept;
			
			
			};
			
			
			std::optional<Action> get_action (EncodingErrorType) const noexcept;
	
	
		protected:
			
			
			bool Handle (
				std::vector<char> & buffer,
				const CodePoint * begin,
				const CodePoint * end,
				EncodingErrorType type,
				const char * what=nullptr
			) const;
			bool Handle (
				std::vector<CodePoint> & cps,
				const void * where,
				EncodingErrorType type,
				const char * what=nullptr
			) const;
			
			
			virtual void Encoder (std::vector<char> & buffer, const CodePoint * begin, const CodePoint * end) const = 0;
			virtual void Decoder (std::vector<CodePoint> & cps, const void * begin, const void * end) const = 0;
	
	
		public:
		
		
			/**
			 *	If \em true, the encoder should strictly
			 *	enforce the Unicode standard.  For example
			 *	by refusing to encode or decode code points
			 *	that are not valid Unicode.
			 */
			bool UnicodeStrict;
			/**
			 *	The action the encoder should take when a
			 *	Unicode compliance error is encountered.
			 */
			EncodingAction UnicodeStrictAction;
			/**
			 *	The replacement the encoder should use when
			 *	a Unicode compliance error is encountered
			 *	and UnicodeStrictAction is EncodingAction::Replace.
			 */
			Grapheme UnicodeStrictReplacement;
			/**
			 *	If \em true, the encoder should strictly
			 *	enforce its encoding.  For example some
			 *	encodings can technically represent certain
			 *	values, but their standardization disallows
			 *	this.  In strict mode this would be an
			 *	error condition.
			 */
			bool Strict;
			/**
			 *	The action the encoder should take when
			 *	a complaince error is encountered.
			 */
			EncodingAction StrictAction;
			/**
			 *	The replacement the encoder should use
			 *	when a compliance error is encountered
			 *	and StrictAction is EncodingAction::Replace.
			 */
			Grapheme StrictReplacement;
			/**
			 *	The action the encoder should take when
			 *	it encounters a situation wherein it would
			 *	be \"lossy\", for example skipping a code
			 *	point it cannot represent (in the case
			 *	of an encoding that cannot fully
			 *	represent Unicode).
			 */
			EncodingAction LossyAction;
			/**
			 *	The replacement the encoder should use
			 *	when it encounters a code point it cannot
			 *	represent and LossyAction is EncodingAction::Replace.
			 *
			 *	If the encoder cannot represent this code
			 *	point either, it should throw an exception.
			 */
			Grapheme LossyReplacement;
			/**
			 *	\em true if the encoder should prepend all
			 *	encoded output with an appropriate byte order
			 *	mark, \em false otherwise.
			 */
			bool OutputBOM;
			
			
			Encoding () noexcept;
			
			
			Encoding (const Encoding &) = delete;
			Encoding (Encoding &&) = delete;
			Encoding & operator = (const Encoding &) = delete;
			Encoding & operator = (Encoding &&) = delete;
			
			
			/**
			 *	Encodes a string.
			 *
			 *	\param [in] str
			 *		A string.
			 *
			 *	\return
			 *		A vector of bytes containing the
			 *		encoded representation of the string.
			 */
			std::vector<char> Encode (const String & str) const;
			/**
			 *	Encodes a string.
			 *
			 *	\param [in] str
			 *		A vector containing the code points of
			 *		the string.
			 *
			 *	\return
			 *		A vector of bytes containing the
			 *		encoded representation of the string.
			 */
			std::vector<char> Encode (const std::vector<CodePoint> & cps) const;
			/**
			 *	Encodes a string.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a contiguous
			 *		range of code points.
			 *	\param [in] end
			 *		An iterator to one past the end of a contiguous
			 *		range of code points.
			 *
			 *	\return
			 *		A vector of bytes containing the
			 *		encoded representation of the string.
			 */
			std::vector<char> Encode (const CodePoint * begin, const CodePoint * end) const;
			
			
			/**
			 *	Decodes a string.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of a contiguous
			 *		range of bytes.
			 *	\param [in] end
			 *		An iterator to the end of a contiguous range
			 *		of bytes.
			 *
			 *	\return
			 *		A string.
			 */
			String Decode (const void * begin, const void * end) const;
			
			
			/**
			 *	Determines whether the encoder can losslessly
			 *	represent a certain code point.
			 *
			 *	\param [in] cp
			 *		The code point.
			 *
			 *	\return
			 *		\em true if the encoder can losslessly
			 *		represent \em cp, \em false otherwise.
			 */
			virtual bool CanRepresent (CodePoint cp) const noexcept = 0;
			/**
			 *	Determines whether the encoder can losslessly
			 *	represent a certain grapheme.
			 *
			 *	\param [in] g
			 *		The grapheme.
			 *
			 *	\return
			 *		\em true if the encoder can losslessly
			 *		represent \em g, \em false otherwise.
			 */
			bool CanRepresent (const Grapheme & g) const noexcept;
			/**
			 *	Determines the number of bytes the encoder
			 *	will require to represent a certain code point.
			 *
			 *	\param [in] cp
			 *		The code point.
			 *
			 *	\return
			 *		The number of bytes this encoder will require
			 *		to represent \em cp, or zero if this encoder
			 *		cannot represent \em cp.
			 */
			virtual std::size_t Count (CodePoint cp) const noexcept = 0;
			/**
			 *	Determines the number of bytes the encoder will
			 *	require to represent a certain grapheme.
			 *
			 *	\param [in] g
			 *		The grapheme.
			 *
			 *	\return
			 *		The number of bytes this encoder will require
			 *		to represent \em g.
			 */
			std::size_t Count (const Grapheme & g) const noexcept;
	
	
	};


}
