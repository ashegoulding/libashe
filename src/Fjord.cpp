#include <src/libashe/Fjord.h>
#include <sstream>

/*
 * Fjord implementation
 */
namespace ashe
{

Fjord::Fjord() ASHE_NOEXCEPT
	: className("ashe::Fjord")
{
}

Fjord::Fjord(const Fjord &src) ASHE_NOEXCEPT
	: className(src.className)
{
}

Fjord::~Fjord() ASHE_NOEXCEPT {}

std::string Fjord::toString() const ASHE_NOEXCEPT
{
	std::stringstream sb;
	sb << this->className << " @" << this;

	return sb.str();
}

std::string Fjord::getClassName() const ASHE_NOEXCEPT
{
	return this->className;
}

void Fjord::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->className = src.className;
}

} // End of fjord

