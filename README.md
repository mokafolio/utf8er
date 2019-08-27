utf8er (v0.0.1)
======

Overview
--------

*utf8er* is a lightweight single header c++ library to deal with utf8 encoded strings. It focusses on encoding, decoding and iteration.

Decoding
--------


Encoding
--------


Error Handling
--------

*utf8er* uses error codes for error handling and will never throw an exception itself. (That is, if you compile it with exceptions enabled, and one of your provided string or iterator classes throws exceptions in certain cases, it might still thow).

All functions that validate unicode or utf8 encoded codepoints exist in two flavors, the regular, non validating form and its counterpart that is post-fixed with `_safe` and performs error checking (i.e. `decode` and `decode_safe`). Errors are transmitted using the `error_report` structure and are commonly passed in as the last argument to a function.

