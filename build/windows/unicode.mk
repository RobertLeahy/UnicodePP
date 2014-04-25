unicode: bin/unicode.dll


bin/unicode.dll: $(UNICODE_DEPENDENCIES)
	$(GPP) -shared -o $@ $^