all: test


.PHONY: test


REGEX_TESTS_DEPENDENCIES:=\
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
obj/test/regex/wildcard.o