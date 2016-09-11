#include <iostream>
#include <iomanip>

#include <sstream>
#include <regex>
#include <cstring>

#include "libashe/Endian.h"
#include "libashe/RegexHelper.h"


static void __printHelp__(std::ostream &s, const char *pname) noexcept;
#define __REINTP_TYPE(T, x) *((T*)x)

int main(const int argc, const char **args)
{
	static const std::regex __RE_NUMERAL__(LASHE_REGEX_FORMAT_NUMERAL);

	std::stringstream sb;
	uint64_t n;
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
	// 64bit
	__REINTP_TYPE(uint64_t, arr) = n;
	ashe::invertEndian_lb__(arr, 8);
	std::cout << "  invertEndian_lb__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << __REINTP_TYPE(uint64_t, arr) << std::endl;
	__REINTP_TYPE(uint64_t, arr) = n;
	ashe::invertEndian_ml__(arr, 8);
	std::cout << "  invertEndian_ml__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << __REINTP_TYPE(uint64_t, arr) << std::endl;
	__REINTP_TYPE(uint64_t, arr) = n;
	ashe::invertEndian_mb__(arr, 8);
	std::cout << "  invertEndian_mb__(): 0x" << std::hex << std::noshowbase << std::setw(16) << std::setfill('0') << __REINTP_TYPE(uint64_t, arr) << std::endl;
	// 32bit
	__REINTP_TYPE(uint32_t, arr) = (uint32_t)n;
	ashe::invertEndian_lb__(arr, 4);
	std::cout << "  invertEndian_lb__(): 0x" << std::hex << std::noshowbase << std::setw(8) << std::setfill('0') << __REINTP_TYPE(uint32_t, arr) << std::endl;
	__REINTP_TYPE(uint32_t, arr) = (uint32_t)n;
	ashe::invertEndian_ml__(arr, 4);
	std::cout << "  invertEndian_ml__(): 0x" << std::hex << std::noshowbase << std::setw(8) << std::setfill('0') << __REINTP_TYPE(uint32_t, arr) << std::endl;
	__REINTP_TYPE(uint32_t, arr) = (uint32_t)n;
	ashe::invertEndian_mb__(arr, 4);
	std::cout << "  invertEndian_mb__(): 0x" << std::hex << std::noshowbase << std::setw(8) << std::setfill('0') << __REINTP_TYPE(uint32_t, arr) << std::endl;
	// 16bit
	__REINTP_TYPE(uint16_t, arr) = (uint16_t)n;
	ashe::invertEndian_lb__(arr, 2);
	std::cout << "  invertEndian_lb__(): 0x" << std::hex << std::noshowbase << std::setw(4) << std::setfill('0') << __REINTP_TYPE(uint16_t, arr) << std::endl;
	__REINTP_TYPE(uint16_t, arr) = (uint16_t)n;
	ashe::invertEndian_ml__(arr, 2);
	std::cout << "  invertEndian_ml__(): 0x" << std::hex << std::noshowbase << std::setw(4) << std::setfill('0') << __REINTP_TYPE(uint16_t, arr) << std::endl;
	__REINTP_TYPE(uint16_t, arr) = (uint16_t)n;
	ashe::invertEndian_mb__(arr, 2);
	std::cout << "  invertEndian_mb__(): 0x" << std::hex << std::noshowbase << std::setw(4) << std::setfill('0') << __REINTP_TYPE(uint16_t, arr) << std::endl;

	// Uncommon type tests.
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
		<< "Usage: " << pname << " <Number>" << std::endl
		<< std::endl
		<< "Bug report: ashe.goulding@gmail.com" << std::endl;
}
