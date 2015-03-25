/*
 *
 * http://tools.ietf.org/html/rfc4122
 *
 * xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx (32 or 36)
 *  Where M is version number in heximal and N is one of 8, 9, A or B
 */
#ifndef UUID_H_
#define UUID_H_

#include "Frelish.h"

#include <regex>
#include <random>

namespace ashe
{

/* - Handling capability: All versions to 5
 * - Generating capability: 1, 4, 5 (since version 2 is cross-platform portable)
 *
 * @NOTE
 *  - The implementation of version 1 is not that of RFC.
 *  - It uses a key given when construction and timestamp, instead of MAC address.
 */
class UUID : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef UUID thisClass;

	static const size_t UUID_BYTE_SIZE = 16;
	static const size_t UUID_STRING_LENGTH = 36;

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
			C_NONE,
			C_SHORT_LENGTH,
			C_INVALID_VERSION_DIGIT,
			C_INVALID_MAGIC_DIGIT,
			C_PARSE_ERROR,
			C_EMPTY_CONTENT
		};

		static std::string codeToString__(const Code x) noexcept;

	protected:
		Code code = C_NONE;

		void __construct(const thisClass &src) noexcept;

	public:
		Rune(const Code code = C_NONE, const std::string msg = "") noexcept;
		Rune(const thisClass &src) noexcept;
		virtual ~Rune() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual Code getCode() const noexcept;
	};

	/* Version 4 UUID generator engine
	 */
	class RandomEngine : public Icebank
	{
	public:
		typedef Icebank motherClass;
		typedef RandomEngine thisClass;

	protected:
		std::mt19937_64 __theEngine;
		size_t poolSize = 256;

	public:
		RandomEngine() noexcept;
		RandomEngine(const thisClass &src) throw(StrongRune);
		virtual ~RandomEngine() noexcept;

		virtual thisClass &operator =(const thisClass &src) throw(StrongRune);
		virtual UUID operator()() noexcept;

		virtual thisClass &setPoolSize(const size_t poolSize) noexcept;
		virtual size_t getPoolSize() const noexcept;

		thisClass &randomise() noexcept;
		virtual UUID generate() noexcept;
		virtual unsigned long long random() noexcept;
	};

	static const unsigned int IMPL_VERSION = (unsigned int)VER_SHA1_AND_NAMESPACE;

	static std::string versionToString__(const Version v) noexcept;
	static void validate__(const void *x) throw(Rune);
	static UUID generate__() noexcept;

protected:
	unsigned char data[UUID_BYTE_SIZE];

	virtual void __build(const void *x, const size_t len, const Version ver = VER_NONE) throw(Rune);
	virtual void __parse(const char* x, const size_t len) throw(Rune);

	static void __putGlamour__(void *x, const Version ver) throw(StrongRune);

	UUID() noexcept;
	virtual void __construct(const thisClass &src) noexcept;

public:
	/* Version 1
	 */
	UUID(const unsigned long long seed) noexcept;
	/* From string
	 */
	UUID(const std::string &x) throw(Rune);
	/* From string
	 */
	UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH) throw(Rune);
	/* From raw binary, without dashes and encoding
	 */
	UUID(const void *buffer, const size_t len = thisClass::UUID_BYTE_SIZE) throw(Rune);
	UUID(const thisClass &src) noexcept;
	virtual ~UUID() noexcept;

	virtual thisClass &operator =(const thisClass &src) noexcept;
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

	/* Version 5
	 */
	virtual UUID operator +(const thisClass &x) const noexcept;
	virtual UUID operator +(const std::string &x) const throw(Rune);
	virtual UUID operator +(const char *x) const throw(Rune);

	virtual std::array<unsigned char, thisClass::UUID_BYTE_SIZE> toBinary() const noexcept;
	virtual std::string toString(const bool upper = false) const noexcept;
	virtual Version version() const noexcept;

	/* Version 5
	 */
	virtual UUID merge(const void *x, const size_t len) const throw(Rune);
	virtual UUID merge(const thisClass &x) const noexcept;
	virtual UUID merge(const std::string &x) const throw(Rune);
};

extern const UUID UUID_NS_DNS;
extern const UUID UUID_NS_URL;
extern const UUID UUID_NS_ISO_OID;
extern const UUID UUID_NS_X500;

extern const std::regex UUID_REGEX;

} /* namespace ashe */

#endif /* UUID_H_ */
