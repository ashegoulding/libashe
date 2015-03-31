/*
 * IPEndpoint.cpp
 *
 *  Created on: 2015. 3. 31.
 *      Author: david
 */

#include <Siblings/IPEndpoint.h>

namespace ashe
{

IPEndpoint::IPEndpoint(const Version ver/* = V_NONE*/) noexcept
{
	switch(ver)
	{
	case V_4: this->__address.resize(4); break;
	case V_6: this->__address.resize(16); break;
	}
}

IPEndpoint::IPEndpoint(const struct sockaddr_in& ipv4) noexcept
	: __address((unsigned char*)&ipv4.sin_addr, 4)
	, __port(ipv4.sin_port)
{
}

IPEndpoint::IPEndpoint(const struct sockaddr_in6& ipv6) noexcept
	: __address((unsigned char*)&ipv6.sin6_addr, 16)
	, __port(ipv6.sin6_port)
{
}

IPEndpoint::IPEndpoint(const Endianness endian, const Version ver, const void* addr, const unsigned short port) throw (Rune)
{
}

IPEndpoint::IPEndpoint(const Version ver, const Endianness addrEndian,
		const void* addr, const Endianness portEndian,
		const unsigned short port) throw (Rune)
{
}

IPEndpoint::IPEndpoint(const std::string address, const Version version)
		throw (Rune)
{
}

IPEndpoint::IPEndpoint(const std::string address, const unsigned short port,
		const Version version) throw (Rune)
{
}

IPEndpoint::IPEndpoint(const thisClass& src)
{
}

IPEndpoint::~IPEndpoint()
{
	// TODO Auto-generated destructor stub
}

thisClass& IPEndpoint::operator =(const thisClass& src)
{
}

void IPEndpoint::__construct(const thisClass& src)
{
}

unsigned short IPEndpoint::port(const Endianness endian) const throw (Rune)
{
}

std::vector<unsigned char> IPEndpoint::address(const Endianness endian) const
		throw (Rune)
{
}

std::string IPEndpoint::address() const
{
}

Version IPEndpoint::version() const
{
}

bool IPEndpoint::empty() const
{
}

thisClass& IPEndpoint::port(const Endianness endian, const unsigned short port)
		throw (Rune)
{
}

thisClass& IPEndpoint::address(const Endianness endian, const Version ver,
		std::vector<unsigned char>& address) throw (Rune)
{
}

thisClass& IPEndpoint::address(const Version ver, const std::string address)
		throw (Rune)
{
}

std::string IPEndpoint::toString() const
{
}

} /* namespace ashe */
