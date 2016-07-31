#include "__internal.h"

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


void __trim__(std::string& str)
{
	// TODO
}

std::string __trim__(const char* str)
{
	std::string ret;
	__trim__(ret);
	return ret;
}

void __lower__(std::string& str)
{
	// TODO
}

std::string __lower__(const char* str)
{
	std::string ret;
	__lower__(ret);
	return ret;
}

}
