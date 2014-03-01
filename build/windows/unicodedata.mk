all: unicodedata


.PHONY: unicodedata
unicodedata: bin/unicodedata.dll


bin/unicodedata.dll: \
obj/file.o \
obj/fileiterator.o \
obj/item.o \
obj/line.o | \
bin
	$(GPP) -shared -o $@ $^