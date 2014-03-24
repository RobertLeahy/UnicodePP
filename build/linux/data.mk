all: data


clean: cleandata


obj/data/getcollation.o: \
src/data/collation.cpp


obj/data/getdata.o: \
src/data/data.cpp


.PHONY: cleandata
cleandata:
	-@rm -r -f UCD
	-@rm -r -f UCA
	-@rm -f UCD.zip
	-@rm -f src/data/collation.cpp
	-@rm -f src/data/data.cpp


.PHONY: data
data: \
bin/data


bin/collation: \
obj/data/getdata.o \
obj/parsing/codeoutput.o \
obj/parsing/ducet.o \
obj/parsing/ducet_main.o | \
bin/unicodedata.so
	$(GPP) -o $@ $^ bin/unicodedata.so


bin/data: \
obj/parsing/casing.o \
obj/parsing/codeoutput.o \
obj/parsing/main.o \
obj/parsing/parser.o \
obj/parsing/tointeger.o | \
bin/unicodedata.so
	$(GPP) -o $@ $^ bin/unicodedata.so
	
	
src/data/collation.cpp: \
bin/collation | \
UCA/allkeys.txt
	$(call MKDIR,$@)
	bin/collation ./UCA/allkeys.txt ./src/data/collation.cpp
	
	
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
	
	
UCA:
	$(call MKDIR,$@)

	
UCA/allkeys.txt: | \
UCA
	wget -P ./UCA http://www.unicode.org/Public/UCA/latest/allkeys.txt
	