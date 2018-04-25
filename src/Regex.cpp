#include "libashe/Regex.hpp"


namespace ashe {

const char *get_regex_string(const RegexClass v) LASHE_NOEXCEPT
{
    switch (v) {
    case RegexClass::LIBASHE_VERSION:
        return "([0-9]{1,5})\\.([0-9]{1,5})\\.([0-9]{1,5})";
    case RegexClass::FMT_NUMERAL:
        return "^\\-?[0-9]+$";
    case RegexClass::FMT_NUMERAL_HEX:
        return "^0x[0-9a-fA-F]+$";
    case RegexClass::FMT_NUMERAL_OCT:
        return "^0[0-7]+$";
    case RegexClass::FMT_NUMERAL_BIN:
        return "^0b[01]+$";
    case RegexClass::FMT_BOOLEAN_TRUE:
        return "^(true|\\-?[0-9]*[1-9][0-9]*)$";
    case RegexClass::FMT_BOOLEAN_FALSE:
        return "^(false|(\\-?0+))$";
    case RegexClass::FMT_BASE64:
        return "^[A-Za-z0-9\\+\\/\\s]+={0,2}$";
    case RegexClass::FMT_BASE64_URL:
        return "^[A-Za-z0-9\\-_\\s]+={0,2}$";
    case RegexClass::FMT_UUID_STRICT:
        return "^[0-9a-f]{8}\\-[0-9a-f]{4}\\-[0-9a-f]{4}\\-[8-9a-b][0-9a-f]{3}"
               "\\-[0-9a-f]{12}$";
    case RegexClass::FMT_UUID_HUSK:
        return "^[0-9a-f]{8}\\-[0-9a-f]{4}\\-[0-9a-f]{4}\\-[0-9a-f]{4}\\-[0-9a-"
               "f]{12}$";
    }
    return nullptr;
}

} // namespace ashe
