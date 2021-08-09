#include "sscc.h"
#include "validation.h"
#include "encode.h"

#include <sstream>

namespace epc {
    std::vector<SSCC::PartitionTable> SSCC::PARTITION_TABLE = {
        {0, 40, 12, 18, 5},
        {1, 37, 11, 21, 6},
        {2, 34, 10, 24, 7},
        {3, 30, 9, 28, 8},
        {4, 27, 8, 31, 9},
        {5, 24, 7, 34, 10},
        {6, 20, 6, 38, 11}
    };

    std::pair<Status, SSCC> SSCC::create(const std::string &company_prefix,
                                         const std::string &serial_ref) {
        SSCC sscc(company_prefix, serial_ref);
        Status status = sscc.validate();
        if (status != Status::kOk) {
            return std::make_pair(status, sscc);
        }
        return std::make_pair(Status::kOk, sscc);
    }

    std::pair<Status, SSCC> SSCC::createFromURI(const std::string &uri) {
        SSCC sscc;
        std::smatch m;
        if (std::regex_match(uri, m, std::regex(URI_RE))) {
            return create(m[1].str(), m[2].str());
        }
        return std::make_pair(Status::kInvalidArgument, sscc);
    }

    std::pair<Status, SSCC> SSCC::createFromTagURI(const std::string &tag_uri) {
        SSCC sscc;
        Status status;
        std::smatch m;
        if (!std::regex_match(tag_uri, m, std::regex(TAG_URI_RE))) {
            return std::make_pair(Status::kInvalidArgument, sscc);
        }
        std::tie(status, sscc) = create(m[2].str(), m[3].str());
        if (status != Status::kOk) {
            return std::make_pair(status, sscc);
        }
        unsigned int filter = std::stoi(m[1].str());
        if ((status = sscc.setFilterValue(filter)) != Status::kOk) {
            return std::make_pair(status, sscc);
        }
        return std::make_pair(Status::kOk, sscc);
    }

    std::pair<Status, SSCC> SSCC::createFromBinary(const std::string &hex) {
        Status status;
        SSCC sscc;
        std::string bin;
        std::tie(status, bin) = convert_hex_to_bin(hex);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sscc);
        }
        std::stringstream ss(bin);
        std::string header = read_string(ss, 8);
        Scheme scheme;
        if (header == SSCC96_HEADER) {
            if (bin.length() != 96) {
                return std::make_pair(Status::kInvalidArgument, sscc);
            }
            scheme = Scheme::kSSCC96;
        } else {
            return std::make_pair(Status::kInvalidArgument, sscc);
        }
        unsigned int filter = decode_integer(read_string(ss, 3));
        unsigned int partition = decode_integer(read_string(ss, 3));
        PartitionTable table = getPartitionTable(partition);
        std::string company_prefix = std::to_string(
            decode_integer(read_string(ss, table.company_prefix_bits_)));
        lpad(company_prefix, table.company_prefix_digits_, '0');
        std::string ext_digti_serial_ref = std::to_string(
            decode_integer(read_string(ss, table.serial_ref_bits_)));
        std::tie(status, sscc) = create(
            company_prefix, ext_digti_serial_ref);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sscc);
        }
        status = sscc.setFilterValue(filter);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sscc);
        }
        return std::make_pair(Status::kOk, sscc);
    }

    Status SSCC::validate() const {
        if (!is_padded_numbers(company_prefix_)) return Status::kInvalidArgument;
        if (!is_padded_numbers(serial_ref_)) return Status::kInvalidArgument;
        if (company_prefix_.length() + serial_ref_.length()
            != TOTAL_PADDED_NUMBERS) {
            return Status::kInvalidArgument;
        }
        return Status::kOk;
    }

    std::string SSCC::getURI() const {
        std::stringstream ss;
        ss << "urn:epc:id:sscc:"
           << company_prefix_ << "."
           << serial_ref_;
        return ss.str();
    }

    std::string SSCC::getTagURI() const {
        std::stringstream ss;
        ss << "urn:epc:tag:sscc-96:"
           << getFilterValue() << "."
           << company_prefix_ << "."
           << serial_ref_;
        return ss.str();
    }

    std::pair<Status, std::string> SSCC::getBinary() const {
        std::stringstream ss;
        ss << SSCC96_HEADER
           << encode_integer(getFilterValue(), FILTER_VALUE_BITS);
        PartitionTable table = getPartitionTable(company_prefix_);
        ss << encode_integer(table.partition_, PARTITION_BITS)
           << encode_integer(
               std::stoll(company_prefix_), table.company_prefix_bits_)
           << encode_integer(
               std::stoll(serial_ref_), table.serial_ref_bits_);
        std::string bin = ss.str();
        rpad(bin, 96, '0');
        std::string hex;
        Status status;
        std::tie(status, hex) = convert_bin_to_hex(bin);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, "");
        }
        return std::make_pair(Status::kOk, hex);
    }

    SSCC::PartitionTable SSCC::getPartitionTable(
        const std::string &company_prefix) {
        for (auto &table : PARTITION_TABLE) {
            if (company_prefix.length()
                == table.company_prefix_digits_) {
                return table;
            }
        }
        return PARTITION_TABLE[0];
    }

    SSCC::PartitionTable SSCC::getPartitionTable(
        unsigned int partition) {
        for (auto &table : PARTITION_TABLE) {
            if (partition
                == table.partition_) {
                return table;
            }
        }
        return PARTITION_TABLE[0];
    }
}
