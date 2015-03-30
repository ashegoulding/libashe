#include "TransitiveInterface.h"

namespace ashe
{

const unsigned int TransitiveInterface::SB_GOOD =			0;
const unsigned int TransitiveInterface::SB_FAILED = 	0b00000000000000000000000000000001;
const unsigned int TransitiveInterface::SB_DELAYED = 	0b00000000000000000000000000000010;
const unsigned int TransitiveInterface::SB_ENDED = 		0b00000000000000000000000000000100;
const unsigned int TransitiveInterface::SB_CLOSED = 	0b00000000000000000000000000001000;

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

size_t TransitiveInterface::retrieved(const bool overall/* = false*/) const noexcept
{
	return overall? this->retrievedSize : this->lastRetrievedSize;
}

size_t TransitiveInterface::sent(const bool overall/* = false*/) const noexcept
{
	return overall? this->sentSize :  this->lastSentSize;
}

void TransitiveInterface::__accumilateSentSize(const size_t sent) noexcept
{
	const size_t prev = this->sentSize;
	this->sentSize += sent;
	if(this->sentSize < prev)
		this->onSentSizeOverflow(sent, prev);
}

void TransitiveInterface::__accumilateRetrievedSize(const size_t sent) noexcept
{
	const size_t prev = this->retrievedSize;
	this->retrievedSize += sent;
	if(this->retrievedSize < prev)
		this->onRetrievedSizeOverflow(sent, prev);
}

void ashe::TransitiveInterface::onRetrievedSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept{}

void ashe::TransitiveInterface::onSentSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept{}

} /* namespace ashe */

