#include "WeakRune.h"

#include <cstring> // For strerror()
#include <sstream>

namespace ashe
{

WeakRune::WeakRune(std::string msg) ASHE_NOEXCEPT
		: whatString(msg)
{
	this->className = "WeakRune";
}

WeakRune::WeakRune(const thisClass& src) ASHE_NOEXCEPT
		: motherClass(src)
{
	this->className = "WeakRune";
	thisClass::__construct(src);
}

WeakRune::~WeakRune() ASHE_NOEXCEPT
{
}

WeakRune::thisClass& WeakRune::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	thisClass::__construct(src);
	return *this;
}

std::string WeakRune::toString() const ASHE_NOEXCEPT
{
	return this->whatString;
}

const char* WeakRune::what() const ASHE_NOEXCEPT
{
	return this->whatString.c_str();
}

void WeakRune::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->whatString = src.whatString;
	this->errorNumber = src.errorNumber;
}

bool WeakRune::hasError() const ASHE_NOEXCEPT
{
	return this->errorNumber != 0;
}

WeakRune::thisClass& WeakRune::setError() ASHE_NOEXCEPT
{
	this->errorNumber = errno;
	return *this;
}

int WeakRune::getErrorNumber() const ASHE_NOEXCEPT
{
	return this->errorNumber;
}

std::string WeakRune::getErrorString() const ASHE_NOEXCEPT
{
	std::stringstream sb;
	const auto str = ::strerror(this->errorNumber);

	sb << '(' << this->errorNumber << ')';
	if(str)
		sb << ' ' << str;
	else
		sb << " Unknown";

	return sb.str();
}

}
