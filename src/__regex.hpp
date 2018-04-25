#ifndef __LASHE_REGEX_HPP_
#define __LASHE_REGEX_HPP_
#include "libashe/pub.hpp"
#include <regex>


namespace __lashe {

struct RegexGlobal {
    std::regex lashe_version;
    std::regex fmt_numeral;
    std::regex fmt_numeral_hex;
    std::regex fmt_numeral_oct;
    std::regex fmt_numeral_bin;
    std::regex fmt_boolean_true;
    std::regex fmt_boolean_false;
    std::regex fmt_base64;
    std::regex fmt_base64_url;
    std::regex uuid_strict;
    std::regex uuid_husk;

    RegexGlobal();
};

extern RegexGlobal *regex;

} // namespace __lashe

#endif
