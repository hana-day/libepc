#include "sscc.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(SSCCTest, Create) {
    SSCC sscc;
    Status status;
    {
        std::tie(status, sscc) = SSCC::create("0614141", "1234567890");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sscc.getCompanyPrefix());
        ASSERT_EQ("1234567890", sscc.getSerialReference());
    }
    // Validation for company prefix
    {
        std::tie(status, sscc) = SSCC::create("a614141", "1234567890");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 17.
        std::tie(status, sscc) = SSCC::create("06141410", "1234567890");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for serial reference
    {
        std::tie(status, sscc) = SSCC::create("0614141", "a234567890");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 17.
        std::tie(status, sscc) = SSCC::create("0614141", "12345678900");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SSCCTest, CreateFromURI) {
    SSCC sscc;
    Status status;
    {
        std::tie(status, sscc) = SSCC::createFromURI(
            "urn:epc:id:sscc:0614141.1234567890");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sscc.getCompanyPrefix());
        ASSERT_EQ("1234567890", sscc.getSerialReference());
    }
    // Check invalid uri
    {
        std::tie(status, sscc) = SSCC::createFromURI(
            "urn:epc:id:sscc:0614141.1234567890a");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SSCCTest, CreateFromTagURI) {
    SSCC sscc;
    Status status;
    {
        std::tie(status, sscc) = SSCC::createFromTagURI(
            "urn:epc:tag:sscc-96:3.0614141.1234567890");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sscc.getCompanyPrefix());
        ASSERT_EQ("1234567890", sscc.getSerialReference());
        ASSERT_EQ(3, sscc.getFilterValue());
        ASSERT_EQ(SSCC::Scheme::kSSCC96, sscc.getSSCCScheme());
    }
    // Check invalid tag uri
    {
        std::tie(status, sscc) = SSCC::createFromTagURI(
            "urn:epc:tag:sscc-96:3.0614141.1234567890a");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SSCCTest, CreateFromBinary) {
    SSCC sscc;
    Status status;
    {
        std::tie(status, sscc) = SSCC::createFromBinary(
            "3174257BF4499602D2000000");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sscc.getCompanyPrefix());
        ASSERT_EQ("1234567890", sscc.getSerialReference());
        ASSERT_EQ(3, sscc.getFilterValue());
        ASSERT_EQ(SSCC::Scheme::kSSCC96, sscc.getSSCCScheme());
    }
    // Check for invalid SSCC96
    {
        std::tie(status, sscc) = SSCC::createFromBinary(
            "3174257BF4499602D20000000");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SSCCTest, GetURI) {
    SSCC sscc;
    Status status;

    std::tie(status, sscc) = SSCC::create("0614141", "1234567890");
    ASSERT_EQ(status, Status::kOk);
    ASSERT_EQ("urn:epc:id:sscc:0614141.1234567890", sscc.getURI());
}

TEST(SSCCTest, GetTagURI) {
    SSCC sscc;
    Status status;
    {
        std::tie(status, sscc) = SSCC::create("0614141", "1234567890");
        ASSERT_EQ(status, Status::kOk);
        sscc.setFilterValue(3);
        ASSERT_EQ("urn:epc:tag:sscc-96:3.0614141.1234567890",
                  sscc.getTagURI());
    }
}

TEST(SSCCTest, GetterSetters) {
    SSCC sscc;
    Status status;
    std::tie(status, sscc) = SSCC::create("0614141", "1234567890");
    ASSERT_EQ(status, Status::kOk);

    // Getter for company prefix
    {
        ASSERT_EQ("0614141", sscc.getCompanyPrefix());
    }
    // Getter for serial reference
    {
        ASSERT_EQ("1234567890", sscc.getSerialReference());
    }
    // Getter for filter value
    {
        ASSERT_EQ(0, sscc.getFilterValue()); // default
    }
    // Setter for filter value
    {
        sscc.setFilterValue(7);
        ASSERT_EQ(7, sscc.getFilterValue()); // default
        status = sscc.setFilterValue(8);
        // Filter value must be within 0~7.
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Getter for SSCC scheme
    {
        ASSERT_EQ(SSCC::Scheme::kSSCC96, sscc.getSSCCScheme());
    }
}

TEST(SSCCTest, GetBinary) {
    SSCC sscc;
    Status status;
    std::string bin;
    {
        std::tie(status, sscc) = SSCC::createFromTagURI(
            "urn:epc:tag:sscc-96:3.0614141.1234567890");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = sscc.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3174257BF4499602D2000000", bin);
    }
}
