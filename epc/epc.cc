#include "epc.h"

namespace epc {
    Status EPC::setFilterValue(unsigned int value) {
        if (value > MAX_FILTER_VALUE) return Status::kInvalidArgument;
        filter_value_ = value;
        return Status::kOk;
    }

}
