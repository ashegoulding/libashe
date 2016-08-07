#include "__internal.h"

#include <cstdlib>
#include <algorithm>

namespace ashe
{

bool __lashe_initialised = false;
uint32_t *__lashe_initialisedAbilities = nullptr;
std::regex *__lashe_re_version = nullptr;
std::regex *__lashe_format_numberal = nullptr;
std::regex *__lashe_format_booleanTrue = nullptr;
std::regex *__lashe_format_booleanFalse = nullptr;

std::regex
	*__lashe_re_uuidHusk = nullptr,
	*__lashe_re_uuidStrict = nullptr;
uuid::MersenneTwisterEngine *__lashe_defUUIDEngine = nullptr;
std::mutex *__lashe_mtx_defUUIDEngine = nullptr;

std::regex *__lashe_format_base64 = nullptr;
std::regex *__lashe_format_base64url = nullptr;


void __die_critical() LASHE_NOEXCEPT
{
	::abort();
	::exit(128 + 6);
}

__ModuleType __loadModule__(const char *name) LASHE_EXCEPT(HelperException)
{
	__ModuleType ret;
#if ASHE_ISOS_POSIX()
	ret = ::dlopen(name, RTLD_LAZY);
	if(nullptr == ret)
	{
		HelperException e;
		std::stringstream sb;

		sb << "Failed to load module(" << name << "): " << ::dlerror();
		e
			.code(HelperException::C_MODULE_ERROR)
			.msg(sb.str().c_str())
			.arg1(HelperException::SC_MODULE_OPEN);
		throw e;
	}
#elif ASHE_ISOS_WIN()
	// TODO
#error "FIXME"
#endif
	return ret;
}

void __unloadModule__(__ModuleType *mod) LASHE_NOEXCEPT
{
#if ASHE_ISOS_POSIX()
	if(mod && *mod)
	{
		::dlclose(*mod);
		*mod = nullptr;
	}
#elif ASHE_ISOS_WIN()
	// TODO
#error "FIXME"
#endif
}

void __trim__(std::string& str) LASHE_NOEXCEPT
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

std::string __trim__(const char* str) LASHE_NOEXCEPT
{
	std::string ret = str;
	__trim__(ret);
	return ret;
}

void __lower__(std::string& str) LASHE_NOEXCEPT
{
	std::locale l;
	for(auto &v : str)
		v = std::tolower(v, l);
}

std::string __lower__(const char* str) LASHE_NOEXCEPT
{
	std::string ret = str;
	__lower__(ret);
	return ret;
}

void __upper__(std::string& str) LASHE_NOEXCEPT
{
	std::locale l;
	for(auto &v : str)
		v = std::toupper(v, l);
}

std::string __upper__(const char* str) LASHE_NOEXCEPT
{
	std::string ret = str;
	__upper__(ret);
	return ret;
}

}
