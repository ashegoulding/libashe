#include <gtest/gtest.h>

#include <list>
#include <vector>
#include <set>

#include "../libashe/StringUtils.hpp"
#include "../libashe/helper.hpp"


static void init_lib(const std::vector<ashe::LAsheAbility> abilities)
{
    static const std::vector<int32_t> FLAGS = {
        (int32_t)ashe::LAsheInitFlag::OSCODE, ASHE_HOST_OSCODE};

    ashe::init_LAshe(FLAGS.data(), FLAGS.size(), abilities.data(),
                     abilities.size());
}

TEST(BlankInitDeinitTest, Combined)
{
    static const std::list<std::vector<ashe::LAsheAbility>> list = {
        {}, {ashe::LAsheAbility::OPENSSL}};
    const ashe::LAsheAbility *ptr;
    size_t i, size;

    // TODO: Do permutation
    for (const auto &v : list) {
        std::set<ashe::LAsheAbility> ability_set[2];

        for (const auto &a : v) {
            ability_set[0].insert(a);
        }

        try {
            init_lib(v);

            ashe::get_init_LAshe_abilities(ptr, size);
            for (i = 0; i < size; i += 1) {
                ability_set[1].insert(ptr[i]);
            }

            EXPECT_EQ(ability_set[0], ability_set[1]);

            ashe::deinit_LAshe();
        }
        catch (ashe::Exception &e) {
            std::vector<std::string> arr;

            arr.reserve(v.size());
            for (const auto &a : v) {
                arr.push_back(ashe::tostr(a));
            }

            FAIL() << " exception thrown with: "
                   << ashe::join_str(arr.begin(), arr.end(), ", ") << std::endl
                   << e.what();
            break;
        }
    }
}
