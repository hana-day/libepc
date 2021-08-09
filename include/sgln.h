#ifndef LIBEPC_EPC_SGLN_H_
#define LIBEPC_EPC_SGLN_H_

#include "epc.h"

#include <utility>

namespace epc {

/**
 * A SGLN(Global Location Number) class.
 */
class SGLN : public EPC {
public:
    enum class Scheme {
        kSGLN96,
        kSGLN195,
    };

    SGLN() = default;
    /**
     * A static method creating a SGLN instance from company prefix,
     * location reference and extension.
     *
     * @param company_prefix A company prefix
     * @param location_ref A location reference
     * @param extension An extension
     * @return A pair of a status and a SGLN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGLN> create(const std::string &company_prefix,
                                          const std::string &location_ref,
                                          const std::string &extension);
    /**
     * A static method creating a SGLN instance from EPC URI.
     *
     * @param uri EPC URI
     * @return A pair of a status and a SGLN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGLN> createFromURI(const std::string &uri);
    /**
     * A static method creating a SGLN instance from EPC Tag URI.
     *
     * @param tag_uri EPC Tag URI.
     * @return A pair of a status and a SGLN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGLN> createFromTagURI(const std::string &tag_uri);
    /**
     * A static method creating a SGLN instance from EPC Binary.
     *
     * @param hex EPC Binary in hex string format.
     * @return A pair of a status and a SGLN instance.
     * The status is Status::kOk on normal completion or the error factor
     * on error.
     */
    static std::pair<Status, SGLN> createFromBinary(const std::string &hex);

    std::string getURI() const override;
    std::string getTagURI() const override;
    std::pair<Status, std::string> getBinary() const override;

    /**
     * A method returning company prefix of the SGLN.
     * @return A company prefix in string.
     */
    std::string getCompanyPrefix() const { return company_prefix_; }
    /**
     * A method returning asset reference of the SGLN.
     * @return A location reference in string.
     */
    std::string getLocationReference() const { return location_ref_; }
    /**
     * A method returning extension of the SGLN.
     * @return An extension in string.
     */
    std::string getExtension() const { return extension_; }

    /**
     * A method setting SGLN encoding scheme.
     * @param scheme SGLN encoding scheme
     * @return Status::kOk on normal completion or the error factor on error.
     */
    Status setSGLNScheme(Scheme scheme);
    /**
     * A method getting SGLN encoding scheme.
     * @return A SGLN encoding scheme (default: SGLN::Scheme::kSGLN96)
     */
    Scheme getSGLNScheme() const { return scheme_; }

private:
    using PartitionTable = struct PartitionTableStruct {
        unsigned int partition_;
        unsigned int company_prefix_bits_;
        unsigned int company_prefix_digits_;
        unsigned int location_ref_bits_;
        unsigned int location_ref_digits_;
    };

    SGLN(const std::string &company_prefix,
         const std::string &location_ref,
         const std::string &extension)
        : company_prefix_(company_prefix),
          location_ref_(location_ref),
          extension_(extension) {}
    Status validate() const;
    static PartitionTable getPartitionTable(
        const std::string &company_prefix);
    static PartitionTable getPartitionTable(
        unsigned int partition);
    Status validateExtensionForBinaryCoding() const;

    std::string company_prefix_;
    std::string location_ref_;
    std::string extension_;
    Scheme scheme_ = Scheme::kSGLN96;

    static constexpr int TOTAL_PADDED_NUMBERS = 12;
    static constexpr const char *URI_RE =
        "^urn:epc:id:sgln:(\\d+)\\.(\\d+)\\.(.+)$";
    static constexpr const char *TAG_URI_RE =
        "urn:epc:tag:sgln-(96|195):(\\d)\\.(\\d+)\\.(\\d+)\\.(.+)";
    static constexpr const char *SGLN96_HEADER = "00110010";
    static constexpr const char *SGLN195_HEADER = "00111001";

    static constexpr unsigned int PARTITION_TABLE_SIZE = 7;
    static PartitionTable PARTITION_TABLE[PARTITION_TABLE_SIZE];

    static constexpr uint64_t MAX_SGLN96_EXTENSION = 2199023255551LL;
    static constexpr unsigned int MAX_SGLN195_EXTENSION_LENGTH = 20;

    static constexpr unsigned int SGLN96_EXTENSION_BITS = 41;
    static constexpr unsigned int SGLN195_EXTENSION_BITS = 140;
};

}

#endif
