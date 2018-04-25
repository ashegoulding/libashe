#ifndef LASHE_REGEX_HPP_
#define LASHE_REGEX_HPP_
#include "pub.hpp"

#include <regex>
#include <string>


namespace ashe {

enum class RegexClass {
    LIBASHE_VERSION,
    FMT_NUMERAL,
    FMT_NUMERAL_HEX,
    FMT_NUMERAL_OCT,
    FMT_NUMERAL_BIN,
    FMT_BOOLEAN_TRUE,
    FMT_BOOLEAN_FALSE,
    FMT_BASE64,
    FMT_BASE64_URL,
    FMT_UUID_STRICT, // Does not check version digit
    FMT_UUID_HUSK    // Does not check magic digit
};

const char *get_regex_string(const RegexClass v) LASHE_NOEXCEPT;

} // namespace ashe

#endif
