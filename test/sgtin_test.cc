#include "sgtin.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(SGTINTest, Create) {
    SGTIN sgtin;
    Status status;
    {
        std::tie(status, sgtin) = SGTIN::create("0614141", "112345", "400");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("112345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("400", sgtin.getSerial());
    }
    // Validation for company prefix
    {
        // Company prefix must be padded numbers.
        std::tie(status, sgtin) = SGTIN::create("0614141", "112345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgtin) = SGTIN::create("A614141", "112345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 13.
        std::tie(status, sgtin) = SGTIN::create("061414", "112345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, sgtin) = SGTIN::create("06141412", "112345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for item reference and indicator
    {
        // ItemReference and Indicator must be padded numbers.
        std::tie(status, sgtin) = SGTIN::create("0614141", "112345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgtin) = SGTIN::create("0614141", "A12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 13.
        std::tie(status, sgtin) = SGTIN::create("061414", "112345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, sgtin) = SGTIN::create("06141412", "112345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for serial
    {
        // Serial must be Application Identifier 21 string.
        std::tie(status, sgtin) = SGTIN::create("0614141", "712345", "32a%2Fb");
        ASSERT_EQ(status, Status::kOk);

        // \x20 is not Application Identifier 21 character.
        std::tie(status, sgtin) = SGTIN::create("0614141", "712345", "32a%2Fb\x20");
        ASSERT_EQ(status, Status::kInvalidArgument);
    }
}

TEST(SGTINTest, CreateFromURI) {
    SGTIN sgtin;
    Status status;
    {
        std::tie(status, sgtin) = SGTIN::createFromURI(
            "urn:epc:id:sgtin:0614141.812345.6789");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("812345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("6789", sgtin.getSerial());
    }
    // Check invalid uri
    {
        std::tie(status, sgtin) = SGTIN::createFromURI(
            "urn:epc:id:sgtin::0614141.812345.6789");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGTINTest, createFromTagURI) {
    SGTIN sgtin;
    Status status;
    {
        std::tie(status, sgtin) = SGTIN::createFromTagURI(
            "urn:epc:tag:sgtin-96:3.0614141.812345.6789");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("812345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("6789", sgtin.getSerial());
        ASSERT_EQ(3, sgtin.getFilterValue());
        ASSERT_EQ(SGTIN::Scheme::kSGTIN96, sgtin.getSGTINScheme());
    }
    {
        std::tie(status, sgtin) = SGTIN::createFromTagURI(
            "urn:epc:tag:sgtin-198:3.0614141.712345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("712345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("32a/b", sgtin.getSerial());
        ASSERT_EQ(3, sgtin.getFilterValue());
        ASSERT_EQ(SGTIN::Scheme::kSGTIN198, sgtin.getSGTINScheme());
    }
    // Check invalid tag uri
    {
        std::tie(status, sgtin) = SGTIN::createFromTagURI(
            "urn::epc:tag:sgtin-198:3.0614141.712345.32a%2Fb");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGTINTest, createFromBinary) {
    SGTIN sgtin;
    Status status;
    // SGTIN96
    {
        std::tie(status, sgtin) = SGTIN::createFromBinary(
            "3074257BF7194E4000001A85");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("812345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("6789", sgtin.getSerial());
        ASSERT_EQ(3, sgtin.getFilterValue());
        ASSERT_EQ(SGTIN::Scheme::kSGTIN96, sgtin.getSGTINScheme());
    }
    // SGTIN198
    {
        std::tie(status, sgtin) = SGTIN::createFromBinary(
            "3674257BF6B7A659B2C2BF100000000000000000000000000000");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
        ASSERT_EQ("712345", sgtin.getItemReferenceAndIndicator());
        ASSERT_EQ("32a/b", sgtin.getSerial());
        ASSERT_EQ(3, sgtin.getFilterValue());
        ASSERT_EQ(SGTIN::Scheme::kSGTIN198, sgtin.getSGTINScheme());
    }
    // Check for invalid SGTIN96
    {
        std::tie(status, sgtin) = SGTIN::createFromBinary(
            "3074257BF7194E4000001A8");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Check for invalid SGTIN198
    {
        std::tie(status, sgtin) = SGTIN::createFromBinary(
            "3674257BF6B7A659B2C2BF10000000000000000000000000000");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGTINTest, GetterSetters) {
    SGTIN sgtin;
    Status status;
    std::tie(status, sgtin) = SGTIN::create("0614141", "112345", "400");
    ASSERT_EQ(Status::kOk, status);

    // Getter for company prefix
    {
        ASSERT_EQ("0614141", sgtin.getCompanyPrefix());
    }
    // Getter for item reference and indicator
    {
        ASSERT_EQ("112345", sgtin.getItemReferenceAndIndicator());
    }
    // Getter for serial
    {
        ASSERT_EQ("400", sgtin.getSerial());
    }
    // Getter for filter value
    {
        ASSERT_EQ(0, sgtin.getFilterValue()); // default

    }
    // Setter for filter value
    {
        ASSERT_EQ(Status::kOk, sgtin.setFilterValue(7));
        ASSERT_EQ(7, sgtin.getFilterValue());
        // Filter value must be 0~7
        ASSERT_EQ(Status::kInvalidArgument, sgtin.setFilterValue(8));
    }
    // Getter for SGTIN scheme
    {
        ASSERT_EQ(SGTIN::Scheme::kSGTIN96, sgtin.getSGTINScheme()); // default
    }
    // Setter for SGTIN scheme
    {
        ASSERT_EQ(Status::kOk, sgtin.setSGTINScheme(SGTIN::Scheme::kSGTIN198));
        ASSERT_EQ(SGTIN::Scheme::kSGTIN198, sgtin.getSGTINScheme());
    }
}

TEST(SGTINTest, GetURI) {
    SGTIN sgtin;
    Status status;

    std::tie(status, sgtin) = SGTIN::create("0614141", "812345", "6789");
    ASSERT_EQ(status, Status::kOk);

    ASSERT_EQ("urn:epc:id:sgtin:0614141.812345.6789", sgtin.getURI());
}


TEST(SGTINTest, GetTagURI) {
    SGTIN sgtin;
    Status status;

    {
        std::tie(status, sgtin) = SGTIN::create("0614141", "812345", "6789");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgtin.setFilterValue(3));
        ASSERT_EQ("urn:epc:tag:sgtin-96:3.0614141.812345.6789",
                  sgtin.getTagURI());
    }
    {
        std::tie(status, sgtin) = SGTIN::create("0614141", "712345", "32a/b");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgtin.setFilterValue(3));
        ASSERT_EQ(Status::kOk, sgtin.setSGTINScheme(SGTIN::Scheme::kSGTIN198));
        ASSERT_EQ("urn:epc:tag:sgtin-198:3.0614141.712345.32a%2Fb", sgtin.getTagURI());
    }
}

TEST(SGTINTest, GetBinary) {
    SGTIN sgtin;
    Status status;
    std::string bin;

    // Test SGTIN96 encoding
    {
        std::tie(status, sgtin) = SGTIN::createFromTagURI(
            "urn:epc:tag:sgtin-96:3.0614141.812345.6789");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = sgtin.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3074257BF7194E4000001A85", bin);
    }

    // Test SGTIN198 encoding
    {
        std::tie(status, sgtin) = SGTIN::createFromTagURI(
            "urn:epc:tag:sgtin-198:3.0614141.712345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = sgtin.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3674257BF6B7A659B2C2BF100000000000000000000000000000", bin);
    }

    // Test validation for sgtin96
    {
        std::tie(status, sgtin) = SGTIN::create(
            "0614141", "712345", "274877906943");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgtin.setSGTINScheme(SGTIN::Scheme::kSGTIN96));
        std::tie(status, bin) = sgtin.getBinary();
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgtin) = SGTIN::create(
            "0614141", "712345", "274877906944");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgtin.setSGTINScheme(SGTIN::Scheme::kSGTIN96));
        std::tie(status, bin) = sgtin.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);

        std::tie(status, sgtin) = SGTIN::create("0614141", "712345", "32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgtin.setSGTINScheme(SGTIN::Scheme::kSGTIN96));
        std::tie(status, bin) = sgtin.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);
    }
}
