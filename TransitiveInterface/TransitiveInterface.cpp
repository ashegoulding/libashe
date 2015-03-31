#include "TransitiveInterface.h"

#include <sstream>

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

bool TransitiveInterface::detached() const noexcept
{
	return this->__detached;
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

void TransitiveInterface::onRetrievedSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept{}

void TransitiveInterface::onSentSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept{}

TransitiveInterface::thisClass& TransitiveInterface::resetStatistics(const bool sent/* = true*/, const bool retrieved/* = true*/) noexcept
{
	if(sent)
		this->sentSize = 0;
	if(retrieved)
		this->retrievedSize = 0;

	return *this;
}

} /* namespace ashe */

namespace ashe
{

TransitiveInterface::TransitiveRune::TransitiveRune(const Code code, const std::string msg) noexcept
		: code(code)
{
	std::stringstream sb;
	this->className = "ashe::TransitiveInterface::TransitiveRune";
	sb << this->className << '[' << thisClass::codeToString__(code) << ']';
	if(! msg.empty())
		sb << ' ' << msg;
	this->whatString = sb.str();
}

TransitiveInterface::TransitiveRune::TransitiveRune(const thisClass& src) noexcept
		: motherClass(src)
{
	this->className = "ashe::TransitiveInterface::TransitiveRune";
	this->__construct(src);
}

TransitiveInterface::TransitiveRune::~TransitiveRune() noexcept{}

TransitiveInterface::TransitiveRune::thisClass& TransitiveInterface::TransitiveRune::operator =(const thisClass& src) noexcept
{
	motherClass::__construct(src);
	this->__construct(src);
	return *this;
}

void TransitiveInterface::TransitiveRune::__construct(const thisClass& src) noexcept
{
	this->code = src.code;
}

unsigned int TransitiveInterface::TransitiveRune::getCode() const noexcept
{
	return this->code;
}

std::string TransitiveInterface::TransitiveRune::codeToString__(const Code x) noexcept
{
	switch(x)
	{
	case C_ILLEGAL_STATE: return "Illegal instance state";
	case C_INTERNAL_ERROR: return "Internal error (errno set)";
	case C_DELAYED: return "Data transfer delayed";
	case C_NO_DATA_GIVEN: return "No data given for transfer";
	case C_SHORT_LENGTH: return "Short data range";
	case C_EMPTY_BUFFER: return "No data to read in the buffer";
	}
	return "Unknown";
}

}

