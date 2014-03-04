all: data


.PHONY: data
data: \
bin/data


bin/data: \
obj/parsing/casing.o \
obj/parsing/codeoutput.o \
obj/parsing/main.o \
obj/parsing/parser.o \
obj/parsing/tointeger.o | \
bin/unicodedata.so
	$(GPP) -o $@ $^ bin/unicodedata.so
	
	
src/data/data.cpp: \
bin/data | \
UCD
	$(call MKDIR,$@)
	bin/data ./UCD ./src/data/data.cpp
	
	
UCD: | \
UCD.zip
	unzip UCD.zip -d UCD
	
	
UCD.zip:
	wget http://www.unicode.org/Public/UNIDATA/UCD.zip
	