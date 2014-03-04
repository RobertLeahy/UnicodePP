all: data


.PHONY: data
data: \
bin/data.exe


bin/collation.exe: \
obj/data/getdata.o \
obj/parsing/codeoutput.o \
obj/parsing/ducet.o \
obj/parsing/ducet_main.o | \
bin/unicodedata.dll
	$(GPP) -o $@ $^ bin/unicodedata.dll


bin/data.exe: \
obj/parsing/casing.o \
obj/parsing/codeoutput.o \
obj/parsing/main.o \
obj/parsing/parser.o \
obj/parsing/tointeger.o | \
bin/unicodedata.dll
	$(GPP) -o $@ $^ bin/unicodedata.dll
	
	
src/data/collation.cpp: \
bin/collation.exe | \
UCA/allkeys.txt
	$(call MKDIR,$@)
	bin/collation.exe ./UCA/allkeys.txt ./src/data/collation.cpp
	
	
src/data/data.cpp: \
bin/data.exe | \
UCD
	$(call MKDIR,$@)
	bin/data.exe ./UCD ./src/data/data.cpp