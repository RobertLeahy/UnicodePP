all: unicode


.PHONY: unicode
unicode: bin/unicode.dll


bin/unicode.dll: \
obj/ascii.o \
obj/boundaries.o \
obj/byteordermark.o \
obj/caseconverter.o \
obj/codepoint.o \
obj/collator.o \
obj/comparer.o \
obj/condition.o \
obj/conditions.o \
obj/data/getcollation.o \
obj/data/getdata.o \
obj/defaultlocale.o \
obj/encoding.o \
obj/encodingaction.o \
obj/encodingerror.o \
obj/endianencoding.o \
obj/error.o \
obj/latin1.o \
obj/locale.o \
obj/normalizer.o \
obj/regex.o \
obj/regexalternation.o \
obj/regexanchor.o \
obj/regexatomic.o \
obj/regexcapture.o \
obj/regexcharacterclass.o \
obj/regexcharacterclasses.o \
obj/regexcomment.o \
obj/regexcompiler.o \
obj/regexconditional.o \
obj/regexengine.o \
obj/regexerror.o \
obj/regexgroup.o \
obj/regexiterator.o \
obj/regexlastmatch.o \
obj/regexliteral.o \
obj/regexlookaround.o \
obj/regexmatch.o \
obj/regexmultiple.o \
obj/regexparser.o \
obj/regexpatternelement.o \
obj/regexprivatestate.o \
obj/regexrecurse.o \
obj/regexrecursegroup.o \
obj/regexreplacement.o \
obj/regexreplacementbackreference.o \
obj/regexreplacementcompiler.o \
obj/regexreplacemententirematch.o \
obj/regexreplacementinput.o \
obj/regexreplacementlastgroup.o \
obj/regexreplacementliteral.o \
obj/regexreplacementparser.o \
obj/regexstate.o \
obj/regexunicodeproperties.o \
obj/regexvalidunicode.o \
obj/regexwildcard.o \
obj/regexwordboundary.o \
obj/string.o \
obj/ucs2.o \
obj/utf8.o \
obj/utf16.o \
obj/utf32.o | \
bin
	$(GPP) -shared -o $@ $^