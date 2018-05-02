#ifndef LASHE_HELPER_HPP_
#define LASHE_HELPER_HPP_
#include "HelperException.hpp"
#include "pub.hpp"


namespace ashe {

enum class LAsheInitFlag : int32_t {
    // Following value being ASHE_HOST_OSCODE of the software.
    OSCODE,
    ALL_ABILITIES
};

// TODO: test
LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheInitFlag v) LASHE_NOEXCEPT;

// TODO: test
LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheAbility v) LASHE_NOEXCEPT;

// TODO: test
LASHE_DECL_EXT_NOEXCEPT const char *tostr(const LAsheCompOp v) LASHE_NOEXCEPT;

// TODO: test
LASHE_DECL_EXT_NOEXCEPT const char *get_LAshe_version() LASHE_NOEXCEPT;

// TODO: test
LASHE_DECL_EXT void init_LAshe(const /* LAsheInitFlag */ int32_t *flags,
                               const size_t nb_flags,
                               const LAsheAbility *abilities,
                               const size_t nb_abilities);

// TODO: test
LASHE_DECL_EXT_NOEXCEPT void deinit_LAshe() LASHE_NOEXCEPT;

// TODO: test
LASHE_DECL_EXT void get_init_LAshe_abilities(const LAsheAbility *&ptr,
                                             size_t &size);

// TODO: test
LASHE_DECL_EXT bool compare_LAshe_version(const char *a, const LAsheCompOp op,
                                          const char *b);

} // namespace ashe

#endif
