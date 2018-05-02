#include "__internal.hpp"
#include "__openssl.hpp"
#include "__regex.hpp"
#include "libashe/Regex.hpp"
#include "libashe/helper.hpp"


namespace ashe {

template <class T>
bool __compare_LAshe_version__(const T a, const LAsheCompOp op, const T b)
{
    switch (op) {
    case LAsheCompOp::EQ:
        return a == b;
    case LAsheCompOp::NE:
        return a != b;
    case LAsheCompOp::GT:
        return a > b;
    case LAsheCompOp::GE:
        return a >= b;
    case LAsheCompOp::LT:
        return a < b;
    case LAsheCompOp::LE:
        return a <= b;
    default:
        throw HelperException("unknown value: 'op'");
    }
    return false;
}

LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheInitFlag v) LASHE_NOEXCEPT
{
    switch (v) {
    case LAsheInitFlag::OSCODE:
        return "LAsheInitFlag::OSCODE";
    case LAsheInitFlag::ALL_ABILITIES:
        return "LAsheInitFlag::ALL_ABILITIES";
    }

    return nullptr;
}

LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheAbility v) LASHE_NOEXCEPT
{
    switch (v) {
    case LAsheAbility::OPENSSL:
        return "LAsheAbility::OPENSSL";
    }
    return nullptr;
}

LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheCompOp v) LASHE_NOEXCEPT
{
    switch (v) {
    case LAsheCompOp::EQ:
        return "LAsheCompOp::EQ";
    case LAsheCompOp::NE:
        return "LAsheCompOp::NE";
    case LAsheCompOp::GT:
        return "LAsheCompOp::GT";
    case LAsheCompOp::GE:
        return "LAsheCompOp::GE";
    case LAsheCompOp::LT:
        return "LAsheCompOp::LT";
    case LAsheCompOp::LE:
        return "LAsheCompOp::LE";
    }
    return nullptr;
}

LASHE_DECL_EXT_NOEXCEPT const char *get_LAshe_version() LASHE_NOEXCEPT
{
#ifdef PACKAGE_VERSION
    return PACKAGE_VERSION; // Autoconf or Visual Studio project properties.
#else
    return nullptr;
#endif
}

LASHE_DECL_EXT void init_LAshe(const /* LAsheInitFlag */ int32_t *flags,
                               const size_t nb_flags,
                               const LAsheAbility *abilities,
                               const size_t nb_abilities)
{
    if (__lashe::global != nullptr) {
        return;
    }

    try {
        const std::set<LAsheAbility> allAbilities = {LAsheAbility::OPENSSL};

        size_t i;
        int32_t os_code = -1;
        bool init_all_abilities = false;
        std::set<LAsheAbility> abilitySet;
        auto ensure_next_flag = [&i, &nb_flags](const size_t more,
                                                const LAsheInitFlag flag) {
            if (i + more >= nb_flags) {
                std::stringstream ss;

                ss << "'" << tostr(flag) << "' needs " << more
                   << " following values.";
                throw HelperException(ss.str().c_str());
            }
        };
        auto has_ability = [&abilitySet](const LAsheAbility ability) -> bool {
            return abilitySet.end() != abilitySet.find(ability);
        };

        for (i = 0; i < nb_flags; i += 1) {
            switch (flags[i]) {
            case (int32_t)LAsheInitFlag::OSCODE:
                ensure_next_flag(1, (LAsheInitFlag)flags[i]);
                os_code = flags[i + 1];
                break;
            case (int32_t)LAsheInitFlag::ALL_ABILITIES:
                ensure_next_flag(1, (LAsheInitFlag)flags[i]);
                init_all_abilities = flags[i + 1] != 0;
                break;
            }
        }

        if (os_code < 0) {
            throw HelperException("missing flag: LAIF_OSCODE");
        }
        else {
            auto throw_incomp = []() {
                throw HelperException(
                    "incompatible OS code. Check build info.");
            };

            if (ASHE_ISOS_POSIX()) {
                if (ASHE_HOST_OSCODE == 0x01) {
                    if (ASHE_HOST_OSCODE != os_code) {
                        // TODO: Warn.
                    }
                }
                else if (ASHE_HOST_OSCODE != os_code) {
                    throw_incomp();
                }
            }
            else if (ASHE_ISOS_WIN()) {
                if (ASHE_HOST_OSCODE > os_code) {
                    throw_incomp();
                }
                else if (ASHE_HOST_OSCODE < os_code) {
                    // TODO: Warn.
                }
            }
        }
        // Parse requested abilities.
        if (init_all_abilities) {
            // Every single ability.
            abilitySet = allAbilities;
        }
        else {
            for (i = 0; i < nb_abilities; i += 1) {
                if (allAbilities.find(abilities[i]) == allAbilities.end()) {
                    throw HelperException("unrecognised 'LAsheAbility'.");
                }
                abilitySet.insert(abilities[i]);
            }
        }

        // Initialise globals.
        __lashe::global = new __lashe::Global;
        __lashe::global->initAbilitySet = abilitySet;
        for (const auto &v : abilitySet) {
            __lashe::global->initAbilityVec.push_back(v);
        }

        __lashe::regex = new __lashe::RegexGlobal;

        // Initialise the modules discrete in order.
        if (has_ability(LAsheAbility::OPENSSL)) {
            __lashe::init_openssl();
        }
    }
    catch (Exception &e) {
        deinit_LAshe();
        throw HelperException("failed to initialise LibAshe.", &e);
    }
}

LASHE_DECL_EXT_NOEXCEPT void deinit_LAshe() LASHE_NOEXCEPT
{
    if (__lashe::global == nullptr) {
        return;
    }

    __lashe::deinit_openssl();
    delete __lashe::regex;
    delete __lashe::global;

    __lashe::regex = nullptr;
    __lashe::global = nullptr;
}

LASHE_DECL_EXT void get_init_LAshe_abilities(const LAsheAbility *&ptr,
                                             size_t &size)
{
    __lashe::ensure_init();
    ptr = __lashe::global->initAbilityVec.data();
    size = __lashe::global->initAbilityVec.size();
}

LASHE_DECL_EXT bool compare_LAshe_version(const char *a, const LAsheCompOp op,
                                          const char *b)
{
    // Assuming each number is less than 2^16
    const std::regex re(get_regex_string(RegexClass::LIBASHE_VERSION));
    auto parse_version = [&re](const char *str) -> uint64_t {
        size_t i;
        uint16_t ret[3];
        std::stringstream ss;
        std::cmatch m;

        if (std::regex_match(str, m, re)) {
            for (i = 0; i < 3; i += 1) {
                ss << m[i + 1];
                ss >> ret[i];
                if (ss.fail()) {
                    throw HelperException("invalid format.");
                }
            }
        }
        else {
            throw HelperException("invalid format.");
        }

        return ((uint64_t)ret[0] << 32) | ((uint64_t)ret[1] << 16) |
               ((uint64_t)ret[2]);
    };
    const auto ver_a = parse_version(a);
    const auto ver_b = parse_version(b);

    return __compare_LAshe_version__(ver_a, op, ver_b);
}

} // namespace ashe
