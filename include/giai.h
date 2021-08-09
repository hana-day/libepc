#ifndef LIBEPC_EPC_GIAI_H_
#define LIBEPC_EPC_GIAI_H_

#include "epc.h"

#include <utility>
#include <regex>

namespace epc {


/**
 * A GIAI(Global Individual Asset Identifier) class.
 */
class GIAI : public EPC {
public:
    enum class Scheme {
        kGIAI96,
        kGIAI202,
    };

    GIAI() = default;
    /**
     * A static method creating a GIAI instance from company prefix
     * and asset reference.
     *
     * @param company_prefix A company prefix
     * @param asset_ref An asset reference
     * @return A pair of a status and a GIAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GIAI> create(const std::string &company_prefix,
                                          const std::string &asset_ref);
    /**
     * A static method creating a GIAI instance from EPC URI.
     *
     * @param uri EPC URI
     * @return A pair of a status and a GIAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GIAI> createFromURI(const std::string &uri);
    /**
     * A static method creating a GIAI instance from EPC Tag URI.
     *
     * @param tag_uri EPC Tag URI.
     * @return A pair of a status and a GIAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GIAI> createFromTagURI(const std::string &tag_uri);
    /**
     * A static method creating a GIAI instance from EPC Binary.
     *
     * @param hex EPC Binary in hex string format.
     * @return A pair of a status and a GIAI instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, GIAI> createFromBinary(const std::string &hex);

    std::string getURI() const override;
    std::string getTagURI() const override;
    std::pair<Status, std::string> getBinary() const override;

    /**
     * A method returning company prefix of the GIAI.
     * @return A company prefix in string.
     */
    std::string getCompanyPrefix() const { return company_prefix_; }
    /**
     * A method returning asset reference of the GIAI.
     * @return An asset reference in string.
     */
    std::string getAssetReference() const { return asset_ref_; }

    /**
     * A method setting GIAI encoding scheme.
     * @param scheme GIAI encoding scheme
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setGIAIScheme(Scheme scheme);
    /**
     * A method getting GIAI encoding scheme.
     * @return A GIAI encoding scheme (default: GIAI::Scheme::kGIAI96)
     */
    Scheme getGIAIScheme() const { return scheme_; }

private:
    using PartitionTable = struct PartitionTableStruct {
        unsigned int partition_;
        unsigned int company_prefix_bits_;
        unsigned int company_prefix_digits_;
        unsigned int asset_ref_bits_;
        unsigned int asset_ref_digits_;
    };

    GIAI(const std::string &company_prefix,
         const std::string &asset_ref)
        : company_prefix_(company_prefix),
          asset_ref_(asset_ref) {}
    Status validate() const;
    static PartitionTable getPartitionTable(
        Scheme scheme, const std::string &company_prefix);
    static PartitionTable getPartitionTable(
        Scheme scheme, unsigned int partition);
    Status validateAssetReferenceForBinaryCoding() const;

    std::string company_prefix_;
    std::string asset_ref_;
    Scheme scheme_ = Scheme::kGIAI96;

    static constexpr const char *URI_RE =
        "^urn:epc:id:giai:(\\d+)\\.(.+)$";
    static constexpr const char *TAG_URI_RE =
        "urn:epc:tag:giai-(96|202):(\\d)\\.(\\d+)\\.(.+)";
    static constexpr const char *GIAI96_HEADER = "00110100";
    static constexpr const char *GIAI202_HEADER = "00111000";

    static constexpr unsigned int PARTITION_TABLE_SIZE = 7;
    static PartitionTable GIAI96_PARTITION_TABLE[PARTITION_TABLE_SIZE];
    static PartitionTable GIAI202_PARTITION_TABLE[PARTITION_TABLE_SIZE];

    static constexpr unsigned int MAX_ASSET_REFERENCE_LENGTH = 24;
};

}

#endif
