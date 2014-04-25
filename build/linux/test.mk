test: \
bin/regex_tests \
bin/tests
	
	
include/catch.hpp:
	wget -P ./include https://raw.github.com/philsquared/Catch/master/single_include/catch.hpp
	
	
bin/regex_tests: $(REGEX_TESTS_DEPENDENCIES) | bin/unicode.so
	$(GPP) -o $@ $^ bin/unicode.so
	bin/regex_tests
	
	
bin/tests: \
include/catch.hpp \
obj/test/main.o | \
bin/unicode.so
	$(GPP) -o $@ obj/test/main.o bin/unicode.so
	bin/tests
