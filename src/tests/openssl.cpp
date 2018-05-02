#include <gtest/gtest.h>

#include "common.hpp"


int main (int argc, char **argv)
{
    int ret;

    ::testing::InitGoogleTest(&argc, argv);
    init_lib({ ashe::LAsheAbility::OPENSSL });

    ret = RUN_ALL_TESTS();

    ashe::deinit_LAshe();
    return ret;
}
