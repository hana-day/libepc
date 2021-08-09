#include "sgtin.h"
#include "validation.h"
#include "encode.h"

#include <sstream>
#include <iostream>

namespace epc {
    SGTIN::PartitionTable SGTIN::PARTITION_TABLE[] = {
        {0, 40, 12, 4, 1},
        {1, 37, 11, 7, 2},
        {2, 34, 10, 10, 3},
        {3, 30, 9, 14, 4},
        {4, 27, 8, 17, 5},
        {5, 24, 7, 20, 6},
        {6, 20, 6, 24, 7}
    };

    std::pair<Status, SGTIN> SGTIN::create(const std::string &company_prefix,
                                           const std::string &itemref_indicator,
                                           const std::string &serial) {
        SGTIN sgtin(company_prefix, itemref_indicator, serial);
        Status status = sgtin.validate();
        if (status != Status::kOk) {
            return std::make_pair(status, sgtin);
        }
        return std::make_pair(Status::kOk, sgtin);
    }

    std::pair<Status, SGTIN> SGTIN::createFromURI(const std::string &uri) {
        SGTIN sgtin;
        std::smatch m;
        if (std::regex_match(uri, m, std::regex(URI_RE))) {
            return create(m[1].str(), m[2].str(), uri_decode(m[3].str()));
        }
        return std::make_pair(Status::kInvalidArgument, sgtin);
    }

    std::pair<Status, SGTIN> SGTIN::createFromTagURI(const std::string &tag_uri) {
        SGTIN sgtin;
        Status status;
        std::smatch m;
        if (!std::regex_match(tag_uri, m, std::regex(TAG_URI_RE))) {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        } else {
            std::tie(status, sgtin) = create(
                m[3].str(), m[4].str(), uri_decode(m[5].str()));
            if (status != Status::kOk) {
                return std::make_pair(status, sgtin);
            }
            auto scheme_s = m[1].str();
            auto filter_s = m[2].str();
            Scheme scheme;
            if (scheme_s == "96") {
                scheme = Scheme::kSGTIN96;
            } else {
                scheme = Scheme::kSGTIN198;
            }
            unsigned int filter = std::stoi(filter_s);

            if ((status = sgtin.setSGTINScheme(scheme)) != Status::kOk) {
                return std::make_pair(status, sgtin);
            }
            if ((status = sgtin.setFilterValue(filter)) != Status::kOk) {
                return std::make_pair(status, sgtin);
            }
            return std::make_pair(Status::kOk, sgtin);
        }
    }

    std::pair<Status, SGTIN> SGTIN::createFromBinary(const std::string &hex) {
        Status status;
        SGTIN sgtin;
        std::string bin;
        std::tie(status, bin) = convert_hex_to_bin(hex);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        }
        std::stringstream ss(bin);
        std::string header = read_string(ss, 8);
        Scheme scheme;
        if (header == SGTIN96_HEADER) {
            if (bin.length() != 96) {
                return std::make_pair(Status::kInvalidArgument, sgtin);
            }
            scheme = Scheme::kSGTIN96;
        } else if (header == SGTIN198_HEADER) {
            if (bin.length() != 208) {
                return std::make_pair(Status::kInvalidArgument, sgtin);
            }
            scheme = Scheme::kSGTIN198;
        } else {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        }
        unsigned int filter = decode_integer(read_string(ss, 3));
        unsigned int partition = decode_integer(read_string(ss, 3));
        PartitionTable table = getPartitionTable(partition);
        std::string company_prefix = std::to_string(
            decode_integer(read_string(ss, table.company_prefix_bits_)));
        lpad(company_prefix, table.company_prefix_digits_, '0');
        std::string itemref_indicator = std::to_string(
            decode_integer(read_string(ss, table.indicator_itemref_bits_)));
        lpad(itemref_indicator, table.indicator_itemref_digits_, '0');

        std::string serial;
        if (scheme == Scheme::kSGTIN96) {
            serial = std::to_string(
                decode_integer(read_string(ss, 38)));
        } else {
            serial = decode_string(read_string(ss, 140));
        }
        std::tie(status, sgtin) = create(
            company_prefix, itemref_indicator, serial);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        }
        status = sgtin.setFilterValue(filter);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        }
        status = sgtin.setSGTINScheme(scheme);
        if (status != Status::kOk) {
            return std::make_pair(Status::kInvalidArgument, sgtin);
        }
        return std::make_pair(Status::kOk, sgtin);
    }

    Status SGTIN::validate() const {
        if (!is_padded_numbers(company_prefix_)) return Status::kInvalidArgument;
        if (!is_padded_numbers(itemref_indicator_)) return Status::kInvalidArgument;
        if (company_prefix_.length() + itemref_indicator_.length()
            != TOTAL_PADDED_NUMBERS) {
            return Status::kInvalidArgument;
        }
        if (!is_serial(serial_))
            return Status::kInvalidArgument;
        return Status::kOk;
    }

    Status SGTIN::validateSerialForBinaryCoding() const {
        if (scheme_ == Scheme::kSGTIN96) {
            if (is_padded_numbers(serial_)
                && std::stoll(serial_, nullptr, 10) <= MAX_SGTIN96_SERIAL) {
                return Status::kOk;
            }
        } else {
            if (is_serial(serial_)
                && serial_.length() <= MAX_SGTIN198_SERIAL_LENGTH) {
                return Status::kOk;
            }
        }
        return Status::kInvalidSerial;
    }

    Status SGTIN::setSGTINScheme(Scheme scheme) {
        scheme_ = scheme;
        return Status::kOk;
    }

    std::string SGTIN::getURI() const {
        std::stringstream ss;
        ss << "urn:epc:id:sgtin:"
           << company_prefix_ << "."
           << itemref_indicator_ << "."
           << uri_encode(serial_);
        return ss.str();
    }

    std::string SGTIN::getTagURI() const {
        std::stringstream ss;
        ss << "urn:epc:tag:"
           << (scheme_ == Scheme::kSGTIN96 ? "sgtin-96" : "sgtin-198") << ":"
           << getFilterValue() << "."
           << company_prefix_ << "."
           << itemref_indicator_ << "."
           << uri_encode(serial_);
        return ss.str();
    };

    SGTIN::PartitionTable SGTIN::getPartitionTable(
        const std::string &company_prefix) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (company_prefix.length()
                == PARTITION_TABLE[i].company_prefix_digits_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    SGTIN::PartitionTable SGTIN::getPartitionTable(
        unsigned int partition) {
        for (int i = 0; i < PARTITION_TABLE_SIZE; i++) {
            if (partition
                == PARTITION_TABLE[i].partition_) {
                return PARTITION_TABLE[i];
            }
        }
        return PARTITION_TABLE[0];
    }

    std::pair<Status, std::string> SGTIN::getBinary() const {
        Status status = validateSerialForBinaryCoding();
        if (status != Status::kOk) return std::make_pair(status, "");

        std::stringstream ss;
        PartitionTable table = getPartitionTable(company_prefix_);
        if (scheme_ == Scheme::kSGTIN96) {
            ss << SGTIN96_HEADER;
        } else {
            ss << SGTIN198_HEADER;
        }
        ss << encode_integer(getFilterValue(), FILTER_VALUE_BITS);
        ss << encode_integer(table.partition_, PARTITION_BITS);
        ss << encode_integer(std::stoi(company_prefix_),
                             table.company_prefix_bits_);
        ss << encode_integer(std::stoi(itemref_indicator_),
                             table.indicator_itemref_bits_);
        if (scheme_ == Scheme::kSGTIN96) {
            ss << encode_integer(std::stoll(serial_), SGTIN96_SERIAL_BITS);
        } else {
            ss << encode_string(serial_, SGTIN198_SERIAL_BITS);
            ss << "0000000000";
        }
        std::string hex;
        std::tie(status, hex) = convert_bin_to_hex(ss.str());
        if (status != Status::kOk) return std::make_pair(status, "");
        return std::make_pair(Status::kOk, hex);
    };
}
