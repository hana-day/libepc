#include "encode.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(EncodeTest, EncodeInteger) {
    ASSERT_EQ("1111011", encode_integer(123, 7));
    ASSERT_EQ("01111011", encode_integer(123, 8));
}

TEST(EncodeTest, DecodeInteger) {
    ASSERT_EQ(123, decode_integer("1111011"));
}

TEST(EncodeTest, EncodeString) {
    ASSERT_EQ("01000011111010", encode_string("!z", 14));
    ASSERT_EQ("0100001111101000", encode_string("!z", 16));
}

TEST(EncodeTest, DecodeString) {
    ASSERT_EQ("!z", decode_string("01000011111010"));
    ASSERT_EQ("!z", decode_string("0100001111101000"));
    ASSERT_EQ("!z", decode_string("010000111110100000000"));
    ASSERT_EQ("", decode_string("111100"));
}

TEST(EncodeTest, ConvertBinToHex) {
    Status status;
    std::string hex;
    std::tie(status, hex) = convert_bin_to_hex("1010011101000111");
    ASSERT_EQ(Status::kOk, status);
    ASSERT_EQ("A747", hex);

    std::tie(status, hex) = convert_bin_to_hex("101001110100011");
    ASSERT_EQ(Status::kInvalidArgument, status);
}

TEST(EncodeTest, ConvertHexToBin) {
    Status status;
    std::string bin;
    std::tie(status, bin) = convert_hex_to_bin("3ABBB8");
    ASSERT_EQ(Status::kOk, status);
    ASSERT_EQ("001110101011101110111000", bin);

    std::tie(status, bin) = convert_hex_to_bin("3ABBBG");
    ASSERT_EQ(Status::kInvalidArgument, status);
}

TEST(EncodeTest, URIEncode) {
    ASSERT_EQ("%22%25%26%2F%3C%3E%3F", uri_encode("\"%&/<>?"));
    ASSERT_EQ("%22%25%26%2F%3C%3E%3Ftest%22%25%26%2F%3C%3E%3F",
              uri_encode("\"%&/<>?test\"%&/<>?"));
}

TEST(EncodeTest, URIDecode) {
    ASSERT_EQ("\"%&/<>?", uri_decode("%22%25%26%2F%3C%3E%3F"));
    ASSERT_EQ("\"%&/<>?test\"%&/<>?",
              uri_decode("%22%25%26%2F%3C%3E%3Ftest%22%25%26%2F%3C%3E%3F"));
}

TEST(EncodeTest, ReadString) {
    std::stringstream ss("0123456789");
    ASSERT_EQ("012", read_string(ss, 3));
    ASSERT_EQ("3456789", read_string(ss, 7));
}

TEST(EncodeTest, LPad) {
    std::string s = "abc";
    lpad(s, 5, '0');
    ASSERT_EQ("00abc", s);
    s = "abc";
    lpad(s, 3, '0');
    ASSERT_EQ("abc", s);
    s = "abc";
    lpad(s, 2, '0');
    ASSERT_EQ("abc", s);
}

TEST(EncodeTest, RPad) {
    std::string s = "abc";
    rpad(s, 5, '0');
    ASSERT_EQ("abc00", s);
    s = "abc";
    rpad(s, 3, '0');
    ASSERT_EQ("abc", s);
    s = "abc";
    rpad(s, 2, '0');
    ASSERT_EQ("abc", s);
}
