#include "sgln.h"
#include "validation.h"
#include "encode.h"

#include <regex>

namespace epc {
    SGLN::PartitionTable SGLN::PARTITION_TABLE[] = {
        {0, 40, 12, 1, 0},
        {1, 37, 11, 4, 1},
        {2, 34, 10, 7, 2},
        {3, 30, 9, 11, 3},
        {4, 27, 8, 14, 4},
        {5, 24, 7, 17, 5},
        {6, 20, 6, 21, 6}
    };

    std::pair<Status, SGLN> SGLN::create(const std::string &company_prefix,
                                         const std::string &location_ref,
                                         const std::string &extension) {
        SGLN sgln(company_prefix, location_ref, extension);
        Status status = sgln.validate();
        if (status != Status::kOk) {
            return std::make_pair(status, sgln);
        }
        return std::make_pair(Status::kOk, sgln);
    }

    std::pair<Status, SGLN> SGLN::createFromURI(const std::string &uri) {
        SGLN sgln;
        std::smatch m;
        if (std::regex_match(uri, m, std::regex(URI_RE))) {
            return create(m[1].str(), m[2].str(), uri_decode(m[3].str()));
        }
        return std::make_pair(Status::kInvalidArgument, sgln);
    }

    std::pair<Status, SGLN> SGLN::createFromTagURI(const std::string &tag_uri) {
        SGLN sgln;
        Status status;
        std::smatch m;
        if (!std::regex_match(tag_uri, m, std::regex(TAG_URI_RE))) {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        std::tie(status, sgln) = create(
            m[3].str(), m[4].str(), uri_decode(m[5].str()));
        if (status != Status::kOk) {
            return std::make_pair(status, sgln);
        }
        auto scheme_s = m[1].str();
        Scheme scheme;
        if (scheme_s == "96") {
            scheme = Scheme::kSGLN96;
        } else {
            scheme = Scheme::kSGLN195;
        }
        auto filter_s = m[2].str();
        unsigned int filter = std::stoi(filter_s);
        if ((status = sgln.setSGLNScheme(scheme)) != Status::kOk) {
            return std::make_pair(status, sgln);
        }
        if ((status = sgln.setFilterValue(filter)) != Status::kOk) {
            return std::make_pair(status, sgln);
        }
        return std::make_pair(Status::kOk, sgln);
    }


    std::pair<Status, SGLN> SGLN::createFromBinary(const std::string &hex) {
        Status status;
        SGLN sgln;
        std::string bin;
        std::tie(status, bin) = convert_hex_to_bin(hex);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        std::stringstream ss(bin);
        std::string header = read_string(ss, 8);
        Scheme scheme;
        if (header == SGLN96_HEADER) {
            if (bin.length() != 96) {
                return std::make_pair(Status::kInvalidArgument, sgln);
            }
            scheme = Scheme::kSGLN96;
        } else if (header == SGLN195_HEADER) {
            if (bin.length() != 196) {
                return std::make_pair(Status::kInvalidArgument, sgln);
            }
            scheme = Scheme::kSGLN195;
        } else {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        unsigned int filter = decode_integer(read_string(ss, 3));
        unsigned int partition = decode_integer(read_string(ss, 3));
        PartitionTable table = getPartitionTable(partition);
        std::string company_prefix = std::to_string(
            decode_integer(read_string(ss, table.company_prefix_bits_)));
        lpad(company_prefix, table.company_prefix_digits_, '0');
        std::string location_ref = std::to_string(
            decode_integer(read_string(ss, table.location_ref_bits_)));
        lpad(location_ref, table.location_ref_digits_, '0');

        std::string extension;
        if (scheme == Scheme::kSGLN96) {
            extension = std::to_string(
                decode_integer(read_string(ss, SGLN96_EXTENSION_BITS)));
        } else {
            extension = decode_string(read_string(ss, SGLN195_EXTENSION_BITS));
        }
        std::tie(status, sgln) = create(
            company_prefix, location_ref, extension);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        status = sgln.setFilterValue(filter);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        status = sgln.setSGLNScheme(scheme);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgln);
        }
        return std::make_pair(Status::kOk, sgln);
    }

    Status SGLN::validate() const {
        if (!is_padded_numbers(company_prefix_)) return Status::kInvalidArgument;
        if (!is_padded_numbers(location_ref_)) return Status::kInvalidArgument;
        if (company_prefix_.length() + location_ref_.length()
            != TOTAL_PADDED_NUMBERS) {
            return Status::kInvalidArgument;
        }
        if (!is_serial(extension_)) {
            return Status::kInvalidArgument;
        }
        return Status::kOk;
    }

    Status SGLN::setSGLNScheme(Scheme scheme) {
        scheme_ = scheme;
        return Status::kOk;
    }

    std::string SGLN::getURI() const {
        std::stringstream ss;
        ss << "urn:epc:id:sgln:"
           << company_prefix_ << "."
           << location_ref_ << "."
           << uri_encode(extension_);
        return ss.str();
    }

    std::string SGLN::getTagURI() const {
        std::stringstream ss;
        ss << "urn:epc:tag:sgln-"
           << (scheme_ == Scheme::kSGLN96 ? "96" : "195") << ":"
           << getFilterValue() << "."
           << company_prefix_ << "."
           << location_ref_ << "."
           << uri_encode(extension_);
        return ss.str();
    }

    SGLN::PartitionTable SGLN::getPartitionTable(
        const std::string &company_prefix) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (company_prefix.length()
                == PARTITION_TABLE[i].company_prefix_digits_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    SGLN::PartitionTable SGLN::getPartitionTable(
        unsigned int partition) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (partition
                == PARTITION_TABLE[i].partition_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    Status SGLN::validateExtensionForBinaryCoding() const {
        if (scheme_ == Scheme::kSGLN96) {
            if (is_padded_numbers(extension_)
                && std::stoll(extension_, nullptr, 10) <= MAX_SGLN96_EXTENSION) {
                return Status::kOk;
            }
        } else {
            if (is_serial(extension_)
                && extension_.length() <= MAX_SGLN195_EXTENSION_LENGTH) {
                return Status::kOk;
            }
        }
        return Status::kInvalidSerial;
    }

    std::pair<Status, std::string> SGLN::getBinary() const {
        Status status = validateExtensionForBinaryCoding();
        if (status != Status::kOk) return std::make_pair(status, "");
        std::stringstream ss;
        PartitionTable table = getPartitionTable(company_prefix_);
        if (scheme_ == Scheme::kSGLN96) {
            ss << SGLN96_HEADER;
        } else {
            ss << SGLN195_HEADER;
        }
        ss << encode_integer(getFilterValue(), FILTER_VALUE_BITS);
        ss << encode_integer(table.partition_, PARTITION_BITS);
        ss << encode_integer(std::stoi(company_prefix_),
                             table.company_prefix_bits_);
        ss << encode_integer(std::stoi(location_ref_),
                             table.location_ref_bits_);
        if (scheme_ == Scheme::kSGLN96) {
            ss << encode_integer(std::stoll(extension_), SGLN96_EXTENSION_BITS);
        } else {
            ss << encode_string(extension_, SGLN195_EXTENSION_BITS);
            ss << "0";
        }
        std::string hex;
        std::tie(status, hex) = convert_bin_to_hex(ss.str());
        if (status != Status::kOk) return std::make_pair(status, "");
        return std::make_pair(Status::kOk, hex);
    }
}
