#include <iostream>
#include <iomanip>

#include <sstream>
#include <regex>
#include <cstring>

#include "libashe/Endian.h"
#include "libashe/RegexHelper.h"


static void __printHelp__(std::ostream &s, const char *pname) noexcept;


int main(const int argc, const char **args)
{
	static const std::regex __RE_NUMERAL__(LASHE_REGEX_FORMAT_NUMERAL);

	std::stringstream sb;
	uint64_t n, tmp;
	uint8_t arr[64];

	if(argc < 2)
	{
		std::cerr << "** Not enough argument." << std::endl;
		::__printHelp__(std::cerr, args[0]);
		return 1;
	}
	else if(!std::regex_match(args[1], __RE_NUMERAL__))
	{
		std::cerr << "** Invalid value format." << std::endl;
		::__printHelp__(std::cerr, args[0]);
		return 1;
	}
	sb.clear();
	sb.str(args[1]);
	sb >> n;

	std::cout << n << ": 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << n << std::endl;
	tmp = n;
	ashe::invertEndian_lb__(&tmp, 8);
	std::cout << "  invertEndian_lb__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << tmp << std::endl;
	tmp = n;
	ashe::invertEndian_ml__(&tmp, 8);
	std::cout << "  invertEndian_ml__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << tmp << std::endl;
	tmp = n;
	ashe::invertEndian_mb__(&tmp, 8);
	std::cout << "  invertEndian_mb__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << tmp << std::endl;

	::memset(arr, 0, 64);
	::memcpy(arr, &n, 8);
	ashe::invertEndian_lb__(arr, 64);
	::memset(arr, 0, 64);
	::memcpy(arr, &n, 8);
	ashe::invertEndian_ml__(arr, 64);
	::memset(arr, 0, 64);
	::memcpy(arr, &n, 8);
	ashe::invertEndian_mb__(arr, 64);

	return 0;
}

void __printHelp__(std::ostream &s, const char *pname) noexcept
{
	s << "LibAshe Endian test program." << std::endl
		<< "Usage: " << pname << " <Number>" << std::endl;
}
