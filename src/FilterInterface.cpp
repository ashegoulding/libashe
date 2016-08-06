#include "libashe/FilterInterface.h"


namespace ashe
{

/********************
* FilterInterface
*/
FilterInterface::FilterInterface() LASHE_NOEXCEPT
	: __pushed(0)
{
}

FilterInterface::~FilterInterface() LASHE_NOEXCEPT
{
}

FilterInterface::operator bool() LASHE_NOEXCEPT
{
	return this->ready();
}

size_t FilterInterface::pushed() const LASHE_NOEXCEPT
{
	return this->__pushed;
}

void FilterInterface::__dropif_notReady() const LASHE_EXCEPT(FilterException)
{
	if(!this->ready())
	{
		FilterException e;
		e.code(FilterException::C_NOT_READY);
		throw e;
	}
}

/********************
* FilterException
*/
FilterException::~FilterException() LASHE_NOEXCEPT
{
}

const char *FilterException::code2str(const uint32_t x) const LASHE_NOEXCEPT
{
	static const char *__str__[] = {
			"undefined",
			"ability uninitialised",
			"not ready",
			"illegal argument",
			"no data",
			"short buffer",
			"unopened",
			"error from library"
	};
	switch(x)
	{
	case C_UNDEFINED:
	case C_ABILITY_UNINITIALISED:
	case C_NOT_READY:
	case C_ILLEGAL_ARGUMENT:
	case C_NO_DATA:
	case C_SHORT_BUFFER:
	case C_UNOPENED:
	case C_ERROR_FROM_LIB:
		return __str__[x];
	}
	return motherClass::code2str(x);
}

}
