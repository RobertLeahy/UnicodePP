/**
 *	\file
 */
 
 
#pragma once


#include <unicode/codepoint.hpp>
#include <unicode/error.hpp>
#include <unicode/makereverseiterator.hpp>
#include <unicode/string.hpp>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <utility>


namespace Unicode {


	/**
	 *	An enumeration of the various types of errors
	 *	that can be encountered when encoding or
	 *	decoding.
	 */
	enum class EncodingErrorType {
	
		/**
		 *	The input or output stream failed to strictly
		 *	conform to the Unicode standard.
		 */
		UnicodeStrict,
		/**
		 *	The input or output stream failed to strictly
		 *	conform to the encoding standard.
		 */
		Strict,
		/**
		 *	A code point was encountered which could not be
		 *	represented.
		 */
		Lossy,
		/**
		 *	The stream ended unexpectedly.
		 */
		UnexpectedEnd
	
	};


	/**
	 *	The type of exception which is thrown when an encoder
	 *	experiences an error.
	 */
	class EncodingError : public Error {
	
	
		private:
		
		
			EncodingErrorType t;
			const void * w;
			
			
		public:
		
		
			EncodingError (EncodingErrorType, const void *, const char *) noexcept;
			
		
			/**
			 *	Determines where in the stream the error occurred.
			 *
			 *	\return
			 *		A pointer to the location at which the error was
			 *		experienced.
			 */
			const void * where () const noexcept;
			/**
			 *	Determines the type of error that occurred.
			 *
			 *	\return
			 *		The type of error that occurred.
			 */
			EncodingErrorType type () const noexcept;
	
	
	};
	
	
	/**
	 *	Encapsulates an action to be taken when an error
	 *	encoding or decoding is experienced.
	 */
	class EncodingAction {
	
	
		private:
		
		
			enum class Action {
			
				Nothing,
				Throw,
				Replace,
			
			};
			
			
			bool ignore;
			Action action;
			EncodingErrorType type;
			CodePoint cp;
			
			
			[[noreturn]]
			void raise (const void *);
			
			
		public:
		
		
			/**
			 *	Creates a new EncodingAction.
			 *
			 *	\param [in] type
			 *		The type of error.
			 *	\param [in] throws
			 *		\em true if this action should throw, \em false
			 *		otherwise.  Defaults to \em true.
			 *	\param [in] ignore
			 *		\em true if this error type is ignorable, \em false
			 *		otherwise.  Defaults to \em false.
			 */
			explicit constexpr EncodingAction (EncodingErrorType type, bool throws=true, bool ignore=false) noexcept
				:	ignore(ignore),
					action(throws ? Action::Throw : Action::Nothing),
					type(type)
			{	}
			/**
			 *	Creates a new EncodingAction.
			 *
			 *	\param [in] type
			 *		The type of error.
			 *	\param [in] cp
			 *		The code point that shall be substituted in case
			 *		of an error.
			 *	\param [in] ignore
			 *		\em true if this error type is ignorable, \em false
			 *		otherwise.  Defaults to \em false.
			 */
			constexpr EncodingAction (EncodingErrorType type, CodePoint cp, bool ignore=false) noexcept
				:	ignore(ignore),
					action(Action::Replace),
					type(type),
					cp(cp)
			{	}
			
			
			/**
			 *	Executes the error action.
			 *
			 *	\param [in] where
			 *		The location at which the error occurred.
			 *
			 *	\return
			 *		A disengaged optional if no replacement is to be
			 *		made.  An engaged optional containing a replacement
			 *		code point if a replacement is to be made.  This
			 *		method does not return if the action is to throw.
			 */
			std::optional<CodePoint> Execute (const void * where);
			/**
			 *	Throws.
			 *
			 *	\param [in] where
			 *		The location at which the error occurred.
			 */
			[[noreturn]]
			void Throw (const void * where);
			/**
			 *	Determines whether errors of this type may be
			 *	ignored.
			 *
			 *	\return
			 *		\em true if errors of this type are ignorable,
			 *		\em false otherwise.
			 */
			constexpr bool Ignored () const noexcept {
			
				return ignore;
			
			}
	
	
	};

	
	/**
	 *	An enumeration containing the types of endianness.
	 */
	enum class Endianness {
	
		/**
		 *	Big-endian.  Most significant byte first.
		 */
		Big,
		/**
		 *	Little-endian.  Least significant byte first.
		 */
		Little
	
	};
	
	
	/**
	 *	Encapsulates a byte order mark (i.e.\ BOM).
	 *
	 *	All BOMs are considered to default to their
	 *	big endian forms.
	 */
	class ByteOrderMark {
	
	
		private:
		
		
			Array<unsigned char> bom;
			
			
		public:
		
		
			/**
			 *	Creates a ByteOrderMark from an array.
			 *
			 *	\param [in] arr
			 *		The array.
			 */
			template <std::size_t i>
			constexpr ByteOrderMark (const unsigned char (& arr) [i]) noexcept : bom(arr,i) {	}
		
		
			/**
			 *	Attempts to detect the endianness of a sequence
			 *	of bytes.
			 *
			 *	\param [in,out] begin
			 *		An iterator to the beginning of the sequence
			 *		of bytes.  If the endianness is successfully
			 *		detected this iterator will be advanced to one
			 *		past the end of the byte order mark.
			 *	\param [in] end
			 *		An iterator to the end of the sequence of bytes.
			 *
			 *	\return
			 *		An engaged optional containing the detected
			 *		endianness on success, a disengaged optional
			 *		otherwise.
			 */
			std::optional<Endianness> Detect (const void * & begin, const void * end) const noexcept;
			/**
			 *	Attempts to detect the endianness of a sequence
			 *	of bytes.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the sequence
			 *		of bytes.
			 *	\param [in] end
			 *		An iterator to the end of the sequence of bytes.
			 *
			 *	\return
			 *		An engaged optional containing the detected
			 *		endianness on success, a disengaged optional
			 *		otherwise.
			 */
			std::optional<Endianness> Detect (const void * const & begin, const void * end) const noexcept {
			
				const void * b=begin;
				return Detect(b,end);
			
			}
			
			
			/**
			 *	Places the byte order mark into a container.
			 *
			 *	\tparam T
			 *		The type of parameter into which to insert the
			 *		byte order mark.
			 *
			 *	\param [in,out] container
			 *		The container into which to insert the byte order
			 *		mark.
			 *	\param [in] order
			 *		The endianness of the byte order mark to insert.
			 *		Optional.  Defaults to big-endian.
			 */
			template <typename T>
			void Get (T & container, Endianness order=Endianness::Big) const {
			
				auto inserter=std::back_inserter(container);
				
				auto b=bom.begin();
				auto e=bom.end();
				
				if (order==Endianness::Big) std::copy(b,e,std::move(inserter));
				else std::copy(
					MakeReverseIterator(e),
					MakeReverseIterator(b),
					std::move(inserter)
				);
			
			}
	
	
	};
	

	/**
	 *	The base class from which all encoders derive.
	 */
	class Encoding {
	
	
		private:
		
		
			std::optional<CodePoint> check (CodePoint cp) const;
	
	
		protected:
		
		
			virtual void Encoder (
				std::vector<unsigned char> & buffer,
				CodePoint cp
			) const = 0;
			
			virtual std::optional<EncodingErrorType> Decoder (
				std::vector<CodePoint> & cps,
				const unsigned char * & begin,
				const unsigned char * end,
				std::optional<Endianness> order
			) const = 0;
			
			virtual void GetBOM (std::vector<unsigned char> & buffer) const;
			
			virtual std::optional<Endianness> GetBOM (
				const unsigned char * & begin,
				const unsigned char * end
			) const;
	
	
		public:
		

			/**
			 *	The action that shall be taken when an error
			 *	relating to Unicode conformance is encountered.
			 */
			EncodingAction UnicodeStrict;		
			/**
			 *	The action that shall be taken when an error
			 *	relating to encoding standard conformance
			 *	is encountered.
			 */
			EncodingAction Strict;
			/**
			 *	The action that shall be taken when the underlying
			 *	encoding cannot represent a certain code point
			 *	or sequence.
			 */
			EncodingAction Lossy;
			/**
			 *	The action that shall be taken when the end of the
			 *	stream is unexpectedly encountered.
			 */
			EncodingAction UnexpectedEnd;
			/**
			 *	\em true if all output should begin with the
			 *	underlying encoding's BOM.  \em false otherwise.
			 */
			bool OutputBOM;
			/**
			 *	\em true if an attempt should be made at the
			 *	beginning of all input to detect the BOM.
			 *	\em false otherwise.
			 */
			bool DetectBOM;
			
			
			/**
			 *	Creates a new Encoding.
			 *
			 *	\param [in] output_bom
			 *		The initial value for OutputBOM.  Defaults to
			 *		\em true.
			 *	\param [in] detect_bom
			 *		The initial value for DetectBOM.  Defaults to
			 *		\em true.
			 */
			Encoding (bool output_bom=true, bool detect_bom=true) noexcept;
			Encoding (const Encoding &) = delete;
			Encoding (Encoding &&) = delete;
			Encoding & operator = (const Encoding &) = delete;
			Encoding & operator = (Encoding &&) = delete;
			virtual ~Encoding () noexcept;
			
			
			/**
			 *	Encodes a range of code points.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of code points.
			 *	\param [in] end
			 *		An iterator to the end of the range of
			 *		code points.
			 *
			 *	\return
			 *		A buffer of bytes containing the range
			 *		as encoded by the underlying encoding.
			 */
			std::vector<unsigned char> Encode (const CodePoint * begin, const CodePoint * end) const;
			/**
			 *	Encodes a vector of code points.
			 *
			 *	\param [in] vec
			 *		The vector of code points.
			 *
			 *	\return
			 *		A buffer of bytes containing the range
			 *		as encoded by the underlying encoding.
			 */
			std::vector<unsigned char> Encode (const std::vector<CodePoint> & vec) const;
			/**
			 *	Encodes a Unicode string.
			 *
			 *	\param [in] str
			 *		The string.
			 *
			 *	\return
			 *		A buffer of bytes containing the range
			 *		as encoded by the underlying encoding.
			 */
			std::vector<unsigned char> Encode (const String & str) const;
			
			
			/**
			 *	Decodes a range of bytes.
			 *
			 *	\param [in] begin
			 *		An iterator to the beginning of the range
			 *		of bytes.
			 *	\param [in] end
			 *		An iterator to the end of the range of bytes.
			 *
			 *	\return
			 *		A vector of the code points represented by
			 *		the range of bytes.
			 */
			std::vector<CodePoint> Decode (const void * begin, const void * end) const;
			
			
			/**
			 *	Retrieves the byte order mark for this
			 *	encoding.
			 *
			 *	\return
			 *		The byte order mark.
			 */
			virtual ByteOrderMark BOM () const noexcept = 0;
			/**
			 *	Determines whether this encoding may losslessly
			 *	represent a certain code point.
			 *
			 *	\param [in] cp
			 *		The code point to check.
			 *
			 *	\return
			 *		
			 */
			virtual bool CanRepresent (CodePoint cp) const noexcept = 0;
			/**
			 *	Determines the number of bytes this encoding will
			 *	require to represent a certain code point.
			 *
			 *	\param [in] cp
			 *		The code point to check.
			 *
			 *	\return
			 *		The number of bytes this encoding will require to
			 *		represent \em cp, or 0 if this encoding cannot
			 *		represent \em cp;
			 */
			virtual std::size_t Count (CodePoint cp) const noexcept = 0;
	
	
	};


}
