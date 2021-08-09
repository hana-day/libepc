#include "sgln.h"
#include "status.h"

#include <gtest/gtest.h>

using namespace epc;

TEST(SGLNTest, Create) {
    SGLN sgln;
    Status status;
    {
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("400", sgln.getExtension());
    }
    // Validation for company prefix
    {
        // Company prefix must be padded numbers.
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgln) = SGLN::create("061414A", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 12.
        std::tie(status, sgln) = SGLN::create("061414", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, sgln) = SGLN::create("06141412", "12345", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for location reference
    {
        // location reference must be padded numbers
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "400");
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgln) = SGLN::create("0614141", "1234A", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);

        // Total number of padded numbers must be 12.
        std::tie(status, sgln) = SGLN::create("0614141", "1234", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
        std::tie(status, sgln) = SGLN::create("0614141", "123456", "400");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Validation for serial
    {
        // Serial must be Application Identifier 21 string.
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "32a%2Fb");
        ASSERT_EQ(status, Status::kOk);

        // \x20 is not Application Identifier 21 character.
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "32a%2Fb\x20");
        ASSERT_EQ(status, Status::kInvalidArgument);
    }
}

TEST(SGLNTest, CreateFromURI) {
    SGLN sgln;
    Status status;
    {
        std::tie(status, sgln) = SGLN::createFromURI(
            "urn:epc:id:sgln:0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("5678", sgln.getExtension());
    }
    // Check invalid uri
    {
        std::tie(status, sgln) = SGLN::createFromURI(
            "urn:epc:id:sgln:0614141.123456.5678");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGLNTest, createFromTagURI) {
    SGLN sgln;
    Status status;
    {
        std::tie(status, sgln) = SGLN::createFromTagURI(
            "urn:epc:tag:sgln-96:3.0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("5678", sgln.getExtension());
        ASSERT_EQ(SGLN::Scheme::kSGLN96, sgln.getSGLNScheme());
    }
    {
        std::tie(status, sgln) = SGLN::createFromTagURI(
            "urn:epc:tag:sgln-195:3.0614141.12345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("32a/b", sgln.getExtension());
        ASSERT_EQ(3, sgln.getFilterValue());
        ASSERT_EQ(SGLN::Scheme::kSGLN195, sgln.getSGLNScheme());
    }
    // Check invalid tag uri
    {
        std::tie(status, sgln) = SGLN::createFromTagURI(
            "urn:epc:tag:sgln-195:3.0614141a.12345.32a%2Fb");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGLNTest, createFromBinary) {
    SGLN sgln;
    Status status;
    // SGLN96
    {
        std::tie(status, sgln) = SGLN::createFromBinary(
            "3274257BF46072000000162E");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("5678", sgln.getExtension());
        ASSERT_EQ(3, sgln.getFilterValue());
        ASSERT_EQ(SGLN::Scheme::kSGLN96, sgln.getSGLNScheme());
    }
    // SGLN195
    {
        std::tie(status, sgln) = SGLN::createFromBinary(
            "3974257BF46072CD9615F8800000000000000000000000000");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
        ASSERT_EQ("12345", sgln.getLocationReference());
        ASSERT_EQ("32a/b", sgln.getExtension());
        ASSERT_EQ(3, sgln.getFilterValue());
        ASSERT_EQ(SGLN::Scheme::kSGLN195, sgln.getSGLNScheme());
    }
    // Check for invalid SGLN96
    {
        std::tie(status, sgln) = SGLN::createFromBinary(
            "3274257BF46072000000162");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
    // Check for invalid SGLN195
    {
        std::tie(status, sgln) = SGLN::createFromBinary(
            "3974257BF46072CD9615F880000000000000000000000000");
        ASSERT_EQ(Status::kInvalidArgument, status);
    }
}

TEST(SGLNTest, GetterSetters) {
    SGLN sgln;
    Status status;
    std::tie(status, sgln) = SGLN::create("0614141", "12345", "400");
    ASSERT_EQ(Status::kOk, status);
    // Getter for company prefix
    {
        ASSERT_EQ("0614141", sgln.getCompanyPrefix());
    }
    // Getter for location reference
    {
        ASSERT_EQ("12345", sgln.getLocationReference());
    }
    // Getter for extension
    {
        ASSERT_EQ("400", sgln.getExtension());
    }
    // Getter for filter value
    {
        ASSERT_EQ(0, sgln.getFilterValue());
    }
    // Setter for filter value
    {
        ASSERT_EQ(Status::kOk, sgln.setFilterValue(7));
        ASSERT_EQ(7, sgln.getFilterValue());
        ASSERT_EQ(Status::kOk, sgln.setFilterValue(0));
        // Filter value must be 0~7
        ASSERT_EQ(Status::kInvalidArgument, sgln.setFilterValue(8));
    }
    // Getter for SGLN scheme
    {
        ASSERT_EQ(SGLN::Scheme::kSGLN96, sgln.getSGLNScheme()); // default
    }
    // Setter for SGLN scheme
    {
        ASSERT_EQ(Status::kOk, sgln.setSGLNScheme(SGLN::Scheme::kSGLN195));
        ASSERT_EQ(SGLN::Scheme::kSGLN195, sgln.getSGLNScheme());
    }
}

TEST(SGLNTest, GetURI) {
    SGLN sgln;
    Status status;
    std::tie(status, sgln) = SGLN::create("0614141", "12345", "5678");
    ASSERT_EQ(Status::kOk, status);
    ASSERT_EQ("urn:epc:id:sgln:0614141.12345.5678", sgln.getURI());
}

TEST(SGLNTest, GetTagURI) {
    SGLN sgln;
    Status status;
    {
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "5678");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgln.setFilterValue(3));
        ASSERT_EQ("urn:epc:tag:sgln-96:3.0614141.12345.5678",
                  sgln.getTagURI());
    }
    {
        std::tie(status, sgln) = SGLN::create("0614141", "12345", "32a/b");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgln.setFilterValue(3));
        ASSERT_EQ(Status::kOk, sgln.setSGLNScheme(SGLN::Scheme::kSGLN195));
        ASSERT_EQ("urn:epc:tag:sgln-195:3.0614141.12345.32a%2Fb",
                  sgln.getTagURI());
    }
}

TEST(SGLNTest, GetBinary) {
    SGLN sgln;
    Status status;
    std::string bin;
    // Test SGLN96 encoding
    {
        std::tie(status, sgln) = SGLN::createFromTagURI(
            "urn:epc:tag:sgln-96:3.0614141.12345.5678");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = sgln.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3274257BF46072000000162E", bin);
    }
    // Test SGLN195 encoding
    {
        std::tie(status, sgln) = SGLN::createFromTagURI(
            "urn:epc:tag:sgln-195:3.0614141.12345.32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        std::tie(status, bin) = sgln.getBinary();
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ("3974257BF46072CD9615F8800000000000000000000000000", bin);
    }
    // Test validation for sgln96
    {
        std::tie(status, sgln) = SGLN::create(
            "0614141", "12345", "2199023255551");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgln.setSGLNScheme(SGLN::Scheme::kSGLN96));
        std::tie(status, bin) = sgln.getBinary();
        ASSERT_EQ(Status::kOk, status);

        std::tie(status, sgln) = SGLN::create(
            "0614141", "12345", "2199023255552");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgln.setSGLNScheme(SGLN::Scheme::kSGLN96));
        std::tie(status, bin) = sgln.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);

        std::tie(status, sgln) = SGLN::create(
            "0614141", "12345", "32a%2Fb");
        ASSERT_EQ(Status::kOk, status);
        ASSERT_EQ(Status::kOk, sgln.setSGLNScheme(SGLN::Scheme::kSGLN96));
        std::tie(status, bin) = sgln.getBinary();
        ASSERT_EQ(Status::kInvalidSerial, status);
    }
}
