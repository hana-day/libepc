#include "giai.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(GIAITest, Create) {
    GIAI giai;
    Status status;
    {
        std::tie(status, giai) = GIAI::create("0614141", "12345400");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("12345400", giai.getAssetReference());
    }
    // Validation for company prefix
    {
        std::tie(status, giai) = GIAI::create("0614141", "12345400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, giai) = GIAI::create("061414A", "12345400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for asset reference
    {
        // Asset reference must be Application Identifier 21 string.
        std::tie(status, giai) = GIAI::create("0614141", "32a%2Fb");
        ASSERT_EQ(status, Status::kOk);

        // \x20 is not Application Identifier 21 character.
        std::tie(status, giai) = GIAI::create("0614141", "32a%2Fb\x20");
        ASSERT_EQ(status, Status::kInvalidArgument);
    }
}

TEST(GIAITest, CreateFromURI) {
    GIAI giai;
    Status status;
    {
        std::tie(status, giai) = GIAI::createFromURI(
            "urn:epc:id:giai:0614141.12345400");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("12345400", giai.getAssetReference());
    }
    // Check for invalid uri
    {
        std::tie(status, giai) = GIAI::createFromURI(
            "urn:epc:id:giai::0614141.12345400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GIAITest, createFromTagURI) {
    GIAI giai;
    Status status;
    {
        std::tie(status, giai) = GIAI::createFromTagURI(
            "urn:epc:tag:giai-96:3.0614141.5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("5678", giai.getAssetReference());
        ASSERT_EQ(3, giai.getFilterValue());
        ASSERT_EQ(GIAI::Scheme::kGIAI96, giai.getGIAIScheme());
    }
    {
        std::tie(status, giai) = GIAI::createFromTagURI(
            "urn:epc:tag:giai-202:3.0614141.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("32a/b", giai.getAssetReference());
        ASSERT_EQ(3, giai.getFilterValue());
        ASSERT_EQ(GIAI::Scheme::kGIAI202, giai.getGIAIScheme());
    }
    // Check for invaldi tag uri
    {
        std::tie(status, giai) = GIAI::createFromTagURI(
            "urn:epc:tag:giai-202:3.a614141.32a%2Fb");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GIAITest, createFromBinary) {
    GIAI giai;
    Status status;
    // GIAI96
    {
        std::tie(status, giai) = GIAI::createFromBinary(
            "3474257BF40000000000162E");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("5678", giai.getAssetReference());
        ASSERT_EQ(3, giai.getFilterValue());
        ASSERT_EQ(GIAI::Scheme::kGIAI96, giai.getGIAIScheme());
    }
    // GIAI202
    {
        std::tie(status, giai) = GIAI::createFromBinary(
            "3874257BF59B2C2BF10000000000000000000000000000000000");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
        ASSERT_EQ("32a/b", giai.getAssetReference());
        ASSERT_EQ(3, giai.getFilterValue());
        ASSERT_EQ(GIAI::Scheme::kGIAI202, giai.getGIAIScheme());
    }
    // Check for invalid GIAI96
    {
        std::tie(status, giai) = GIAI::createFromBinary(
            "3574257BF40000000000162E");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Check for invalid GIAI202
    {
        std::tie(status, giai) = GIAI::createFromBinary(
            "3974257BF59B2C2BF10000000000000000000000000000000000");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(GIAITest, GetterSetters) {
    GIAI giai;
    Status status;
    std::tie(status, giai) = GIAI::create("0614141", "12345400");
    ASSERT_EQ(Status::kOk, status);
    // Getter for company prefix
    {
        ASSERT_EQ("0614141", giai.getCompanyPrefix());
    }
    // Getter for asset reference
    {
        ASSERT_EQ("12345400", giai.getAssetReference());
    }
    // Getter for filter value
    {
        ASSERT_EQ(0, giai.getFilterValue());
    }
    // Setter for filter value
    {
        ASSERT_EQ(Status::kOk, giai.setFilterValue(7));
        ASSERT_EQ(7, giai.getFilterValue());
        ASSERT_EQ(Status::kOk, giai.setFilterValue(0));
        // Filter value must be 0~7
        ASSERT_EQ(Status::kInvalidArgument, giai.setFilterValue(8));
    }
    // Getter for GIAI scheme
    {
        ASSERT_EQ(GIAI::Scheme::kGIAI96, giai.getGIAIScheme()); // default
    }
    // Setter for GIAI scheme
    {
        ASSERT_EQ(Status::kOk, giai.setGIAIScheme(GIAI::Scheme::kGIAI202));
        ASSERT_EQ(GIAI::Scheme::kGIAI202, giai.getGIAIScheme());
    }
}

TEST(GIAITest, GetURI) {
    GIAI giai;
    Status status;
    std::tie(status, giai) = GIAI::create("0614141", "5678");
    ASSERT_EQ(Status::kOk, status);
    ASSERT_EQ("urn:epc:id:giai:0614141.5678", giai.getURI());
}

TEST(GIAITest, GetTagURI) {
    GIAI giai;
    Status status;
    {
        std::tie(status, giai) = GIAI::create("0614141", "5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, giai.setFilterValue(3));
        ASSERT_EQ("urn:epc:tag:giai-96:3.0614141.5678",
                  giai.getTagURI());
    }
    {
        std::tie(status, giai) = GIAI::create("0614141", "32a/b");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, giai.setFilterValue(3));
        ASSERT_EQ(Status::kOk, giai.setGIAIScheme(GIAI::Scheme::kGIAI202));
        ASSERT_EQ("urn:epc:tag:giai-202:3.0614141.32a%2Fb",
                  giai.getTagURI());
    }
}

TEST(GIAITest, GetBinary) {
    GIAI giai;
    Status status;
    std::string bin;
   // Test GIAI96 encoding
    {
        std::tie(status, giai) = GIAI::createFromTagURI(
            "urn:epc:tag:giai-96:3.0614141.5678");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = giai.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3474257BF40000000000162E", bin);
    }
    // Test GIAI202 encoding
    {
        std::tie(status, giai) = GIAI::createFromTagURI(
            "urn:epc:tag:giai-202:3.0614141.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = giai.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3874257BF59B2C2BF10000000000000000000000000000000000", bin);
    }
    // Test validation for GIAI96
    {
        std::tie(status, giai) = GIAI::create("0614141", "32a/b");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, giai.setGIAIScheme(GIAI::Scheme::kGIAI96));
        std::tie(status, bin) = giai.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);
    }
}
