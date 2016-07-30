#include "libashe/AsheClass.h"

namespace ashe
{

AsheClass::~AsheClass() LASHE_NOEXCEPT{}

const char *AsheClass::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::AsheClass";

	return __name__;
}

}
