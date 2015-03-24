#include "UUID.h"

#include "__coreutils/sha1.h"

namespace ashe
{

UUID::UUID(){}

UUID::UUID(const std::string& x) throw (Rune)
{
	this->__parse(x.data(), x.length());
}

UUID::UUID(const char *x, const size_t len/* = thisClass::UUID_STRING_LENGTH*/) throw(Rune)
{
	this->__parse(x, len);
}

UUID::UUID(const void* buffer, const size_t len) throw (Rune)
{
	this->__build(buffer, len);
}

UUID::~UUID()
{
}

void UUID::__parse(const char *x, const size_t len) throw (Rune)
{
}

void UUID::__build(const void* x, const size_t len)
{
}

void UUID::__validate() const throw (Rune)
{
}

UUID& UUID::operator =(const std::string& x) throw (Rune)
{
}

UUID& UUID::operator =(const char* x) throw (Rune)
{
}

bool UUID::operator ==(const thisClass& x) const
{
}

bool UUID::operator ==(const std::string& x) const throw (Rune)
{
}

bool UUID::operator ==(const char* x) const throw (Rune)
{
}

bool UUID::operator <=(const thisClass& x) const
{
}

bool UUID::operator <=(const std::string& x) const throw (Rune)
{
}

bool UUID::operator <=(const char* x) const throw (Rune)
{
}

bool UUID::operator >=(const thisClass& x) const
{
}

bool UUID::operator >=(const std::string& x) const throw (Rune)
{
}

bool UUID::operator >=(const char* x) const throw (Rune)
{
}

bool UUID::operator <(const thisClass& x) const
{
}

bool UUID::operator <(const std::string& x) const throw (Rune)
{
}

bool UUID::operator <(const char* x) const throw (Rune)
{
}

bool UUID::operator >(const thisClass& x) const
{
}

bool UUID::operator >(const std::string& x) const throw (Rune)
{
}

bool UUID::operator >(const char* x) const throw (Rune)
{
}

UUID UUID::operator +(const thisClass& x) const
{
}

UUID UUID::operator +(const std::string& x) const throw (Rune)
{
}

UUID UUID::operator +(const char* x) const throw (Rune)
{
}

std::string UUID::toString() const
{
}

UUID::Version UUID::version() const
{
}

UUID UUID::merge(const void* x, const size_t len) const
{
}

std::string UUID::versionToString__(const Version v)
{
}

void UUID::validate__(const unsigned char* x) throw (Rune)
{
}

} /* namespace ashe */

namespace ashe
{

}

namespace ashe
{

}
