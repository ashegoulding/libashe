#include "TransitiveInterface.h"

namespace ashe
{

TransitiveInterface::~TransitiveInterface() noexcept {}

void TransitiveInterface::__setStateBits(const unsigned int& bit, const bool set) noexcept
{
	if(set)
	{
		if(SB_GOOD == bit)
			this->stateBits = 0;
		else
			this->stateBits |= bit;
	}
	else
		this->stateBits &= ~bit;
}

bool TransitiveInterface::hasDetached() const noexcept
{
	return this->detached;
}

bool TransitiveInterface::good() const noexcept
{
	return this->stateBits == 0;
}

bool TransitiveInterface::failed() const noexcept
{
	return (this->stateBits & SB_FAILED) != 0;
}

bool TransitiveInterface::delayed() const noexcept
{
	return (this->stateBits & SB_DELAYED) != 0;
}

bool TransitiveInterface::ended() const noexcept
{
	return (this->stateBits & SB_ENDED) != 0;
}

bool TransitiveInterface::closed() const noexcept
{
	return (this->stateBits & SB_CLOSED) != 0;
}

size_t TransitiveInterface::retrieved() const noexcept
{
	return this->retrievedSize;
}

size_t TransitiveInterface::sent() const noexcept
{
	return this->sentSize;
}

} /* namespace ashe */
