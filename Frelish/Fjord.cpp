#include "Fjord.h"

#include <sstream>

/*
 * Fjord implementation
 */
namespace ashe
{

Fjord::Fjord() noexcept
	: className("Fjord")
{
}

Fjord::Fjord(const Fjord &src) noexcept
	: className(src.className)
{
}

Fjord::~Fjord() noexcept{}

std::string Fjord::toString() const noexcept
{
	std::stringstream sb;
	sb << this->className << " @" << this;

	return sb.str();
}

std::string Fjord::getClassName() const noexcept
{
	return this->className;
}

void Fjord::__construct(const thisClass& src) noexcept
{
	this->className = src.className;
}

} // End of fjord
