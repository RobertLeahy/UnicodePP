all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/boundaries.o \
obj/byteordermark.o \
obj/caseconverter.o \
obj/codepoint.o \
obj/condition.o \
obj/conditions.o \
obj/data/getdata.o \
obj/defaultlocale.o \
obj/encoding.o \
obj/encodingaction.o \
obj/encodingerror.o \
obj/error.o \
obj/locale.o \
obj/normalizer.o \
obj/string.o \
obj/utf8.o | \
bin
	$(GPP) -shared -o $@ $^