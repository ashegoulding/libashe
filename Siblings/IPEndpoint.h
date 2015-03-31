/*
 */
#ifndef ASHE_IPENDPOINT_H_
#define ASHE_IPENDPOINT_H_

#include "Frelish.h"

#include <vector>
#include <arpa/inet.h>
#include <cstdint>

namespace ashe
{

class IPEndpoint: public Icebank
{
public:
	typedef Icebank motherClass;
	typedef IPEndpoint thisClass;

	class Rune : public WeakRune
	{
	public:
		typedef WeakRune motherClass;
		typedef Rune thisClass;

		enum Code
		{
		};
	};

	enum Version
	{
		V_NONE, // Empty
		V_4, //
		V_6
	};
	enum Endianness
	{
		ENDIAN_NONE,
		ENDIAN_LITTLE, // Host
		ENDIAN_BIG // Network
	};

protected:
	std::vector<uint8_t> __address; // In Big endian, network order
	uint16_t __port = 0; // In Big endian, network order

	void __construct(const thisClass &src) noexcept;

public:
	IPEndpoint(const Version ver = V_NONE) noexcept;
	// Posix support
	IPEndpoint(const struct sockaddr_in &ipv4) noexcept;
	// Posix support
	IPEndpoint(const struct sockaddr_in6 &ipv6) noexcept;
	// Binary support
	IPEndpoint(const Endianness endian, const Version ver, const void *addr, const uint16_t port) throw(Rune);
	IPEndpoint(const Version ver, const Endianness addrEndian, const void *addr, const Endianness portEndian, const uint16_t port) throw(Rune);
	IPEndpoint(const std::string address, const Version = V_NONE) throw(Rune);
	IPEndpoint(const std::string address, const uint16_t port, const Version = V_NONE) throw(Rune);
	IPEndpoint(const thisClass &src) noexcept;
	virtual ~IPEndpoint() noexcept;

	virtual thisClass &operator =(const thisClass &src) noexcept;

	virtual uint16_t port(const Endianness endian = ENDIAN_LITTLE) const throw(Rune);
	virtual std::vector<uint8_t> address() const throw(Rune);
	virtual std::string address() const noexcept;
	virtual Version version() const noexcept;
	virtual bool empty() const noexcept;

	virtual thisClass &port(const Endianness endian, const uint16_t port) throw(Rune);
	virtual thisClass &address(const Endianness endian, const Version ver, std::vector<uint8_t> &address) throw(Rune);
	virtual thisClass &address(const Version ver, const std::string address) throw(Rune);

	virtual std::string toString() const noexcept;
};

} /* namespace ashe */

#endif /* IPENDPOINT_H_ */
