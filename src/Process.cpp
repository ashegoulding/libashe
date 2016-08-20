#include "libashe/Process.h"

#if ASHE_ISOS_POSIX()
#include <unistd.h>
#endif


namespace ashe
{

LASHE_DECL_EXT PIDType pid__() LASHE_NOEXCEPT
{
#if ASHE_ISOS_WIN()
	// TODO
#error "FIXME"
#elif ASHE_ISOS_POSIX()
	return ::getpid();
#endif
}

}
