#include "Icebank.h"

namespace ashe
{

Icebank::Icebank() noexcept
{
	this->className = "ashe::Icebank";
}

Icebank::Icebank(const Icebank &src) noexcept
		: motherClass(src)
{
	motherClass::__construct(src);
	this->className = "ashe::Icebank";
}

Icebank::~Icebank()noexcept{}

}
