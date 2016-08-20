#ifndef LASHE_PROCESS_H_
#define LASHE_PROCESS_H_

#include "pub.h"
#if ASHE_ISOS_WIN()
#include <windows.h>
#elif ASHE_ISOS_POSIX()
#include <sys/types.h>
#else
#error "Unsupported OS."
#endif

namespace ashe
{

typedef
#if ASHE_ISOS_WIN()
		DWORD
#elif ASHE_ISOS_POSIX()
		pid_t
#endif
		PIDType;


LASHE_DECL_EXT PIDType pid__() LASHE_NOEXCEPT;

}

#endif
