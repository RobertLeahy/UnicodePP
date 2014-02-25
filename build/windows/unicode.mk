all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/byteencoding.o \
obj/byteordermark.o \
obj/codepoint.o \
obj/condition.o \
obj/defaultlocale.o \
obj/endianencoding.o \
obj/encoding.o \
obj/encodingerror.o \
obj/error.o \
obj/normalizer.o \
obj/grapheme.o \
obj/locale.o \
obj/string.o \
obj/utf8.o \
obj/utf32.o | \
bin
	$(GPP) -shared -o $@ $^