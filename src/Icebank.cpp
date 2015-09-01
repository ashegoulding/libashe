#include "libashe/Icebank.h"

namespace ashe
{

Icebank::Icebank() ASHE_NOEXCEPT
{
	this->className = "ashe::Icebank";
}

Icebank::Icebank(const Icebank &src) ASHE_NOEXCEPT
		: motherClass(src)
{
	motherClass::__construct(src);
	this->className = "ashe::Icebank";
}

Icebank::~Icebank() ASHE_NOEXCEPT{}

}
