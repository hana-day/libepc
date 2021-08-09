#include "giai.h"
#include "validation.h"
#include "encode.h"

#include <iostream>

namespace epc {
    GIAI::PartitionTable GIAI::GIAI96_PARTITION_TABLE[] = {
        {0, 40, 12, 42, 13},
        {1, 37, 11, 45, 14},
        {2, 34, 10, 48, 15},
        {3, 30, 9, 52, 16},
        {4, 27, 8, 55, 17},
        {5, 24, 7, 58, 18},
        {6, 20, 6, 62, 19},
    };

    GIAI::PartitionTable GIAI::GIAI202_PARTITION_TABLE[] = {
        {0, 40, 12, 148, 18},
        {1, 37, 11, 151, 19},
        {2, 34, 10, 154, 20},
        {3, 30, 9, 158, 21},
        {4, 27, 8, 161, 22},
        {5, 24, 7, 164, 23},
        {6, 20, 6, 168, 24},
    };

    std::pair<Status, GIAI> GIAI::create(const std::string &company_prefix,
                                         const std::string &asset_ref) {
        GIAI giai(company_prefix, asset_ref);
        Status status = giai.validate();
        if (status != Status::kOk) {
            return std::make_pair(status, giai);
        }
        return std::make_pair(Status::kOk, giai);
    }

    Status GIAI::validate() const {
        if (!is_padded_numbers(company_prefix_)) return Status::kInvalidArgument;
        if (!is_serial(asset_ref_)) {
            return Status::kInvalidArgument;
        }
        return Status::kOk;
    }

    std::pair<Status, GIAI> GIAI::createFromURI(const std::string &uri) {
        GIAI giai;
        std::smatch m;
        if (std::regex_match(uri, m, std::regex(URI_RE))) {
            return create(m[1].str(), m[2].str());
        }
        return std::make_pair(Status::kInvalidArgument, giai);
    }

    std::pair<Status, GIAI> GIAI::createFromTagURI(const std::string &tag_uri) {
        GIAI giai;
        Status status;
        std::smatch m;
        if (!std::regex_match(tag_uri, m, std::regex(TAG_URI_RE))) {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        std::tie(status, giai) = create(
            m[3].str(), uri_decode(m[4].str()));
        if (status != Status::kOk) {
            return std::make_pair(status, giai);
        }
        auto scheme_s = m[1].str();
        Scheme scheme;
        if (scheme_s == "96") {
            scheme = Scheme::kGIAI96;
        } else {
            scheme = Scheme::kGIAI202;
        }
        auto filter_s = m[2].str();
        unsigned int filter = std::stoi(filter_s);
        if ((status = giai.setGIAIScheme(scheme)) != Status::kOk) {
            return std::make_pair(status, giai);
        }
        if ((status = giai.setFilterValue(filter)) != Status::kOk) {
            return std::make_pair(status, giai);
        }
        return std::make_pair(Status::kOk, giai);
    }

    GIAI::PartitionTable GIAI::getPartitionTable(
        GIAI::Scheme scheme, const std::string &company_prefix) {
        if (scheme == GIAI::Scheme::kGIAI96) {
            for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
                if (company_prefix.length()
                    == GIAI96_PARTITION_TABLE[i].company_prefix_digits_) {
                    return GIAI96_PARTITION_TABLE[i];
                }
            }
            return GIAI96_PARTITION_TABLE[0];
        } else {
            for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
                if (company_prefix.length()
                    == GIAI202_PARTITION_TABLE[i].company_prefix_digits_) {
                    return GIAI202_PARTITION_TABLE[i];
                }
            }
            return GIAI202_PARTITION_TABLE[0];
        }
    }

    GIAI::PartitionTable GIAI::getPartitionTable(
        GIAI::Scheme scheme, unsigned int partition) {
        if (scheme == GIAI::Scheme::kGIAI202) {
            for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
                if (partition
                    == GIAI96_PARTITION_TABLE[i].partition_) {
                    return GIAI96_PARTITION_TABLE[i];
                }
            }
            return GIAI96_PARTITION_TABLE[0];
        } else {
            for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
                if (partition
                    == GIAI202_PARTITION_TABLE[i].partition_) {
                    return GIAI202_PARTITION_TABLE[i];
                }
            }
            return GIAI202_PARTITION_TABLE[0];
        }
    }

    std::pair<Status, GIAI> GIAI::createFromBinary(const std::string &hex) {
        Status status;
        GIAI giai;
        std::string bin;
        std::tie(status, bin) = convert_hex_to_bin(hex);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        std::stringstream ss(bin);
        std::string header = read_string(ss, 8);
        Scheme scheme;
        if (header == GIAI96_HEADER) {
            if (bin.length() != 96) {
                return std::make_pair(Status::kInvalidArgument, giai);
            }
            scheme = Scheme::kGIAI96;
        } else if (header == GIAI202_HEADER) {
            if (bin.length() != 208) {
                return std::make_pair(Status::kInvalidArgument, giai);
            }
            scheme = Scheme::kGIAI202;
        } else {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        unsigned int filter = decode_integer(read_string(ss, 3));
        unsigned int partition = decode_integer(read_string(ss, 3));
        PartitionTable table = getPartitionTable(scheme, partition);
        std::string company_prefix = std::to_string(
            decode_integer(read_string(ss, table.company_prefix_bits_)));
        lpad(company_prefix, table.company_prefix_digits_, '0');
        std::string asset_ref;
        if (scheme == Scheme::kGIAI96) {
            asset_ref = std::to_string(
                decode_integer(read_string(ss, table.asset_ref_bits_)));
        } else {
            asset_ref = decode_string(read_string(ss, table.asset_ref_bits_));
        }
        std::tie(status, giai) = create(
            company_prefix, asset_ref);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        status = giai.setFilterValue(filter);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        status = giai.setGIAIScheme(scheme);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, giai);
        }
        return std::make_pair(Status::kOk, giai);
    }

    Status GIAI::setGIAIScheme(Scheme scheme) {
        scheme_ = scheme;
        return Status::kOk;
    }

    std::string GIAI::getURI() const {
        std::stringstream ss;
        ss << "urn:epc:id:giai:"
           << company_prefix_ << "."
           << uri_encode(asset_ref_);
        return ss.str();
    }

    std::string GIAI::getTagURI() const {
        std::stringstream ss;
        ss << "urn:epc:tag:giai-"
           << (scheme_ == Scheme::kGIAI96 ? "96" : "202") << ":"
           << getFilterValue() << "."
           << company_prefix_ << "."
           << uri_encode(asset_ref_);
        return ss.str();
    }

    Status GIAI::validateAssetReferenceForBinaryCoding() const {
        if (scheme_ == Scheme::kGIAI96) {
            if (is_padded_numbers(asset_ref_)) {
                return Status::kOk;
            }
        } else {
            if (is_serial(asset_ref_)
                && asset_ref_.length() <= MAX_ASSET_REFERENCE_LENGTH) {
                return Status::kOk;
            }
        }
        return Status::kInvalidSerial;
    }

    std::pair<Status, std::string> GIAI::getBinary() const {
        Status status = validateAssetReferenceForBinaryCoding();
        if (status != Status::kOk) return std::make_pair(status, "");
        std::stringstream ss;
        PartitionTable table = getPartitionTable(scheme_, company_prefix_);
        if (scheme_ == Scheme::kGIAI96) {
            ss << GIAI96_HEADER;
        } else {
            ss << GIAI202_HEADER;
        }
        ss << encode_integer(getFilterValue(), FILTER_VALUE_BITS);
        ss << encode_integer(table.partition_, PARTITION_BITS);
        ss << encode_integer(std::stoi(company_prefix_),
                             table.company_prefix_bits_);
        if (scheme_ == Scheme::kGIAI96) {
            ss << encode_integer(std::stoi(asset_ref_),
                                 table.asset_ref_bits_);
        } else {
            ss << encode_string(asset_ref_, table.asset_ref_bits_);
            ss << "000000";
        }
        std::string hex;
        std::tie(status, hex) = convert_bin_to_hex(ss.str());
        if (status != Status::kOk) return std::make_pair(status, "");
        return std::make_pair(Status::kOk, hex);
    }
 }

