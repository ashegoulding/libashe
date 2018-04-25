#include "__internal.hpp"
#include "__regex.hpp"
#include "libashe/Regex.hpp"


namespace __lashe {

RegexGlobal *regex = nullptr;

RegexGlobal::RegexGlobal()
    : lashe_version(get_regex_string(ashe::RegexClass::LIBASHE_VERSION)),
      fmt_numeral(get_regex_string(ashe::RegexClass::FMT_NUMERAL)),
      fmt_numeral_hex(get_regex_string(ashe::RegexClass::FMT_NUMERAL_HEX)),
      fmt_numeral_oct(get_regex_string(ashe::RegexClass::FMT_NUMERAL_OCT)),
      fmt_numeral_bin(get_regex_string(ashe::RegexClass::FMT_NUMERAL_BIN)),
      fmt_boolean_true(get_regex_string(ashe::RegexClass::FMT_BOOLEAN_TRUE)),
      fmt_boolean_false(get_regex_string(ashe::RegexClass::FMT_BOOLEAN_FALSE)),
      fmt_base64(get_regex_string(ashe::RegexClass::FMT_BASE64)),
      fmt_base64_url(get_regex_string(ashe::RegexClass::FMT_BASE64_URL)),
      uuid_strict(get_regex_string(ashe::RegexClass::FMT_UUID_STRICT)),
      uuid_husk(get_regex_string(ashe::RegexClass::FMT_UUID_HUSK))
{
}

} // namespace __lashe
