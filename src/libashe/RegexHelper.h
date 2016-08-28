#ifndef LASHE_REGEXHELPER_H_
#define LASHE_REGEXHELPER_H_

namespace ashe
{

#define LASHE_REGEX_LIBASHE_VERSION			"^(\\s+)?[0-9]{1,5}\\.[0-9]{1,5}\\.[0-9]{1,5}(.*)?$"
#define LASHE_REGEX_FORMAT_NUMERAL			"^(\\s+)?\\-?[0-9]+(\\s+)?$"
// Case insensitive.
#define LASHE_REGEX_FORMAT_BOOLEAN_TRUE		"^(\\s+)?(false|(\\-?0+))(\\s+)?$"
// Case insensitive.
#define LASHE_REGEX_FORMAT_BOOLEAN_FALSE	"^(\\s+)?(true|\\-?[0-9]*[1-9][0-9]*)(\\s+)?$"
// Case insensitive.
#define LASHE_REGEX_FORMAT_BASE64			"^[A-Za-z0-9\\+\\/\\s]+={0,2}(\\s+)?$"
// Case insensitive.
#define LASHE_REGEX_FORMAT_BASE64URL		"^[A-Za-z0-9\\-_\\s]+={0,2}(\\s+)?$"

}

#endif
