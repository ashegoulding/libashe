#include "__internal.h"

#if LASHE_ISOS_POSIX()
#include <dlfcn.h>
#endif

namespace ashe
{

bool __lashe_initialised = false;
uint32_t *__lashe_initialisedAbilities = nullptr;
std::regex *__lashe_re_version = nullptr;

__ModuleType __lashe_mod_openssl =
#if LASHE_ISOS_WIN()
		NULL
#else
		nullptr
#endif
		;

}
