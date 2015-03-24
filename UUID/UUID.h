/*
 *
 * http://tools.ietf.org/html/rfc4122
 */
#ifndef UUID_H_
#define UUID_H_

#include "Frelish.h"

#include <regex>

namespace ashe
{

class UUID : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef UUID thisClass;

	static const size_t UUID_BYTE_SIZE = 16;
	static const size_t UUID_STRING_LENGTH = 36;

	static const UUID NS_DNS = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
	static const UUID NS_URL = "6ba7b811-9dad-11d1-80b4-00c04fd430c8";
	static const UUID NS_ISO_OID = "6ba7b812-9dad-11d1-80b4-00c04fd430c8";
	static const UUID NS_X500 = "6ba7b814-9dad-11d1-80b4-00c04fd430c8";

	// For version (1 to 5, version digit) + (89ab magic digit)
	static const std::regex UUID_REGEX = "[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[8-9a-b][0-9a-f]{3}-[0-9a-f]{12}";

	enum Version
	{
		VER_NONE = 0,
		VER_MAC_AND_DATETIME = 1,
		VER_DCE_SECURE = 2,
		VER_MD5_AND_NAMESPACE = 3,
		VER_RANDOM = 4,
		VER_SHA1_AND_NAMESPACE = 5
	};

	class Rune : public WeakRune
	{
	public:
		typedef WeakRune motherClass;
		typedef Rune thisClass;

		enum Code
		{
			C_NONE
		};

		static std::string codeToString__(const Code x) noexcept;

	protected:
		Code code;

		void __construct(const thisClass &src) noexcept;

	public:
		Rune(const Code code = C_NONE, const std::string msg = "") noexcept;
		Rune(const thisClass &src) noexcept;
		virtual ~Rune() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual Code getCode() const noexcept;
	};

	class RandomEngine : public Icebank
	{
	};

	static const unsigned int IMPL_VERSION = (unsigned int)VER_SHA1_AND_NAMESPACE;

	static std::string versionToString__(const Version v) noexcept;
	static void validate__(const unsigned char *x) throw(Rune);

protected:
	unsigned char __data[UUID_BYTE_SIZE];

	UUID() noexcept;
	virtual void __build(const void *x, const size_t len) noexcept;
	virtual void __parse(const char* x, const size_t len) throw(Rune);
	virtual void __validate() const throw(Rune);

public:
	UUID(const std::string &x) throw(Rune);
	UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH) throw(Rune);
	UUID(const void *buffer, const size_t len = thisClass::UUID_BYTE_SIZE) throw(Rune);
	virtual ~UUID() noexcept;

	virtual thisClass &operator =(const std::string &x) throw(Rune);
	virtual thisClass &operator =(const char *x) throw(Rune);

	virtual bool operator ==(const thisClass &x) const noexcept;
	virtual bool operator ==(const std::string &x) const throw(Rune);
	virtual bool operator ==(const char *x) const throw(Rune);

	virtual bool operator <=(const thisClass &x) const noexcept;
	virtual bool operator <=(const std::string &x) const throw(Rune);
	virtual bool operator <=(const char *x) const throw(Rune);

	virtual bool operator >=(const thisClass &x) const noexcept;
	virtual bool operator >=(const std::string &x) const throw(Rune);
	virtual bool operator >=(const char *x) const throw(Rune);

	virtual bool operator <(const thisClass &x) const noexcept;
	virtual bool operator <(const std::string &x) const throw(Rune);
	virtual bool operator <(const char *x) const throw(Rune);

	virtual bool operator >(const thisClass &x) const noexcept;
	virtual bool operator >(const std::string &x) const throw(Rune);
	virtual bool operator >(const char *x) const throw(Rune);

	virtual UUID operator +(const thisClass &x) const noexcept;
	virtual UUID operator +(const std::string &x) const throw(Rune);
	virtual UUID operator +(const char *x) const throw(Rune);

	virtual std::string toString() const noexcept; //@Override
	virtual Version version() const noexcept;

	virtual UUID merge(const void *x, const size_t len) const noexcept;
};

} /* namespace ashe */

#endif /* UUID_H_ */
