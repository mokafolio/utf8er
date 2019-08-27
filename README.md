utf8er (v0.0.1)
======

Overview
--------

*utf8er* is a lightweight single header c++ library to deal with utf8 encoded strings. It focusses on encoding, decoding and iteration.

Decoding
--------

To decode a single utf8 encoded codepoint, use the `decode` or `decode_safe` function, i.e.:

```
std::string str = u8"你好世界"
utf8::uint32_t cp = utf8::decode(str.begin());
//cp now holds the unicode value for the first chinese character.
```

To decode a utf8 encoded range of unicode codepoints, use the `decode_range` or `decode_range_save` functions, i.e.:
```
std::string str = u8"你好世界"
std::vector<utf8::uint32_t> output;
utf8::error_report err;
// decode_range* expects an output iterator to write the resulting codepoints to.
// In this case we simply use a back_inserter to append to an std::vector.
utf8::decode_range_safe(str.begin(), str.end(), std::back_inserter(output), err); 
if(err)
{
	printf("Invalid utf8: %s\n", err.message());
	//...exit, throw an exception, whatever suits you :)
}

//do something with the unicode codepoints stored in output
```

Encoding
--------

To utf8 encode a single unicode codepoint, use the `encode` or `encode_safe` function. Both of those functions append the resulting byte sequences to the provided output iterator:

```
std::string output;
utf8::encode(0x00D1, std::back_inserter(output)); // Ñ codepoint
```

To encode a range of codepoints, use `encode_range` or `encode_range_safe`:

```
std::vector<utf8::uint32_t> cps = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
std::string output;
utf8::encode_range(cps.begin(), cps.end(), std::back_inserter(output));
printf("The resulting utf8 string is %s\n", output.c_str());
```

For convenience, *utf8er* provides `append` and `append_safe` functions that append the encoded byte sequence to the provided string, i.e.:

```
std::string output;
utf8::append(0x00D1, output); // Ñ codepoint
printf("The utf8 encoded string is %s\n", output.c_str());
```

Similarly to the `encode` functions, all the `append` functions exist in ranged variants, too (i.e. `append_range` and `append_range_safe`).

By default the type of the string/container passed to all the `append` flavors can be any type that works with `std::back_inserter`. If your type does not conform to the standard, you can create a `output_iterator_picker` template spezialization to create an output iterator that works for you.



Error Handling
--------

*utf8er* uses error codes for error handling and will never throw an exception itself. (That is, if you compile it with exceptions enabled, and one of your provided string or iterator classes throws exceptions in certain cases, it might still thow).

All functions that validate unicode or utf8 encoded codepoints exist in two flavors, the regular, non validating form and its counterpart that is post-fixed with `_safe` and performs error checking (i.e. `decode` and `decode_safe`). Errors are transmitted using the `error_report` structure and are commonly passed in as the last argument to a function.

