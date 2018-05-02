#include <vector>

#include "../libashe/helper.hpp"


void init_lib(const std::vector<ashe::LAsheAbility> abilities)
{
    static const std::vector<int32_t> FLAGS = {
        (int32_t)ashe::LAsheInitFlag::OSCODE, ASHE_HOST_OSCODE};

    ashe::init_LAshe(FLAGS.data(), FLAGS.size(), abilities.data(),
                     abilities.size());
}
