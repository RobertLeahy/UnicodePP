all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/byteencoding.o \
obj/byteordermark.o \
obj/codepoint.o \
obj/encoding.o \
obj/encodingerror.o \
obj/error.o \
obj/grapheme.o \
obj/string.o \
obj/utf8.o | \
bin
	$(GPP) -shared -o $@ $^