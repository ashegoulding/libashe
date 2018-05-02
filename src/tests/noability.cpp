#include <gtest/gtest.h>

#include "../libashe/LAsheException.hpp"
#include "../libashe/Base64Decoder.hpp"
#include "../libashe/Base64Encoder.hpp"
#include "../libashe/MessageDigest.hpp"
#include "../libashe/UUID.hpp"
#include "common.hpp"


TEST(NoAbility, OPENSSL)
{
    EXPECT_THROW({ ashe::Base64Decoder().open(ashe::Base64Type::PLAIN); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::Base64Decoder().open(ashe::Base64Type::URL); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::Base64Encoder().open(ashe::Base64Type::PLAIN); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::Base64Encoder().open(ashe::Base64Type::URL); }, ashe::LAsheException);

    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::MD5); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::SHA1); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::SHA224); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::SHA256); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::SHA384); }, ashe::LAsheException);
    EXPECT_THROW({ ashe::MessageDigest().open(ashe::HashAlgorithm::SHA512); }, ashe::LAsheException);

    EXPECT_THROW({ ashe::MersenneTwisterUUIDEngine()(); }, ashe::LAsheException);
}

int main (int argc, char **argv)
{
    int ret;

    ::testing::InitGoogleTest(&argc, argv);
    init_lib({});

    ret = RUN_ALL_TESTS();

    ashe::deinit_LAshe();
    return ret;
}
