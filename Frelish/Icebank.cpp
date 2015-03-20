#include "Icebank.h"

namespace ashe
{

Icebank::Icebank() noexcept
{
	motherClass::className = "Icebank";
}

Icebank::Icebank(const Icebank &src) noexcept
		: motherClass(src)
{
	motherClass::__construct(src);
}

Icebank::~Icebank(){}

}
