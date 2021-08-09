#include "grai.h"
#include "validation.h"
#include "encode.h"

#include <regex>

namespace epc {
    GRAI::PartitionTable GRAI::PARTITION_TABLE[] = {
        {0, 40, 12, 4, 0},
        {1, 37, 11, 7, 1},
        {2, 34, 10, 10, 2},
        {3, 30, 9, 14, 3},
        {4, 27, 8, 17, 4},
        {5, 24, 7, 20, 5},
        {6, 20, 6, 24, 6},
    };

    std::pair<Status, GRAI> GRAI::create(const std::string &company_prefix,
                                         const std::string &asset_type,
                                         const std::string &serial) {
        GRAI grai(company_prefix, asset_type, serial);
        Status status = grai.validate();
        if (status != Status::kOk) {
            return std::make_pair(status, grai);
        }
        return std::make_pair(Status::kOk, grai);
    }

    std::pair<Status, GRAI> GRAI::createFromURI(const std::string &uri) {
        GRAI grai;
        std::smatch m;
        if (std::regex_match(uri, m, std::regex(URI_RE))) {
            return create(m[1].str(), m[2].str(), uri_decode(m[3].str()));
        }
        return std::make_pair(Status::kInvalidArgument, grai);
    }

    std::pair<Status, GRAI> GRAI::createFromTagURI(const std::string &tag_uri) {
        GRAI grai;
        Status status;
        std::smatch m;
        if (!std::regex_match(tag_uri, m, std::regex(TAG_URI_RE))) {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        std::tie(status, grai) = create(
            m[3].str(), m[4].str(), uri_decode(m[5].str()));
        if (status != Status::kOk) {
            return std::make_pair(status, grai);
        }
        auto scheme_s = m[1].str();
        Scheme scheme;
        if (scheme_s == "96") {
            scheme = Scheme::kGRAI96;
        } else {
            scheme = Scheme::kGRAI170;
        }
        auto filter_s = m[2].str();
        unsigned int filter = std::stoi(filter_s);
        if ((status = grai.setGRAIScheme(scheme)) != Status::kOk) {
            return std::make_pair(status, grai);
        }
        if ((status = grai.setFilterValue(filter)) != Status::kOk) {
            return std::make_pair(status, grai);
        }
        return std::make_pair(Status::kOk, grai);
    }


    std::pair<Status, GRAI> GRAI::createFromBinary(const std::string &hex) {
        Status status;
        GRAI grai;
        std::string bin;
        std::tie(status, bin) = convert_hex_to_bin(hex);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        std::stringstream ss(bin);
        std::string header = read_string(ss, 8);
        Scheme scheme;
        if (header == GRAI96_HEADER) {
            if (bin.length() != 96) {
                return std::make_pair(Status::kInvalidArgument, grai);
            }
            scheme = Scheme::kGRAI96;
        } else if (header == GRAI170_HEADER) {
            if (bin.length() != 172) {
                return std::make_pair(Status::kInvalidArgument, grai);
            }
            scheme = Scheme::kGRAI170;
        } else {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        unsigned int filter = decode_integer(read_string(ss, 3));
        unsigned int partition = decode_integer(read_string(ss, 3));
        PartitionTable table = getPartitionTable(partition);
        std::string company_prefix = std::to_string(
            decode_integer(read_string(ss, table.company_prefix_bits_)));
        lpad(company_prefix, table.company_prefix_digits_, '0');
        std::string asset_type = std::to_string(
            decode_integer(read_string(ss, table.asset_type_bits_)));
        lpad(asset_type, table.asset_type_digits_, '0');

        std::string serial;
        if (scheme == Scheme::kGRAI96) {
            serial = std::to_string(
                decode_integer(read_string(ss, GRAI96_SERIAL_BITS)));
        } else {
            serial = decode_string(read_string(ss, GRAI170_SERIAL_BITS));
        }
        std::tie(status, grai) = create(
            company_prefix, asset_type, serial);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        status = grai.setFilterValue(filter);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        status = grai.setGRAIScheme(scheme);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, grai);
        }
        return std::make_pair(Status::kOk, grai);
    }

    Status GRAI::validate() const {
        if (!is_padded_numbers(company_prefix_)) return Status::kInvalidArgument;
        if (!is_padded_numbers(asset_type_)) return Status::kInvalidArgument;
        if (company_prefix_.length() + asset_type_.length()
            != TOTAL_PADDED_NUMBERS) {
            return Status::kInvalidArgument;
        }
        if (!is_serial(serial_)) {
            return Status::kInvalidArgument;
        }
        return Status::kOk;
    }

    Status GRAI::setGRAIScheme(Scheme scheme) {
        scheme_ = scheme;
        return Status::kOk;
    }

    std::string GRAI::getURI() const {
        std::stringstream ss;
        ss << "urn:epc:id:grai:"
           << company_prefix_ << "."
           << asset_type_ << "."
           << uri_encode(serial_);
        return ss.str();
    }

    std::string GRAI::getTagURI() const {
        std::stringstream ss;
        ss << "urn:epc:tag:grai-"
           << (scheme_ == Scheme::kGRAI96 ? "96" : "170") << ":"
           << getFilterValue() << "."
           << company_prefix_ << "."
           << asset_type_ << "."
           << uri_encode(serial_);
        return ss.str();
    }

    GRAI::PartitionTable GRAI::getPartitionTable(
        const std::string &company_prefix) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (company_prefix.length()
                == PARTITION_TABLE[i].company_prefix_digits_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    GRAI::PartitionTable GRAI::getPartitionTable(
        unsigned int partition) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (partition
                == PARTITION_TABLE[i].partition_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    Status GRAI::validateSerialForBinaryCoding() const {
        if (scheme_ == Scheme::kGRAI96) {
            if (is_padded_numbers(serial_)
                && std::stoll(serial_, nullptr, 10) <= MAX_GRAI96_SERIAL) {
                return Status::kOk;
            }
        } else {
            if (is_serial(serial_)
                && serial_.length() <= MAX_GRAI170_SERIAL_LENGTH) {
                return Status::kOk;
            }
        }
        return Status::kInvalidSerial;
    }

    std::pair<Status, std::string> GRAI::getBinary() const {
        Status status = validateSerialForBinaryCoding();
        if (status != Status::kOk) return std::make_pair(status, "");
        std::stringstream ss;
        PartitionTable table = getPartitionTable(company_prefix_);
        if (scheme_ == Scheme::kGRAI96) {
            ss << GRAI96_HEADER;
        } else {
            ss << GRAI170_HEADER;
        }
        ss << encode_integer(getFilterValue(), FILTER_VALUE_BITS);
        ss << encode_integer(table.partition_, PARTITION_BITS);
        ss << encode_integer(std::stoi(company_prefix_),
                             table.company_prefix_bits_);
        ss << encode_integer(std::stoi(asset_type_),
                             table.asset_type_bits_);
        if (scheme_ == Scheme::kGRAI96) {
            ss << encode_integer(std::stoll(serial_), GRAI96_SERIAL_BITS);
        } else {
            ss << encode_string(serial_, GRAI170_SERIAL_BITS);
            ss << "00";
        }
        std::string hex;
        std::tie(status, hex) = convert_bin_to_hex(ss.str());
        if (status != Status::kOk) return std::make_pair(status, "");
        return std::make_pair(Status::kOk, hex);
    }
}
