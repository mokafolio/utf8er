// license at the bottom

#ifndef UTF8ER_UTF8ER_HPP
#define UTF8ER_UTF8ER_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator> //for std::back_insert_iterator
#include <utility>  //for pair

// helper macro to make things a little more compact.
#define UTF8ER_RETURN_ON_ERR(_exp)                                                                 \
    do                                                                                             \
    {                                                                                              \
        auto code = _exp;                                                                          \
        if (code != error_code::none)                                                              \
        {                                                                                          \
            _out_error = error_report(code);                                                       \
            return not_a_character;                                                                \
        }                                                                                          \
    } while (0)

namespace utf8er
{

namespace _me = utf8er;

// some integer types we want to use.
using uint8_t = std::uint8_t;
using uint32_t = std::uint32_t;
using size_t = std::size_t;

// this codepoint is returned if any of the decode methods fails
static constexpr uint32_t not_a_character = 0xFFFF;

// different error codes used by utf8er
enum class error_code
{
    none = 0,
    bad_leading_byte,
    incomplete_sequence,
    overlong_sequence,
    bad_continuation_byte,
    bad_codepoint
};

// encapsulates an error_code to default construct to error_code::none, and adds some convenience.
// all API functions operate on error_report rather than error_code.
struct error_report;

// template class to pick the output iterator for a string of type T. The existing
// implementation simply picks std::back_insert_iterator. Can be specialized for custom string
// types to support types that might not work with std::back_insert_iterator. check the
// implementation below for details.
template <class T>
struct output_iterator_picker;

// appends the provided unicode _codepoint to the provided _append_string.
template <class T>
typename output_iterator_picker<T>::output_iter append(uint32_t _codepoint, T & _append_string);

// appends the provided unicode _codepoint to the provided _append_string. Performs some
// checking to ensure that _codepoint can be store as utf8. Potential errors will be store in
// _out_error.
template <class T>
typename output_iterator_picker<T>::output_iter append_safe(uint32_t _codepoint,
                                                            T & _append_string,
                                                            error_report & _out_error);

// appends the unicode codepoints in the range from _begin to _end to the provided string.
template <class IT, class STR>
typename output_iterator_picker<STR>::output_iter append_range(IT _begin,
                                                               IT _end,
                                                               STR & _append_string);

// appends the unicode codepoints in the range from _begin to _end to the provided string. Performs
// some checking to ensure that _codepoint can be store as utf8. Potential errors will be store in
// _out_error.
template <class IT, class STR>
typename output_iterator_picker<STR>::output_iter append_range_safe(IT _begin,
                                                                    IT _end,
                                                                    STR & _append_string,
                                                                    error_report & _out_error);

// based on the starting byte of a utf8 byte sequence, returns the number of bytes that it uses.
inline size_t byte_count(uint8_t _starting_byte);

// returns the number of unicode codepoints store in between the utf8 encoded byte iterator
// range _begin to _end.
template <class IT>
size_t count(IT _begin, IT _end);

// returns the number of unicode codepoints stored in the provided utf8 encoded, null terminated
// c string.
inline size_t count(const char * _cstr);

// decodes the utf8 byte sequence starting at _begin and returns the resulting unicode
// codepoint. _out_byte_count optionally stores the number of bytes used be the returned
// codepoint. As no end iterator is provided, the provided utf8 encoded codepoints need to be
// complete (otherwise undefined behavior).
template <class IT>
uint32_t decode(IT _begin, uint8_t * _out_byte_count = nullptr);

// decodes the first utf8 encoded codepoint starting at _begin and returns the resulting unicode
// codepoint. _end is the end of the byte sequence. On error it returns not_a_character and
// saves the error reason in _out_error. _out_byte_count optionally stores the number of bytes
// used be the returned codepoint.
template <class IT>
uint32_t decode_safe(IT _begin,
                     IT _end,
                     error_report & _out_error,
                     uint8_t * _out_byte_count = nullptr);

// decode the first utf8 encoded codepoint starting at c_str. The provided c string is expected to
// be null terminated. See decode_safe above for more info.
inline uint32_t decode_safe(const char * _c_str,
                            error_report & _out_error,
                            uint8_t * _out_byte_count = nullptr);

// decodes all the utf8 encoded codepoints between _begin and _end and appends them to the provided
// output iterator. Returns the next output iterator.
template <class IT, class OIT>
OIT decode_range(IT _begin, IT _end, OIT _output_it);

// decodes all the utf8 encoded codepoints between _begin and _end and appends them to the provided
// output iterator. Stores potential errors in _out_error. Returns the next output iterator.
template <class IT, class OIT>
OIT decode_range_safe(IT _begin, IT _end, OIT _output_it, error_report & _out_error);

// decodes all utf8 encoded codepoints in the provided, null terminated _c_str and appends them to
// the provided _output_it.
template <class OIT>
OIT decode_c_str(const char * _c_str, OIT _output_it);

// decodes all utf8 encoded codepoints in the provided, null terminated _c_str and appends them to
// the provided _output_it. Potential errors are store in _out_error.
template <class OIT>
OIT decode_c_str_safe(const char * _c_str, OIT _output_it, error_report & _out_error);

// Decodes the current codepoint at _it into _out_codepoint and advances to the next utf8
// codepoint and returns the corresponding iterator. _it has to be at the start of a utf8 byte
// sequence.
template <class IT>
IT decode_and_next(IT _it, uint32_t & _out_codepoint);

// Decodes the current codepoint at _it into _out_codepoint and advances to the next utf8
// codepoint and returns the corresponding iterator. _it has to be at tthe start of a utf8 byte
// sequence. Error checking is performed and the results are store in _out_error.
template <class IT>
IT decode_and_next_safe(IT _it, IT _end, uint32_t & _out_codepoint, error_report & _out_error);

template <class IT, class T>
typename output_iterator_picker<T>::output_iter decode_range_and_append(IT _begin,
                                                                        IT _end,
                                                                        T & _append_container);

// Returns the iterator to the start of the previous utf8 encoded codepoint of _it. Decodes the
// codepoint at _it and stores it in _out_codepoint.
template <class IT>
IT decode_and_previous(IT _it, uint32_t & _out_codepoint);

// Returns the iterator to the start of the previous utf8 encoded codepoint of _it. Decodes the
// codepoint at _it and stores it in _out_codepoint. Performs error checking and stores the
// results in _out_error.
template <class IT>
IT decode_and_previous_safe(IT _it, uint32_t & _out_codepoint, error_report & _out_error);

// encodes the provided unicode _codepoint and appends it to _output_it (output iterator).
template <class IT>
IT encode(uint32_t _codepoint, IT _output_it);

// encodes the provided unicode _codepoint and appends it to _output_it (output iterator). Does
// error checking and stores potential errors in _out_error.
template <class IT>
IT encode_safe(uint32_t _codepoint, IT _output_it, error_report & _out_error);

template <class IT, class OIT>
OIT encode_range(IT _begin, IT _end, OIT _output_it);

template <class IT, class OIT>
OIT encode_range_safe(IT _begin, IT _end, OIT _output_it, error_report & _out_error);

// returns an error message for the corresponding code.
inline const char * error_message(error_code _code);

// returns true if the provided character is in the ascii range
inline bool is_ascii(uint8_t _starting_byte);

// validates the provided utf8 encoded byte sequence between _begin and _end and returns
// potential errors with the position that triggered the error.
template <class IT>
std::pair<error_report, IT> validate(IT _begin, IT _end);

inline std::pair<error_report, const char*> validate_c_str(const char * _str);

// advances the provided byte iterator _it to the next utf8 codepoint and returns the
// corresponding iterator. _it has to be at the start of a utf8 byte sequence.
template <class IT>
IT next(IT _it);

// Returns the iterator to the start of the previous utf8 encoded codepoint of _it.
template <class IT>
IT previous(IT _it);


// Implementation follows below:
// ============================================================================================

struct error_report
{
    error_report() : code(error_code::none)
    {
    }

    error_report(error_code _code) : code(_code)
    {
    }

    error_report(const error_report &) = default;
    error_report(error_report &&) = default;
    error_report & operator=(const error_report &) = default;
    error_report & operator=(error_report &&) = default;

    bool operator==(const error_report & _other)
    {
        return code == _other.code;
    }

    bool operator!=(const error_report & _other)
    {
        return code != _other.code;
    }

    explicit operator bool() const
    {
        return code != error_code::none;
    }

    const char * message()
    {
        return error_message(code);
    }

    error_code code;
};

template <class T>
struct output_iterator_picker
{
    // for a spezialization, output_iter and iter function need to be present
    using output_iter = std::back_insert_iterator<T>;
    static output_iter iter(T & _str)
    {
        return std::back_inserter(_str);
    }
};

inline const char * error_message(error_code _code)
{
    switch (_code)
    {
    case error_code::none:
        return "no error";
    case error_code::bad_leading_byte:
        return "bad leading byte";
    case error_code::incomplete_sequence:
        return "incomplete sequence";
    case error_code::overlong_sequence:
        return "overlong sequence";
    case error_code::bad_continuation_byte:
        return "bad continuation byte";
    case error_code::bad_codepoint:
        return "bad code point";
    default:
        return "unknown";
    }
}

inline bool is_ascii(uint8_t _starting_byte)
{
    return (_starting_byte & 0x80) == 0;
}

inline size_t byte_count(uint8_t _starting_byte)
{
    if (is_ascii(_starting_byte))
        return 1;
    else if ((_starting_byte & 0xE0) == 0xC0)
        return 2;
    else if ((_starting_byte & 0xF0) == 0xE0)
        return 3;
    else if ((_starting_byte & 0xF8) == 0xF0)
        return 4;

    // error
    return 0;
}

// this namespace holds helper functions that are not part of the API
namespace detail
{
inline bool _check_surrogate(uint32_t _cp)
{
    return _cp >= 0xD800 && _cp <= 0xDFFF;
}

inline error_code _validate_2_byte_codepoint(uint32_t _cp)
{
    // check if the encoded value is in the valid range (128 - 2047)
    if (_cp < 0x80 || _cp > 0x7FF || _check_surrogate(_cp))
        return error_code::bad_codepoint;
    return error_code::none;
}

inline error_code _validate_3_byte_codepoint(uint32_t _cp)
{
    // check if the encoded value is in the valid range (2048 - 65535)
    if (_cp < 0x800 || _cp > 0xFFFF || _check_surrogate(_cp))
        return error_code::bad_codepoint;
    return error_code::none;
}

inline error_code _validate_4_byte_codepoint(uint32_t _cp)
{
    // check if the encoded value is in the valid range (65536 - 1114111)
    if (_cp < 0x10000 || _cp > 0x10FFFF || _check_surrogate(_cp))
        return error_code::bad_codepoint;
    return error_code::none;
}

inline bool _is_valid_trailing_byte(uint8_t _byte)
{
    return (_byte & 0xC0) == 0x80;
}

inline error_code _validate_2_byte_sequence(uint8_t _a, uint8_t _b)
{
    if (!_is_valid_trailing_byte(_b))
        return error_code::bad_continuation_byte;

    if ((_a & 0xFE) == 0xC0)
        return error_code::overlong_sequence;

    return error_code::none;
}

inline error_code _validate_3_byte_sequence(uint8_t _a, uint8_t _b, uint8_t _c)
{
    if (!_is_valid_trailing_byte(_b) || !_is_valid_trailing_byte(_c))
        return error_code::bad_continuation_byte;

    if ((_a == 0xE0) && ((_b & 0xE0) == 0x80))
        return error_code::overlong_sequence;

    return error_code::none;
}

inline error_code _validate_4_byte_sequence(uint8_t _a, uint8_t _b, uint8_t _c, uint8_t _d)
{
    if (!_is_valid_trailing_byte(_b) || !_is_valid_trailing_byte(_c) ||
        !_is_valid_trailing_byte(_d))
        return error_code::bad_continuation_byte;

    if ((_a == 0xF0) && ((_b & 0xF0) == 0x80))
        return error_code::overlong_sequence;

    return error_code::none;
}

template <class T>
error_code _checked_increment(T & _it, T _end)
{
    if (++_it == _end)
        return error_code::incomplete_sequence;
    return error_code::none;
}

template <class IT>
IT _encode_impl(uint32_t _codepoint, IT _output_it, error_report * _out_error = nullptr)
{
    if (_out_error && _check_surrogate(_codepoint))
    {
        *_out_error = error_code::bad_codepoint;
        return _output_it;
    }
    // needs to be encoded in four bytes
    if (_codepoint >= 0x10000)
    {
        *(_output_it++) = static_cast<uint8_t>(0xF0 | (_codepoint >> 18));
        *(_output_it++) = static_cast<uint8_t>(0x80 | ((_codepoint >> 12) & 0x3F));
        *(_output_it++) = static_cast<uint8_t>(0x80 | ((_codepoint >> 6) & 0x3F));
        *(_output_it++) = static_cast<uint8_t>(0x80 | (_codepoint & 0x3F));
    }
    // needs to be encoded in three bytes
    else if (_codepoint >= 0x800)
    {
        *(_output_it++) = static_cast<uint8_t>(0xE0 | (_codepoint >> 12));
        *(_output_it++) = static_cast<uint8_t>(0x80 | ((_codepoint >> 6) & 0x3F));
        *(_output_it++) = static_cast<uint8_t>(0x80 | ((_codepoint)&0x3F));
    }
    // needs to be encoded in two bytes
    else if (_codepoint >= 0x80)
    {
        *(_output_it++) = static_cast<uint8_t>(0xC0 | (_codepoint >> 6));
        *(_output_it++) = static_cast<uint8_t>(0x80 | ((_codepoint)&0x3F));
    }
    // needs to be encoded in one byte? Most likely.
    else
    {
        *(_output_it++) = static_cast<uint8_t>(_codepoint);
    }

    return _output_it;
}

} // namespace detail

template <class IT>
uint32_t decode(IT _begin, uint8_t * _out_byte_count)
{
    uint8_t a = *_begin;
    size_t bc = byte_count(a);
    if (_out_byte_count)
        *_out_byte_count = bc;

    assert(bc > 0);

    if (bc == 1)
    {
        return static_cast<uint32_t>(a);
    }
    else if (bc == 2)
    {
        uint8_t b = *(++_begin);
        return static_cast<uint32_t>(((a & 0x1F) << 6) | (b & 0x3F));
    }
    else if (bc == 3)
    {
        uint8_t b = *(++_begin);
        uint8_t c = *(++_begin);
        return static_cast<uint32_t>((a & 0xF) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
    }
    else if (bc == 4)
    {
        uint8_t b = *(++_begin);
        uint8_t c = *(++_begin);
        uint8_t d = *(++_begin);
        return static_cast<uint32_t>((a & 0x7) << 18) | ((b & 0x3F) << 12) | ((c & 0x3F) << 6) |
               (d & 0x3F);
    }

    return not_a_character;
}

template <class IT>
uint32_t decode_safe(IT _begin, IT _end, error_report & _out_error, uint8_t * _out_byte_count)
{
    uint8_t a = *_begin;
    size_t bc = byte_count(a);
    if (_out_byte_count)
        *_out_byte_count = bc;

    if (bc == 0)
    {
        _out_error = error_code::bad_leading_byte;
        return not_a_character;
    }
    else if (bc == 1)
    {
        return static_cast<uint32_t>(a);
    }
    else if (bc == 2)
    {
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 2, _end)));
        uint8_t b = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_validate_2_byte_sequence(a, b));
        uint32_t cp = static_cast<uint32_t>(((a & 0x1F) << 6) | (b & 0x3F));
        UTF8ER_RETURN_ON_ERR(detail::_validate_2_byte_codepoint(cp));
        return cp;
    }
    else if (bc == 3)
    {
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 3, _end)));
        uint8_t b = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 2, _end)));
        uint8_t c = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_validate_3_byte_sequence(a, b, c));
        uint32_t cp = static_cast<uint32_t>((a & 0xF) << 12) | ((b & 0x3F) << 6) | (c & 0x3F);
        UTF8ER_RETURN_ON_ERR(detail::_validate_3_byte_codepoint(cp));
        return cp;
    }
    else if (bc == 4)
    {
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 4, _end)));
        uint8_t b = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 3, _end)));
        uint8_t c = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_checked_increment(_begin, std::min(_begin + 2, _end)));
        uint8_t d = *_begin;
        UTF8ER_RETURN_ON_ERR(detail::_validate_4_byte_sequence(a, b, c, d));
        uint32_t cp = static_cast<uint32_t>((a & 0x7) << 18) | ((b & 0x3F) << 12) |
                      ((c & 0x3F) << 6) | (d & 0x3F);
        UTF8ER_RETURN_ON_ERR(detail::_validate_4_byte_codepoint(cp));
        return cp;
    }

    // this should never be reached
    assert(false);
    return not_a_character;
}

inline uint32_t decode_safe(const char * _begin,
                            error_report & _out_error,
                            uint8_t * _out_byte_count)
{
    return _me::decode_safe(_begin, _begin + std::strlen(_begin), _out_error, _out_byte_count);
}

template <class IT, class OIT>
OIT decode_range(IT _begin, IT _end, OIT _output_it)
{
    uint32_t cp;
    while (_begin != _end)
    {
        _begin = _me::decode_and_next(_begin, cp);
        *(_output_it++) = cp;
    }
    return _output_it;
}

template <class IT, class OIT>
OIT decode_range_safe(IT _begin, IT _end, OIT _output_it, error_report & _out_error)
{
    uint32_t cp;
    while (_begin != _end)
    {
        _begin = _me::decode_and_next_safe(_begin, _end, cp, _out_error);
        if (_out_error)
            return _output_it;
        *(_output_it++) = cp;
    }
    return _output_it;
}

template <class OIT>
OIT decode_c_str(const char * _c_str, OIT _output_it)
{
    return _me::decode_range(_c_str, _c_str + std::strlen(_c_str), _output_it);
}

template <class OIT>
OIT decode_c_str_safe(const char * _c_str, OIT _output_it, error_report & _out_error)
{
    return _me::decode_range_safe(_c_str, _c_str + std::strlen(_c_str), _output_it, _out_error);
}

template <class IT>
std::pair<error_report, IT> validate(IT _begin, IT _end)
{
    uint8_t bc;
    error_report err;
    while (_begin != _end)
    {
        decode_safe(_begin, err, &bc);
        if (err)
            return std::make_pair(err, _begin);
        _begin += bc;
    }

    return std::make_pair(error_report(), IT());
}

inline std::pair<error_report, const char*> validate_c_str(const char * _str)
{
    return validate(_str, _str + std::strlen(_str));
}

template <class IT>
IT next(IT _it)
{
    return _it + _me::byte_count(*_it);
}

template <class IT>
IT decode_and_next(IT _it, uint32_t & _out_codepoint)
{
    uint8_t bc;
    _out_codepoint = _me::decode(_it, &bc);
    return _it + bc;
}

template <class IT>
IT decode_and_next_safe(IT _it, IT _end, uint32_t & _out_codepoint, error_report & _out_error)
{
    uint8_t bc;
    _out_codepoint = _me::decode_safe(_it, _end, _out_error, &bc);
    return _it + bc;
}

template <class IT>
IT previous(IT _it)
{
    while (detail::_is_valid_trailing_byte(*(--_it)))
        ;
    return _it;
}

template <class IT>
IT decode_and_previous(IT _it, uint32_t & _out_codepoint)
{
    _out_codepoint = decode(_it);
    while (detail::_is_valid_trailing_byte(*(--_it)))
        ;
    return _it;
}

template <class IT>
IT decode_and_previous_safe(IT _it, uint32_t & _out_codepoint, error_report & _out_error)
{
    _out_codepoint = decode_safe(_it, _out_error);
    while (detail::_is_valid_trailing_byte(*(--_it)))
        return _it;
}

template <class IT>
size_t count(IT _begin, IT _end)
{
    size_t ret = 0;
    while (_begin != _end)
    {
        _begin = _me::next(_begin);
        ++ret;
    }
    return ret;
}

inline size_t count(const char * _cstr)
{
    return _me::count(_cstr, _cstr + std::strlen(_cstr));
}

template <class IT>
IT encode(uint32_t _codepoint, IT _output_it)
{
    return detail::_encode_impl(_codepoint, _output_it);
}

template <class IT>
IT encode_safe(uint32_t _codepoint, IT _output_it, error_report & _out_error)
{
    return detail::_encode_impl(_codepoint, _output_it, &_out_error);
}

template <class IT, class OIT>
OIT encode_range(IT _begin, IT _end, OIT _output_it)
{
    while (_begin != _end)
        *(_output_it++) = _me::encode(*(_begin++), _output_it);
    return _output_it;
}

template <class IT, class OIT>
OIT encode_range_safe(IT _begin, IT _end, OIT _output_it, error_report & _out_error)
{
    while (_begin != _end)
    {
        *_output_it = _me::encode_safe(*(_begin++), _output_it, _out_error);
        if (_out_error)
            return _output_it;
        ++_output_it;
    }
    return _output_it;
}

template <class T>
typename output_iterator_picker<T>::output_iter append(uint32_t _codepoint, T & _append_string)
{
    return _me::encode(_codepoint, output_iterator_picker<T>::iter(_append_string));
}

template <class T>
typename output_iterator_picker<T>::output_iter append_safe(uint32_t _codepoint,
                                                            T & _append_string,
                                                            error_report & _out_error)
{
    return _me::encode_safe(
        _codepoint, output_iterator_picker<T>::iter(_append_string), _out_error);
}

template <class IT, class STR>
typename output_iterator_picker<STR>::output_iter append_range(IT _begin,
                                                               IT _end,
                                                               STR & _append_string)
{
    return _me::encode_range(_begin, _end, output_iterator_picker<STR>::iter(_append_string));
}

template <class IT, class STR>
typename output_iterator_picker<STR>::output_iter append_range_safe(IT _begin,
                                                                    IT _end,
                                                                    STR & _append_string,
                                                                    error_report & _out_error)
{
    return _me::encode_range_safe(
        _begin, _end, output_iterator_picker<STR>::iter(_append_string), _out_error);
}

} // namespace utf8er

#endif // UTF8ER_UTF8ER_HPP
