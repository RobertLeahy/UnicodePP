all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/codepoint.o \
obj/condition.o \
obj/conditions.o \
obj/data/getdata.o \
obj/defaultlocale.o \
obj/locale.o | \
bin
	$(GPP) -shared -o $@ $^