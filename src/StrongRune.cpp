#include "libashe/StrongRune.h"

namespace ashe
{

void StrongRune::__construct(const thisClass &src) ASHE_NOEXCEPT
{
	this->whatString = src.whatString;
}

StrongRune::StrongRune(std::string msg) ASHE_NOEXCEPT
		: whatString(msg)
{
}

StrongRune::StrongRune(const StrongRune& src) ASHE_NOEXCEPT
{
	this->__construct(src);
}

StrongRune::~StrongRune() ASHE_NOEXCEPT
{
}

StrongRune::thisClass& StrongRune::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	this->__construct(src);
	return *this;
}

const char* StrongRune::what() const ASHE_NOEXCEPT
{
	return this->whatString.c_str();
}

std::string StrongRune::getMsg() const ASHE_NOEXCEPT
{
	return this->whatString;
}

}
