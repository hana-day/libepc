#ifndef LIBEPC_EPC_ENCODE_H_
#define LIBEPC_EPC_ENCODE_H_

#include "status.h"

#include <string>
#include <vector>
#include <map>

namespace epc {
    std::string encode_integer(uint64_t i, unsigned int bit_len);
    uint64_t decode_integer(const std::string &s);
    std::string encode_string(const std::string &s, unsigned int bit_len);
    std::string decode_string(const std::string &s);
    std::pair<Status, std::string> convert_bin_to_hex(const std::string &bin);
    std::pair<Status, std::string> convert_hex_to_bin(const std::string &hex);
    void replace_all(std::string& str, const std::string& from,
                     const std::string& to);
    std::string uri_encode(const std::string &s);
    std::string uri_decode(const std::string &s);
    std::string read_string(std::stringstream &ss, size_t n);
    void lpad(std::string &s, size_t n, char ch);
    void rpad(std::string &s, size_t n, char ch);

    const std::map<std::string, char> BIN_TO_HEX_MAP = {
        {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
        {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
        {"1000", '8'}, {"1001", '9'}, {"1010", 'A'}, {"1011", 'B'},
        {"1100", 'C'}, {"1101", 'D'}, {"1110", 'E'}, {"1111", 'F'},
    };

    const std::map<char, std::string> HEX_TO_BIN_MAP = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
        {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
    };
}

#endif
