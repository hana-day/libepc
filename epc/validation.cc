#include "validation.h"

#include <algorithm>

namespace epc {
    bool is_padded_numbers(const std::string &s) {
        return std::all_of(s.cbegin(), s.cend(), isdigit);
    }

    bool is_serial(const std::string &s) {
        return std::all_of(s.cbegin(), s.cend(),
                           [](char c) {
                               return
                                   (0x21 <= c && c <= 0x22) ||
                                   (0x25 <= c && c <= 0x3f) ||
                                   (0x41 <= c && c <= 0x5a) ||
                                   (c == 0x5a) ||
                                   (c == 0x5f) ||
                                   (0x61 <= c && c <= 0x7a);
                           });
    }
}
