all: unicodedata


.PHONY: unicodedata
unicodedata: bin/unicodedata.dll


bin/unicodedata.dll: \
obj/condition.o \
obj/file.o \
obj/item.o \
obj/line.o | \
bin
	$(GPP) -shared -o $@ $^