#ifndef __LASHE_INTERNAL_H_
#define __LASHE_INTERNAL_H_

#include "libashe/pub.h"
#include "libashe/helper.h"
#include "libashe/UUID.h"

#include <string>
#include <vector>

#include <regex>

#include <mutex>

#if ASHE_ISOS_WIN()
#include <Windows.h>
#elif ASHE_ISOS_POSIX()
#include <dlfcn.h>
#endif


namespace ashe
{

typedef
#if ASHE_ISOS_POSIX()
		void*
#elif ASHE_ISOS_WIN()
		HMODULE
#endif
		__ModuleType;

extern bool __lashe_initialised;
// malloc()/free()
extern uint32_t *__lashe_initialisedAbilities;
extern std::regex *__lashe_re_version;

extern std::regex
	*__lashe_re_uuidHusk,
	*__lashe_re_uuidStrict;
extern uuid::MersenneTwisterEngine *__lashe_defUUIDEngine;
extern std::mutex *__lashe_mtx_defUUIDEngine;
#define __LOCK_DEF_UUID_ENGINE() std::lock_guard<std::mutex> __lg_lashe_defUUIDEngine(*__lashe_mtx_defUUIDEngine)


template<class E>
void __dropif_uninitialised() LASHE_EXCEPT(E)
{
	if(!__lashe_initialised)
	{
		E e;
		if(typeid(E) == typeid(HelperException) ||
				typeid(E) == typeid(uuid::Exception))
			e.code(E::C_LASHE_UNINITIALISED);
		throw e;
	}
}

template<class E>
void __drop_unimplemented(const char *msg = "", const bool os = false) LASHE_EXCEPT(E)
{
	E e;
	if(typeid(E) == typeid(HelperException))
	{
		e.code(HelperException::C_UNIMPLEMENTED);
		if(os)
			e.arg1(HelperException::SC_OS_RELEATED);
	}
	e.msg(msg);
	throw e;
}

void __die_critical() LASHE_NOEXCEPT;

__ModuleType __loadModule__(const char *name) LASHE_EXCEPT(HelperException);
void __unloadModule__(__ModuleType *mod) LASHE_NOEXCEPT;
template<class T>
T __loadFunc__(const __ModuleType mod, const char *name) LASHE_EXCEPT(HelperException)
{
	T ret;
#if ASHE_ISOS_POSIX()
	ret = (T)::dlsym(mod, name);
	if(nullptr == ret)
	{
		HelperException e;
		std::stringstream sb;

		sb << "Failed to load function '" << name << "()': " << ::dlerror();
		e
			.code(HelperException::C_MODULE_ERROR)
			.msg(sb.str().c_str())
			.arg1(HelperException::SC_FUNC_LOAD);
		throw e;
	}
#elif ASHE_ISOS_WIN()
	// TODO
#error "FIXME"
#endif
	return ret;
}

void __trim__(std::string &str) LASHE_NOEXCEPT;
std::string __trim__(const char *str) LASHE_NOEXCEPT;
void __lower__(std::string &str) LASHE_NOEXCEPT;
std::string __lower__(const char *str) LASHE_NOEXCEPT;
void __upper__(std::string &str) LASHE_NOEXCEPT;
std::string __upper__(const char *str) LASHE_NOEXCEPT;

}

#endif
