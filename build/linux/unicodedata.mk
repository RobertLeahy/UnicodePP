all: unicodedata


.PHONY: unicodedata
unicodedata: bin/unicodedata.so


bin/unicodedata.so: \
obj/file.o \
obj/fileiterator.o \
obj/item.o \
obj/line.o | \
bin
	$(GPP) -shared -o $@ $^