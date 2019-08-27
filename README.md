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
utf8::uint32_t cp = utf8::decode_range_safe(str.begin(), str.end(), err);
if(err)
{
	printf("Invalid utf8: %s\n", err.message());
	//...exit, throw an exception, whatever suits you :)
}

//do something with the unicode codepoints stored in output
```


Encoding
--------


Error Handling
--------

*utf8er* uses error codes for error handling and will never throw an exception itself. (That is, if you compile it with exceptions enabled, and one of your provided string or iterator classes throws exceptions in certain cases, it might still thow).

All functions that validate unicode or utf8 encoded codepoints exist in two flavors, the regular, non validating form and its counterpart that is post-fixed with `_safe` and performs error checking (i.e. `decode` and `decode_safe`). Errors are transmitted using the `error_report` structure and are commonly passed in as the last argument to a function.

