all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/ascii.o \
obj/boundaries.o \
obj/byteordermark.o \
obj/caseconverter.o \
obj/codepoint.o \
obj/collator.o \
obj/comparer.o \
obj/condition.o \
obj/conditions.o \
obj/data/getcollation.o \
obj/data/getdata.o \
obj/defaultlocale.o \
obj/encoding.o \
obj/encodingaction.o \
obj/encodingerror.o \
obj/endianencoding.o \
obj/error.o \
obj/latin1.o \
obj/locale.o \
obj/normalizer.o \
obj/regex.o \
obj/regexcharacterclass.o \
obj/regexcharacterclasses.o \
obj/regexcomment.o \
obj/regexcompiler.o \
obj/regexcompilerstate.o \
obj/regexerror.o \
obj/regexliteral.o \
obj/regexparser.o \
obj/regexpatternelement.o \
obj/regexpatternelementstate.o \
obj/regexpatternelementprivatestate.o \
obj/regexstate.o \
obj/regexunicodeproperties.o \
obj/regexwildcard.o \
obj/string.o \
obj/ucs2.o \
obj/utf8.o \
obj/utf16.o \
obj/utf32.o | \
bin
	$(GPP) -shared -o $@ $^