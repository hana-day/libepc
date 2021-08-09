#ifndef LIBEPC_EPC_SGTIN_H_
#define LIBEPC_EPC_SGTIN_H_

#include "epc.h"
#include "status.h"

#include <regex>

namespace epc {

/**
 * A SGTIN(Serialed Global Trade Item Number) class.
 */
class SGTIN : public EPC {
public:
    enum class Scheme {
        kSGTIN96,
        kSGTIN198
    };
    SGTIN() = default;
    /**
     * A static method creating a SGTIN instance from company prefix,
     * item reference/indicator and serial.
     *
     * @param company_prefix A company prefix
     * @param itemref_indicator Item reference and indicator
     * @param serial A serial string.
     * @return A pair of a status and a SGTIN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGTIN> create(const std::string &company_prefix,
                                           const std::string &itemref_indicator,
                                           const std::string &serial);
    /**
     * A static method creating a SGTIN instance from EPC URI.
     *
     * @param uri EPC URI
     * @return A pair of a status and a SGTIN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGTIN> createFromURI(const std::string &uri);
    /**
     * A static method creating a SGTIN instance from EPC Tag URI.
     *
     * @param tag_uri EPC Tag URI.
     * @return A pair of a status and a SGTIN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGTIN> createFromTagURI(const std::string &tag_uri);
    /**
     * A static method creating a SGTIN instance from EPC Binary.
     *
     * @param hex EPC Binary in hex string format.
     * @return A pair of a status and a SGTIN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGTIN> createFromBinary(const std::string &hex);

    std::string getURI() const override;
    std::string getTagURI() const override;
    std::pair<Status, std::string> getBinary() const override;

    /**
     * A method returning company prefix of the SGTIN.
     * @return A company prefix in string.
     */
    std::string getCompanyPrefix() const { return company_prefix_; }
    /**
     * A method returning item reference and indicator of the SGTIN.
     * @return Item reference and indicator in string.
     */
    std::string getItemReferenceAndIndicator() const { return itemref_indicator_; }
    /**
     * A method returning serial string of the SGTIN.
     * @return A serial string.
     */
    std::string getSerial() const { return serial_; }

    /**
     * A method setting SGTIN encoding scheme.
     * @param scheme SGTIN encoding scheme
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setSGTINScheme(Scheme scheme);
    /**
     * A method getting SGTIN encoding scheme.
     * @return A SGTIN encoding scheme (default: SGTIN::Scheme::kSGTIN96)
     */
    Scheme getSGTINScheme() const { return scheme_; }

private:
    using PartitionTable = struct PartitionTableStruct {
        unsigned int partition_;
        unsigned int company_prefix_bits_;
        unsigned int company_prefix_digits_;
        unsigned int indicator_itemref_bits_;
        unsigned int indicator_itemref_digits_;
    };

    SGTIN(const std::string &company_prefix,
          const std::string &itemref_indicator,
          const std::string &serial)
        : company_prefix_(company_prefix),
          itemref_indicator_(itemref_indicator),
          serial_(serial) {}
    Status validate() const;
    static PartitionTable getPartitionTable(
        const std::string &company_prefix);
    static PartitionTable getPartitionTable(
        unsigned int partition);
    Status validateSerialForBinaryCoding() const;

    std::string company_prefix_;
    std::string itemref_indicator_;
    std::string serial_;

    Scheme scheme_ = Scheme::kSGTIN96;

    static constexpr int TOTAL_PADDED_NUMBERS = 13;
    static constexpr const char *URI_RE =
        "^urn:epc:id:sgtin:(\\d+)\\.(\\d+)\\.(.+)$";
    static constexpr const char *TAG_URI_RE =
        "urn:epc:tag:sgtin-(96|198):(\\d)\\.(\\d+)\\.(\\d+)\\.(.+)";
    static constexpr const char *SGTIN96_HEADER = "00110000";
    static constexpr const char *SGTIN198_HEADER = "00110110";

    static constexpr unsigned int PARTITION_TABLE_SIZE = 7;
    static PartitionTable PARTITION_TABLE[PARTITION_TABLE_SIZE];

    static constexpr int SGTIN96_SERIAL_BITS = 38;
    static constexpr int SGTIN198_SERIAL_BITS = 140;

    static constexpr uint64_t MAX_SGTIN96_SERIAL = 274877906943LL;
    static constexpr int MAX_SGTIN198_SERIAL_LENGTH = 20;
};

}

#endif
