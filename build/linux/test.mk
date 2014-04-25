.PHONY: test
test: \
bin/regex_tests \
bin/tests
	
	
include/catch.hpp:
	wget -P ./include https://raw.github.com/philsquared/Catch/master/single_include/catch.hpp
	
	
bin/regex_tests: \
obj/test/catch.o \
obj/test/main.o \
obj/test/regex/anchor.o \
obj/test/regex/characterclasses.o \
obj/test/regex/lookaround.o \
obj/test/regex/misc.o \
obj/test/regex/multiple.o \
obj/test/regex/recurse.o \
obj/test/regex/replace.o \
obj/test/regex/split.o \
obj/test/regex/validunicode.o \
obj/test/regex/wildcard.o | \
bin/unicode.so
	$(GPP) -o $@ $^ bin/unicode.so
	bin/regex_tests
	
	
bin/tests: \
include/catch.hpp \
obj/test/main.o | \
bin/unicode.so
	$(GPP) -o $@ obj/test/main.o bin/unicode.so
	bin/tests
