#ifndef LIBEPC_EPC_VALIDATION_H_
#define LIBEPC_EPC_VALIDATION_H_

#include <string>

namespace epc {
    bool is_padded_numbers(const std::string &s);
    bool is_serial(const std::string &s);
}

#endif
