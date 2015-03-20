#include "StrongRune.h"

namespace ashe
{

void StrongRune::__construct(const thisClass &src) noexcept
{
	this->whatString = src.whatString;
}

StrongRune::StrongRune(std::string msg) noexcept
		: whatString(msg)
{
}

StrongRune::StrongRune(const StrongRune& src) noexcept
{
	this->__construct(src);
}

StrongRune::~StrongRune() noexcept
{
}

StrongRune::thisClass& StrongRune::operator =(const thisClass& src) noexcept
{
	this->__construct(src);
	return *this;
}

const char* StrongRune::what() const noexcept
{
	return this->whatString.c_str();
}

std::string StrongRune::getMsg() const noexcept
{
	return this->whatString;
}

}
