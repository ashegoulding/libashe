#include "__internal.h"

#include <cstdlib>
#include <algorithm>

namespace ashe
{

bool __lashe_initialised = false;
uint32_t *__lashe_initialisedAbilities = nullptr;
std::regex *__lashe_re_version = nullptr;

std::regex
	*__lashe_re_uuidHusk = nullptr,
	*__lashe_re_uuidStrict = nullptr;
uuid::MersenneTwisterEngine *__lashe_defUUIDEngine = nullptr;
std::mutex *__lashe_mtx_defUUIDEngine = nullptr;


void __die_critical() LASHE_NOEXCEPT
{
	::abort();
	::exit(128 + 6);
}

void __trim__(std::string& str)
{
	std::string::const_iterator start, end, it, beg, itEnd;

	if(str.empty())
		return;
	beg = it = str.begin();
	start = end = itEnd = str.end();
	for(; it!=itEnd; it++)
	{
		if(!std::isspace(*it))
		{
			start = it;
			break;
		}
	}
	if(itEnd == start)
	{
		str.clear();
		return;
	}
	it = itEnd - 1;
	for(; it!=beg; it--)
	{
		if(!std::isspace(*it))
		{
			str.erase(beg, start);
			str.erase(it + 1, itEnd);
			return;
		}
	}
	__die_critical();
}

std::string __trim__(const char* str)
{
	std::string ret;
	__trim__(ret);
	return ret;
}

void __lower__(std::string& str)
{
	std::locale l;
	for(auto &v : str)
		v = std::tolower(v, l);
}

std::string __lower__(const char* str)
{
	std::string ret;
	__lower__(ret);
	return ret;
}

void __upper__(std::string& str)
{
	std::locale l;
	for(auto &v : str)
		v = std::toupper(v, l);
}

std::string __upper__(const char* str)
{
	std::string ret;
	__upper__(ret);
	return ret;
}

}
