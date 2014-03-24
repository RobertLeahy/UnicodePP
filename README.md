Unicode++
=========
C++11 implementation of a Unicode-aware string and related utilities, including utilies for:

- [Case conversion](./include/unicode/caseconverter.hpp) (both full and simple)
- [Case folding](./include/unicode/caseconverter.hpp) (both full and simple)
- [Comparison](./include/unicode/comparer.hpp) (case sensitive or insensitive)
- [Collation](./include/unicode/collator.hpp)
- [Decoding](./include/unicode/encoding.hpp)
- [Encoding](./include/unicode/encoding.hpp)
- [Localization](./include/unicode/locale.hpp)
- [Normalization](./include/unicode/normalizer.hpp)
- [Parsing the Default Unicode Collation Element Table](./include/unicode/ucd.hpp)
- [Parsing the Unicode Character Database](./include/unicode/ucd.hpp)
- [Retrieval/discovery of code point information](./include/unicode/codepoint.hpp)

Dependencies
------------
- C++11 standard library
- [Catch](https://github.com/philsquared/Catch) (for unit tests only)
- [`std::optional`](http://en.cppreference.com/w/cpp/utility/optional)

Compiler Support
----------------
- Clang 3.4 (perhaps earlier, untested)
- GCC 4.8.1, 4.8.2, and 4.9 (4.8.0 not supported due to lack of support for `[[noreturn]]` and ref qualifiers)