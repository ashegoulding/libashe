#include "WeakRune.h"

#include <cstring> // For strerror()
#include <sstream>

namespace ashe
{

WeakRune::WeakRune(std::string msg) noexcept
		: whatString(msg)
{
	this->className = "WeakRune";
}

WeakRune::WeakRune(const thisClass& src) noexcept
		: motherClass(src)
{
	this->className = "WeakRune";
	thisClass::__construct(src);
}

WeakRune::~WeakRune() noexcept
{
}

WeakRune::thisClass& WeakRune::operator =(const thisClass& src) noexcept
{
	motherClass::__construct(src);
	thisClass::__construct(src);
	return *this;
}

std::string WeakRune::toString() const noexcept
{
	return this->whatString;
}

const char* WeakRune::what() const noexcept
{
	return this->whatString.c_str();
}

void WeakRune::__construct(const thisClass& src) noexcept
{
	this->whatString = src.whatString;
	this->errorNumber = src.errorNumber;
}

bool WeakRune::hasError() const noexcept
{
	return this->errorNumber != 0;
}

WeakRune::thisClass& WeakRune::setError() noexcept
{
	this->errorNumber = errno;
	return *this;
}

int WeakRune::getErrorNumber() const noexcept
{
	return this->errorNumber;
}

std::string WeakRune::getErrorString() const noexcept
{
	std::stringstream sb;
	sb << '(' << this->errorNumber << ')' << ::strerror(this->errorNumber);
	return sb.str();
}

}

