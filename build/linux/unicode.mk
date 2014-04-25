unicode: bin/unicode.so


bin/unicode.so: $(UNICODE_DEPENDENCIES)
	$(GPP) -shared -o $@ $^