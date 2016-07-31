#ifndef __LASHE_INTERNAL_H_
#define __LASHE_INTERNAL_H_

#include "libashe/pub.h"

#include <vector>

#include <regex>

#if LASHE_ISOS_WIN()
#include <Windows.h>
#elif LASHE_ISOS_POSIX()
#include <dlfcn.h>
#endif


namespace ashe
{

typedef
#if LASHE_ISOS_POSIX()
		void*
#elif LASHE_ISOS_WIN()
		HMODULE
#endif
		__ModuleType;

extern bool __lashe_initialised;
// malloc()/free()
extern uint32_t *__lashe_initialisedAbilities;
extern std::regex *__lashe_re_version;


template<class E>
void __dropif_uninitialised() LASHE_EXCEPT(E)
{
	if(!__lashe_initialised)
	{
		E e;
		if(typeid(E) == typeid(HelperException))
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

}

#endif
