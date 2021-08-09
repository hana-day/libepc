#include "encode.h"

#include <bitset>
#include <map>
#include <sstream>

namespace epc {
    std::string encode_integer(uint64_t i, unsigned int bit_len) {
        std::string s = std::bitset<64>(i).to_string();
        return s.substr(64-bit_len, 64);
    }

    uint64_t decode_integer(const std::string &s) {
        return std::stoll(s, nullptr, 2);
    }

    std::string encode_string(const std::string &s, unsigned int bit_len) {
        std::stringstream ss;
        for (auto c: s) {
            ss << std::bitset<7>(c).to_string();
        }
        auto binstr = ss.str();
        return binstr + std::string(bit_len-binstr.length(), '0');
    }

    std::string decode_string(const std::string &s) {
        std::stringstream ss;
        for (int i = 0; i < s.length(); i+= 7) {
            if (i+7 > s.length())
                break;
            std::string sub = s.substr(i, 7);
            if (sub == "0000000")
                break;
            ss << static_cast<char>(std::stoi(sub, nullptr, 2));
        }
        return ss.str();
    }

    constexpr char HEX_ARRAY[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F'
    };

    std::pair<Status, std::string> convert_bin_to_hex(const std::string &bin) {
        if (bin.length() % 4 != 0) {
            return std::make_pair(Status::kInvalidArgument, "");
        }
        std::stringstream ss;
        for (size_t i = 0; i < bin.length(); i+=4) {
            auto it = BIN_TO_HEX_MAP.find(bin.substr(i, 4));
            ss << it->second;
        }
        return std::make_pair(Status::kOk, ss.str());
    }

    std::pair<Status, std::string> convert_hex_to_bin(const std::string &hex) {
        std::stringstream ss;
        for (auto c: hex) {
            auto it = HEX_TO_BIN_MAP.find(c);
            if (it == HEX_TO_BIN_MAP.end()) {
                return std::make_pair(Status::kInvalidArgument, "");
            }
            ss << it->second;
        }
        return std::make_pair(Status::kOk, ss.str());
    }

    void replace_all(std::string& str, const std::string& from,
                     const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    std::string uri_encode(const std::string &s) {
        std::string copy = s;
        replace_all(copy, "%", "%25");
        replace_all(copy, "\"", "%22");
        replace_all(copy, "&", "%26");
        replace_all(copy, "/", "%2F");
        replace_all(copy, "<", "%3C");
        replace_all(copy, ">", "%3E");
        replace_all(copy, "?", "%3F");
        return copy;
    }

    std::string uri_decode(const std::string &s) {
        std::string copy = s;
        replace_all(copy, "%22", "\"");
        replace_all(copy, "%26", "&");
        replace_all(copy, "%2F", "/");
        replace_all(copy, "%3C", "<");
        replace_all(copy, "%3E", ">");
        replace_all(copy, "%3F", "?");
        replace_all(copy, "%25", "%");
        return copy;
    }

    std::string read_string(std::stringstream &ss, size_t n) {
        std::string str(n, '\0');
        ss.read(&str[0], n);
        return str;
    }

    void lpad(std::string &s, size_t n, char ch) {
        if(n > s.length()) {
            s.insert(0, n-s.length(), ch);
        }
    }

    void rpad(std::string &s, size_t n, char ch) {
        if (n > s.length()) {
            s.insert(s.length(), n-s.length(), ch);
        }
    }
}
