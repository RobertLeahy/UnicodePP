.PHONY: test
test: \
bin/tests
	
	
include/catch.hpp:
	wget -P ./include https://raw.github.com/philsquared/Catch/master/single_include/catch.hpp
	
	
bin/tests: \
include/catch.hpp \
obj/test/main.o | \
bin/unicode.so
	$(GPP) -o $@ obj/test/main.o bin/unicode.so
	bin/tests
