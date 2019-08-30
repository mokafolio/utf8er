#include "../utf8er.hpp"

#define JC_TEST_IMPLEMENTATION
#include "jc_test.h"

#include <string>
#include <vector>

namespace utf8 = utf8er;

//@TODO: dont operate on similar codepoints for all the tests...spice it up a lil :)

const char * str = "I am ascii!";
const char * str2 = "A1äÑÝ€îꜳ𝄢ů𩶘";

TEST(utf8er, validate)
{
    ASSERT_FALSE(utf8::validate(str, str + std::strlen(str)).first);
    ASSERT_FALSE(utf8::validate(str2, str2 + std::strlen(str2)).first);
}

TEST(utf8er, distance)
{
    ASSERT_EQ(utf8::distance(str, str + std::strlen(str)), 11);
    ASSERT_EQ(utf8::distance(str2, str2 + std::strlen(str2)), 11);
    ASSERT_EQ(utf8::distance(str2 + std::strlen(str2), str2), -11);
}

TEST(utf8er, is_ascii)
{
    ASSERT_TRUE(utf8::is_ascii(str[0]));
    ASSERT_FALSE(utf8::is_ascii(str2[5])); // start of €
}

TEST(utf8er, decode)
{
    ASSERT_EQ(utf8::decode("A"), (uint32_t)0x0041);
    ASSERT_EQ(utf8::decode("1"), (uint32_t)0x0031);
    ASSERT_EQ(utf8::decode("ä"), (uint32_t)0x00E4);
    ASSERT_EQ(utf8::decode("Ñ"), (uint32_t)0x00D1);
    ASSERT_EQ(utf8::decode("Ý"), (uint32_t)0x00DD);
    ASSERT_EQ(utf8::decode("€"), (uint32_t)0x20AC);
    ASSERT_EQ(utf8::decode("î"), (uint32_t)0x00EE);
    ASSERT_EQ(utf8::decode("ꜳ"), (uint32_t)0xA733);
    ASSERT_EQ(utf8::decode("𝄢"), (uint32_t)0x1D122);
    ASSERT_EQ(utf8::decode("ů"), (uint32_t)0x016F);
    ASSERT_EQ(utf8::decode("𩶘"), (uint32_t)0x29D98);
}

TEST(utf8er, decode_safe)
{
    utf8::error_report err;
    ASSERT_EQ(utf8::decode_safe("A", err), (uint32_t)0x0041);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("1", err), (uint32_t)0x0031);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("ä", err), (uint32_t)0x00E4);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("Ñ", err), (uint32_t)0x00D1);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("Ý", err), (uint32_t)0x00DD);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("€", err), (uint32_t)0x20AC);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("î", err), (uint32_t)0x00EE);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("ꜳ", err), (uint32_t)0xA733);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("𝄢", err), (uint32_t)0x1D122);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("ů", err), (uint32_t)0x016F);
    ASSERT_FALSE(err);
    ASSERT_EQ(utf8::decode_safe("𩶘", err), (uint32_t)0x29D98);
    ASSERT_FALSE(err);

    const char * overlong0 = "\xc0\xaf";
    const char * overlong1 = "\xe0\x80\xaf";
    const char * overlong2 = "\xf0\x80\x80\xaf";
    const char * bad_leading_byte = "\xf8\x80\x80\x80\xaf";
    const char * bad_cont_byte = "\xc0\xc0";
    const char * incomplete =
        "\xe2\x82"; // first two bytes of euro character with the last one missing

    utf8::decode_safe(overlong0, err);
    ASSERT_EQ(err, utf8::error_code::overlong_sequence);
    err = utf8::error_report();
    utf8::decode_safe(overlong1, err);
    ASSERT_EQ(err, utf8::error_code::overlong_sequence);
    err = utf8::error_report();
    utf8::decode_safe(overlong2, err);
    ASSERT_EQ(err, utf8::error_code::overlong_sequence);
    err = utf8::error_report();
    utf8::decode_safe(bad_leading_byte, err);
    ASSERT_EQ(err, utf8::error_code::bad_leading_byte);
    err = utf8::error_report();
    utf8::decode_safe(bad_cont_byte, err);
    ASSERT_EQ(err, utf8::error_code::bad_continuation_byte);
    err = utf8::error_report();
    utf8::decode_safe(incomplete, err);
    ASSERT_EQ(err, utf8::error_code::incomplete_sequence);
}

TEST(utf8er, append)
{
    std::string str;
    utf8::append(0x0041, str);
    ASSERT_EQ(str, "A");
    utf8::append(0x0031, str);
    ASSERT_EQ(str, "A1");
    utf8::append(0x00E4, str);
    ASSERT_EQ(str, "A1ä");
    utf8::append(0x00D1, str);
    ASSERT_EQ(str, "A1äÑ");
    utf8::append(0x1D122, str);
    ASSERT_EQ(str, "A1äÑ𝄢");
    utf8::append(0x29D98, str);
    ASSERT_EQ(str, "A1äÑ𝄢𩶘");
}

TEST(utf8er, append_safe)
{
    std::string str;
    utf8::error_report err;
    utf8::append_safe(0x0041, str, err);
    ASSERT_EQ(str, "A");
    ASSERT_FALSE(err);
    utf8::append_safe(0x0031, str, err);
    ASSERT_EQ(str, "A1");
    ASSERT_FALSE(err);
    utf8::append_safe(0x00E4, str, err);
    ASSERT_EQ(str, "A1ä");
    ASSERT_FALSE(err);
    utf8::append_safe(0x00D1, str, err);
    ASSERT_EQ(str, "A1äÑ");
    ASSERT_FALSE(err);
    utf8::append_safe(0x1D122, str, err);
    ASSERT_EQ(str, "A1äÑ𝄢");
    ASSERT_FALSE(err);
    utf8::append_safe(0x29D98, str, err);
    ASSERT_EQ(str, "A1äÑ𝄢𩶘");
    ASSERT_FALSE(err);

    utf8::append_safe(0xD800, str, err);
    ASSERT_EQ(str, "A1äÑ𝄢𩶘");
    ASSERT_TRUE(err);
    ASSERT_EQ(err, utf8::error_code::bad_codepoint);
}

TEST(utf8er, append_range)
{
    std::string str;
    uint32_t input[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    utf8::append_range(&input[0], &input[0] + 6, str);
    ASSERT_EQ(str, "A1äÑ𝄢𩶘");
}

TEST(utf8er, next)
{
    std::string str("A1äÑ𝄢𩶘");
    utf8::error_report err;
    uint32_t cp;
    uint32_t idx = 0;
    uint32_t expected[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    for (auto it = str.begin(); it != str.end(); it = utf8::next(it))
    {
        auto e = expected[idx++];
        ASSERT_EQ(e, utf8::decode(it));
        cp = utf8::decode_safe(it, str.end(), err);
        ASSERT_EQ(e, cp);
        ASSERT_FALSE(err);
    }

    auto it = str.begin();
    idx = 0;
    while (it != str.end())
    {
        it = utf8::decode_and_next(it, cp);
        auto e = expected[idx++];
        ASSERT_EQ(e, cp);
    }
}

TEST(utf8er, previous)
{
    std::string str("𩶘𝄢Ñä1A");
    utf8::error_report err;
    uint32_t cp;
    uint32_t idx = 0;
    uint32_t expected[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    for (auto it = str.begin() + str.length() - 1; it >= str.begin(); it = utf8::previous(it))
    {
        auto e = expected[idx++];
        ASSERT_EQ(e, utf8::decode(it));
        cp = utf8::decode_safe(it, str.end(), err);
        ASSERT_EQ(e, cp);
        ASSERT_FALSE(err);
    }

    auto it = str.end() - 1;
    idx = 0;
    while (it >= str.begin())
    {
        it = utf8::decode_and_previous(it, cp);
        auto e = expected[idx++];
        ASSERT_EQ(e, cp);
    }
}

TEST(utf8er, decode_range)
{
    std::string str("A1äÑ𝄢𩶘");
    uint32_t expected[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    std::vector<utf8::uint32_t> out;
    utf8::decode_range(str.begin(), str.end(), std::back_inserter(out));
    ASSERT_EQ(out.size(), (std::size_t)6);
    for (std::size_t i = 0; i < out.size(); ++i)
    {
        ASSERT_EQ(out[i], expected[i]);
    }
}

TEST(utf8er, decode_range_safe)
{
    std::string str("A1äÑ𝄢𩶘");
    uint32_t expected[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    std::vector<utf8::uint32_t> out;
    utf8::error_report err;
    utf8::decode_range_safe(str.begin(), str.end(), std::back_inserter(out), err);
    ASSERT_EQ(out.size(), (std::size_t)6);
    for (std::size_t i = 0; i < out.size(); ++i)
    {
        ASSERT_EQ(out[i], expected[i]);
    }
    ASSERT_FALSE(err);

    out.clear();
    std::string broken("A1äÑ\xf0\x80\x80\xaf𝄢𩶘");
    utf8::decode_range_safe(broken.begin(), broken.end(), std::back_inserter(out), err);
    ASSERT_EQ(err, utf8::error_code::overlong_sequence);
    ASSERT_EQ(out.size(), (std::size_t)4);
}

TEST(utf8er, encode_range)
{
    uint32_t input[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    std::string out;
    utf8::encode_range(&input[0], &input[0] + 6, std::back_inserter(out));
    ASSERT_EQ(utf8::distance(out.begin(), out.end()), 6);
    ASSERT_EQ(out, "A1äÑ𝄢𩶘");
}

TEST(utf8er, encode_range_safe)
{
    uint32_t input[] = { 0x0041, 0x0031, 0x00E4, 0x00D1, 0x1D122, 0x29D98 };
    std::string out;
    utf8::error_report err;
    utf8::encode_range_safe(&input[0], &input[0] + 6, std::back_inserter(out), err);
    ASSERT_EQ(utf8::distance(out.begin(), out.end()), 6);
    ASSERT_EQ(out, "A1äÑ𝄢𩶘");
    ASSERT_FALSE(err);

    uint32_t broken_input[] = { 0x0041,  0x0031, 0xD800 /*bad codepoint */, 0x00E4, 0x00D1,
                                0x1D122, 0x29D98 };

    out.clear();
    utf8::encode_range_safe(&broken_input[0], &broken_input[0] + 7, std::back_inserter(out), err);
    ASSERT_EQ(utf8::distance(out.begin(), out.end()), 2);
    ASSERT_EQ(out, "A1");
    ASSERT_TRUE(err);
    ASSERT_EQ(err, utf8::error_code::bad_codepoint);
}

int main(int argc, char * argv[])
{
    jc_test_init(&argc, argv);
    return jc_test_run_all();
}
