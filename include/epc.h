#ifndef LIBEPC_EPC_EPC_H_
#define LIBEPC_EPC_EPC_H_

#include "status.h"

#include <string>

namespace epc {

/**
 * An EPC(Electronic Product Code) class.
 */
class EPC {
public:
    /**
     * A pure virtual memeber returning EPC URI.
     *
     * @return EPC URI
     */
    virtual std::string getURI() const = 0;

    /**
     * A pure virtual member returning EPC Tag URI.
     *
     * @return EPC Tag URI
     */
    virtual std::string getTagURI() const = 0;
    /**
     * A pure virtual memeber returning EPC binary in hex string.
     *
     * @return A pair of a status and an EPC binary.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    virtual std::pair<Status, std::string> getBinary() const = 0;

    /**
     * Set filter value for the EPC.
     * @param filter value.
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setFilterValue(unsigned int value);
    /**
     * Get filter value for the EPC.
     * @return filter value.
     */
    unsigned int getFilterValue() const { return filter_value_; }

protected:
    static constexpr int FILTER_VALUE_BITS = 3;
    static constexpr int PARTITION_BITS = 3;

private:
    unsigned int filter_value_ = 0;
    static constexpr int MAX_FILTER_VALUE = 7;
};

}

#endif
