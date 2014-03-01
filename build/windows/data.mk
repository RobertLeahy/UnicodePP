all: data


.PHONY: data
data: \
bin/data.exe


bin/data.exe: \
obj/parsing/casing.o \
obj/parsing/codeoutput.o \
obj/parsing/main.o \
obj/parsing/parser.o \
obj/parsing/tointeger.o | \
bin/unicodedata.dll
	$(GPP) -o $@ $^ bin/unicodedata.dll