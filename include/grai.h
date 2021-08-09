#ifndef LIBEPC_EPC_GRAI_H_
#define LIBEPC_EPC_GRAI_H_

#include "epc.h"

#include <utility>

namespace epc {

/**
 * An GRAI(Global Returnable Asset Identifier) class.
 */
class GRAI : public EPC {
public:
    enum class Scheme {
        kGRAI96,
        kGRAI170,
    };

    GRAI() = default;
    /**
     * A static method creating a GRAI instance from company prefix,
     * asset type and serial.
     *
     * @param company_prefix A company prefix
     * @param asset_type An asset type
     * @param serial A serial string
     * @return A pair of a status and a GRAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GRAI> create(const std::string &company_prefix,
                                          const std::string &asset_type,
                                          const std::string &serial);
    /**
     * A static method creating a GRAI instance from EPC URI.
     *
     * @param uri EPC URI
     * @return A pair of a status and a GRAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GRAI> createFromURI(const std::string &uri);
    /**
     * A static method creating a GRAI instance from EPC Tag URI.
     *
     * @param tag_uri EPC Tag URI.
     * @return A pair of a status and a GRAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GRAI> createFromTagURI(const std::string &tag_uri);
    /**
     * A static method creating a GRAI instance from EPC Binary.
     *
     * @param hex EPC Binary in hex string format.
     * @return A pair of a status and a GRAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GRAI> createFromBinary(const std::string &hex);

    std::string getURI() const override;
    std::string getTagURI() const override;
    std::pair<Status, std::string> getBinary() const override;

    /**
     * A method returning company prefix of the GRAI.
     * @return A company prefix in string.
     */
    std::string getCompanyPrefix() const { return company_prefix_; }
    /**
     * A method returning asset type of the GRAI.
     * @return An asset type in string.
     */
    std::string getAssetType() const { return asset_type_; }
    /**
     * A method returning serial of the GRAI.
     * @return A serial string.
     */
    std::string getSerial() const { return serial_; }

    /**
     * A method setting GRAI scheme.
     * @param scheme GRAI encoding scheme
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setGRAIScheme(Scheme scheme);
    /**
     * A method getting GRAI encoding scheme.
     * @return A GRAI encoding scheme (default: GRAI::Scheme::kGRAI96)
     */
    Scheme getGRAIScheme() const { return scheme_; }

private:
    using PartitionTable = struct PartitionTableStruct {
        unsigned int partition_;
        unsigned int company_prefix_bits_;
        unsigned int company_prefix_digits_;
        unsigned int asset_type_bits_;
        unsigned int asset_type_digits_;
    };

    GRAI(const std::string &company_prefix,
         const std::string &asset_type,
         const std::string &serial)
        : company_prefix_(company_prefix),
          asset_type_(asset_type),
          serial_(serial) {}
    Status validate() const;
    static PartitionTable getPartitionTable(
        const std::string &company_prefix);
    static PartitionTable getPartitionTable(
        unsigned int partition);
    Status validateSerialForBinaryCoding() const;

    std::string company_prefix_;
    std::string asset_type_;
    std::string serial_;
    Scheme scheme_ = Scheme::kGRAI96;

    static constexpr int TOTAL_PADDED_NUMBERS = 12;
    static constexpr const char *URI_RE =
        "^urn:epc:id:grai:(\\d+)\\.(\\d+)\\.(.+)$";
    static constexpr const char *TAG_URI_RE =
        "urn:epc:tag:grai-(96|170):(\\d)\\.(\\d+)\\.(\\d+)\\.(.+)";
    static constexpr const char *GRAI96_HEADER = "00110011";
    static constexpr const char *GRAI170_HEADER = "00110111";

    static constexpr unsigned int PARTITION_TABLE_SIZE = 7;
    static PartitionTable PARTITION_TABLE[PARTITION_TABLE_SIZE];

    static constexpr uint64_t MAX_GRAI96_SERIAL = 274877906943LL;
    static constexpr unsigned int MAX_GRAI170_SERIAL_LENGTH = 16;

    static constexpr unsigned int GRAI96_SERIAL_BITS = 38;
    static constexpr unsigned int GRAI170_SERIAL_BITS = 112;
};

}

#endif
