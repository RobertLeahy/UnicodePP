all: test


.PHONY: test
test: \
bin/tests.exe


bin/tests.exe: \
obj/test/main.o | \
bin/unicode.dll
	$(GPP) -o $@ $^ bin/unicode.dll
	bin/tests.exe
