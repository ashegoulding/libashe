#include <src/libashe/TransitiveInterface.h>
#include <sstream>

namespace ashe
{

const uint32_t TransitiveInterface::SB_GOOD =			0;
const uint32_t TransitiveInterface::SB_FAILED = 	1;
const uint32_t TransitiveInterface::SB_DELAYED = 	2;
const uint32_t TransitiveInterface::SB_ENDED = 		4;
const uint32_t TransitiveInterface::SB_CLOSED = 	8;

TransitiveInterface::~TransitiveInterface() ASHE_NOEXCEPT {}

void TransitiveInterface::__setStateBits(const uint32_t& bit, const bool set) ASHE_NOEXCEPT
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

bool TransitiveInterface::detached() const ASHE_NOEXCEPT
{
	return this->__detached;
}

bool TransitiveInterface::good() const ASHE_NOEXCEPT
{
	return this->stateBits == 0;
}

bool TransitiveInterface::failed() const ASHE_NOEXCEPT
{
	return (this->stateBits & SB_FAILED) != 0;
}

bool TransitiveInterface::delayed() const ASHE_NOEXCEPT
{
	return (this->stateBits & SB_DELAYED) != 0;
}

bool TransitiveInterface::ended() const ASHE_NOEXCEPT
{
	return (this->stateBits & SB_ENDED) != 0;
}

bool TransitiveInterface::closed() const ASHE_NOEXCEPT
{
	return (this->stateBits & SB_CLOSED) != 0;
}

uint64_t TransitiveInterface::retrieved(const bool overall/* = false*/) const ASHE_NOEXCEPT
{
	return overall? this->retrievedSize : this->lastRetrievedSize;
}

uint64_t TransitiveInterface::sent(const bool overall/* = false*/) const ASHE_NOEXCEPT
{
	return overall? this->sentSize :  this->lastSentSize;
}

void TransitiveInterface::__accumilateSentSize(const uint64_t sent) ASHE_NOEXCEPT
{
	const uint64_t prev = this->sentSize;
	this->sentSize += sent;
	if(this->sentSize < prev)
		this->onSentSizeOverflow(sent, prev);
}

void TransitiveInterface::__accumilateRetrievedSize(const uint64_t sent) ASHE_NOEXCEPT
{
	const uint64_t prev = this->retrievedSize;
	this->retrievedSize += sent;
	if(this->retrievedSize < prev)
		this->onRetrievedSizeOverflow(sent, prev);
}

void TransitiveInterface::onRetrievedSizeOverflow(const uint64_t sizeToAdd, const uint64_t previousSize) ASHE_NOEXCEPT {}

void TransitiveInterface::onSentSizeOverflow(const uint64_t sizeToAdd, const uint64_t previousSize) ASHE_NOEXCEPT {}

TransitiveInterface::thisClass& TransitiveInterface::resetStatistics(const bool sent/* = true*/, const bool retrieved/* = true*/) ASHE_NOEXCEPT
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

TransitiveInterface::TransitiveRune::TransitiveRune(const Code code, const std::string msg) ASHE_NOEXCEPT
		: code(code)
{
	std::stringstream sb;
	this->className = "ashe::TransitiveInterface::TransitiveRune";
	sb << this->className << '[' << thisClass::codeToString__(code) << ']';
	if(! msg.empty())
		sb << ' ' << msg;
	this->whatString = sb.str();
}

TransitiveInterface::TransitiveRune::TransitiveRune(const thisClass& src) ASHE_NOEXCEPT
		: motherClass(src)
{
	this->className = "ashe::TransitiveInterface::TransitiveRune";
	this->__construct(src);
}

TransitiveInterface::TransitiveRune::~TransitiveRune() ASHE_NOEXCEPT {}

TransitiveInterface::TransitiveRune::thisClass& TransitiveInterface::TransitiveRune::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->__construct(src);
	return *this;
}

void TransitiveInterface::TransitiveRune::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->code = src.code;
}

uint32_t TransitiveInterface::TransitiveRune::getCode() const ASHE_NOEXCEPT
{
	return this->code;
}

std::string TransitiveInterface::TransitiveRune::codeToString__(const Code x) ASHE_NOEXCEPT
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

