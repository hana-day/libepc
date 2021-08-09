#include "grai.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(GRAITest, Create) {
    GRAI grai;
    Status status;
    {
        std::tie(status, grai) = GRAI::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("400", grai.getSerial());
    }
    // Validation for company prefix
    {
        // Company prefix must be padded numbers.
        std::tie(status, grai) = GRAI::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, grai) = GRAI::create("061414A", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 12.
        std::tie(status, grai) = GRAI::create("061414", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, grai) = GRAI::create("06141412", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for asset type
    {
        // location reference must be padded numbers
        std::tie(status, grai) = GRAI::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, grai) = GRAI::create("0614141", "1234A", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 12.
        std::tie(status, grai) = GRAI::create("0614141", "1234", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, grai) = GRAI::create("0614141", "123456", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for serial
    {
        // Serial must be Application Identifier 21 string.
        std::tie(status, grai) = GRAI::create("0614141", "12345", "32a%2Fb");
        ASSERT_EQ(status, Status::kOk);

        // \x20 is not Application Identifier 21 character.
        std::tie(status, grai) = GRAI::create("0614141", "12345", "32a%2Fb\x20");
        ASSERT_EQ(status, Status::kInvalidArgument);
    }
}

TEST(GRAITest, CreateFromURI) {
    GRAI grai;
    Status status;
    {
        std::tie(status, grai) = GRAI::createFromURI(
            "urn:epc:id:grai:0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("5678", grai.getSerial());
    }
    // Check invalid uri
    {
        std::tie(status, grai) = GRAI::createFromURI(
            "urn:epc:id:grai:0614141.123456.5678");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GRAITest, createFromTagURI) {
    GRAI grai;
    Status status;
    {
        std::tie(status, grai) = GRAI::createFromTagURI(
            "urn:epc:tag:grai-96:3.0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("5678", grai.getSerial());
        ASSERT_EQ(GRAI::Scheme::kGRAI96, grai.getGRAIScheme());
    }
    {
        std::tie(status, grai) = GRAI::createFromTagURI(
            "urn:epc:tag:grai-170:3.0614141.12345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("32a/b", grai.getSerial());
        ASSERT_EQ(3, grai.getFilterValue());
        ASSERT_EQ(GRAI::Scheme::kGRAI170, grai.getGRAIScheme());
    }
    // Check invalid tag uri
    {
        std::tie(status, grai) = GRAI::createFromTagURI(
            "urn:epc:tag:grai-170:3.0614141a.12345.32a%2Fb");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GRAITest, createFromBinary) {
    GRAI grai;
    Status status;
    // GRAI96
    {
        std::tie(status, grai) = GRAI::createFromBinary(
            "3374257BF40C0E400000162E");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("5678", grai.getSerial());
        ASSERT_EQ(3, grai.getFilterValue());
        ASSERT_EQ(GRAI::Scheme::kGRAI96, grai.getGRAIScheme());
    }
    // GRAI170
    {
        std::tie(status, grai) = GRAI::createFromBinary(
            "3774257BF40C0E59B2C2BF100000000000000000000");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
        ASSERT_EQ("12345", grai.getAssetType());
        ASSERT_EQ("32a/b", grai.getSerial());
        ASSERT_EQ(3, grai.getFilterValue());
        ASSERT_EQ(GRAI::Scheme::kGRAI170, grai.getGRAIScheme());
    }
    // Check for invalid GRAI96
    {
        std::tie(status, grai) = GRAI::createFromBinary(
            "3374257BF40C0E400000162");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Check for invalid GRAI170
    {
        std::tie(status, grai) = GRAI::createFromBinary(
            "3774257BF40C0E59B2C2BF10000000000000000000");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GRAITest, GetterSetters) {
    GRAI grai;
    Status status;
    std::tie(status, grai) = GRAI::create("0614141", "12345", "400");
    ASSERT_EQ(Status::kOk, status);
    // Getter for company prefix
    {
        ASSERT_EQ("0614141", grai.getCompanyPrefix());
    }
    // Getter for asset type
    {
        ASSERT_EQ("12345", grai.getAssetType());
    }
    // Getter for serial
    {
        ASSERT_EQ("400", grai.getSerial());
    }
    // Getter for filter value
    {
        ASSERT_EQ(0, grai.getFilterValue());
    }
    // Setter for filter value
    {
        ASSERT_EQ(Status::kOk, grai.setFilterValue(7));
        ASSERT_EQ(7, grai.getFilterValue());
        ASSERT_EQ(Status::kOk, grai.setFilterValue(0));
        // Filter value must be 0~7
        ASSERT_EQ(Status::kInvalidArgument, grai.setFilterValue(8));
    }
    // Getter for GRAI scheme
    {
        ASSERT_EQ(GRAI::Scheme::kGRAI96, grai.getGRAIScheme()); // default
    }
    // Setter for GRAI scheme
    {
        ASSERT_EQ(Status::kOk, grai.setGRAIScheme(GRAI::Scheme::kGRAI170));
        ASSERT_EQ(GRAI::Scheme::kGRAI170, grai.getGRAIScheme());
    }
}

TEST(GRAITest, GetURI) {
    GRAI grai;
    Status status;
    std::tie(status, grai) = GRAI::create("0614141", "12345", "5678");
    ASSERT_EQ(Status::kOk, status);
    ASSERT_EQ("urn:epc:id:grai:0614141.12345.5678", grai.getURI());
}

TEST(GRAITest, GetTagURI) {
    GRAI grai;
    Status status;
    {
        std::tie(status, grai) = GRAI::create("0614141", "12345", "5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, grai.setFilterValue(3));
        ASSERT_EQ("urn:epc:tag:grai-96:3.0614141.12345.5678",
                  grai.getTagURI());
    }
    {
        std::tie(status, grai) = GRAI::create("0614141", "12345", "32a/b");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, grai.setFilterValue(3));
        ASSERT_EQ(Status::kOk, grai.setGRAIScheme(GRAI::Scheme::kGRAI170));
        ASSERT_EQ("urn:epc:tag:grai-170:3.0614141.12345.32a%2Fb",
                  grai.getTagURI());
    }
}

TEST(GRAITest, GetBinary) {
    GRAI grai;
    Status status;
    std::string bin;
    // Test GRAI96 encoding
    {
        std::tie(status, grai) = GRAI::createFromTagURI(
            "urn:epc:tag:grai-96:3.0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = grai.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3374257BF40C0E400000162E", bin);
    }
    // Test GRAI170 encoding
    {
        std::tie(status, grai) = GRAI::createFromTagURI(
            "urn:epc:tag:grai-170:3.0614141.12345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = grai.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3774257BF40C0E59B2C2BF100000000000000000000", bin);
    }
    // Test validation for grai96
    {
        std::tie(status, grai) = GRAI::create(
            "0614141", "12345", "274877906943");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, grai.setGRAIScheme(GRAI::Scheme::kGRAI96));
        std::tie(status, bin) = grai.getBinary();
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, grai) = GRAI::create(
            "0614141", "12345", "274877906944");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, grai.setGRAIScheme(GRAI::Scheme::kGRAI96));
        std::tie(status, bin) = grai.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);

        std::tie(status, grai) = GRAI::create(
            "0614141", "12345", "32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, grai.setGRAIScheme(GRAI::Scheme::kGRAI96));
        std::tie(status, bin) = grai.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);
    }
}
