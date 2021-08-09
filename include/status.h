#ifndef LIBEPC_EPC_STATUS_H_
#define LIBEPC_EPC_STATUS_H_

namespace epc {
    enum class Status {
        kOk,
        kInvalidArgument,
        kInvalidSerial,
    };
}

#endif
