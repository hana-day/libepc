#ifndef LIBEPC_EPC_SSCC_H_
#define LIBEPC_EPC_SSCC_H_

#include "epc.h"

#include <regex>
#include <vector>

namespace epc {

/**
 * A SSCC(Serial Shipping Container Code) class.
*/
class SSCC : public EPC {
public:
    enum class Scheme {
        kSSCC96,
    };
    SSCC() = default;
    /**
     * A static method creating a SSCC instance from company prefix
     * and serial reference.
     *
     * @param company_prefix A company prefix
     * @param serial_ref A serial reference
     * @return A pair of a status and a SSCC instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SSCC> create(const std::string &company_prefix,
                                          const std::string &serial_ref);
    /**
     * A static method creating a SSCC instance from EPC URI.
     *
     * @param uri EPC URI
     * @return A pair of a status and a SSCC instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SSCC> createFromURI(const std::string &uri);
    /**
     * A static method creating a SSCC instance from EPC Tag URI.
     *
     * @param tag_uri EPC Tag URI.
     * @return A pair of a status and a SSCC instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SSCC> createFromTagURI(const std::string &tag_uri);
    /**
     * A static method creating a SSCC instance from EPC Binary.
     *
     * @param hex EPC Binary in hex string format.
     * @return A pair of a status and a SSCC instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SSCC> createFromBinary(const std::string &hex);

    std::string getURI() const override;
    std::string getTagURI() const override;
    std::pair<Status, std::string> getBinary() const override;

    /**
     * A method returning company prefix of the SSCC.
     * @return A company prefix in string.
     */
    std::string getCompanyPrefix() const { return company_prefix_; }
    /**
     * A method returning serial reference of the SSCC.
     * @return A serial reference in string.
     */
    std::string getSerialReference() const { return serial_ref_; }

    /**
     * A method setting SSCC encoding scheme.
     * @param scheme SSCC encoding scheme
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setSSCCScheme(Scheme scheme);
    /**
     * A method getting SSCC encoding scheme.
     * @return A SSCC encoding scheme (default: SSCC::Scheme::kSSCC96)
     */
    Scheme getSSCCScheme() const { return scheme_; }

private:
    using PartitionTable = struct PartitionTableStruct {
        unsigned int partition_;
        unsigned int company_prefix_bits_;
        unsigned int company_prefix_digits_;
        unsigned int serial_ref_bits_;
        unsigned int serial_ref_digits_;
    };

    SSCC(const std::string &company_prefix,
         const std::string &serial_ref)
        : company_prefix_(company_prefix), serial_ref_(serial_ref) {}
    Status validate() const;
    static PartitionTable getPartitionTable(
        const std::string &company_prefix);
    static PartitionTable getPartitionTable(
        unsigned int partition);

    std::string company_prefix_;
    std::string serial_ref_;
    Scheme scheme_ = Scheme::kSSCC96;

    static constexpr int TOTAL_PADDED_NUMBERS = 17;
    static constexpr const char *URI_RE =
        "^urn:epc:id:sscc:(\\d+)\\.(\\d+)$";
    static constexpr const char *TAG_URI_RE =
        "^urn:epc:tag:sscc-96:(\\d)\\.(\\d+)\\.(\\d+)$";
    static constexpr const char* SSCC96_HEADER = "00110001";

    static std::vector<PartitionTable> PARTITION_TABLE;
};
}

#endif
