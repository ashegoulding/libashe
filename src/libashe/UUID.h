/*
* UUID.h
*
* Ashe's UUID utility with C++11
*
* @Maintained
*  2016 Q2
* @Author
*  Ashe David Sterkhus
*  Blame to: ashe.goulding+blame@gmail.com
* @REFER
*  RFC 4112, https://www.ietf.org/rfc/rfc4122.txt
*
* @NOTE
*  xxxxxxxx-xxxx-Mxx-Nxxx-xxxxxxxxxxxx (32 or 36)
*  Where M is version number in heximal and N is one of 8, 9, A or B
*/
#ifndef LASHE_UUID_H_
#define LASHE_UUID_H_

#include "pub.h"
#include "AsheClass.h"
#include "AsheException.h"


namespace ashe
{
class UUID;

namespace uuid
{

enum /* uint32_t */ Version
{
	V_NONE,
	V_MAC_AND_DATETIME,
	V_DCE_SECURE,
	V_MD5_AND_NAMESPACE,
	V_RANDOM,
	V_SHA1_AND_NAMESPACE
};

#pragma pack(push, LASHE_PUB_ALIGN)

class LASHE_DECL_EXT Exception : public AsheException
{
public:
	typedef AsheException motherClass;
	typedef Exception thisClass;

	enum /* uint32_t */ Code
	{
		C_NONE,
		C_LASHE_UNINITIALISED,
		C_LASHE_NO_ABILITY,
		C_NOT_FOUND,
		C_SHORT_LENGTH,
		C_INVALID_MAGIC_DIGIT, // Magic digit(N) is neither  8, 9, A nor B
		C_INVALID_FORMAT // Invalid string format.
	};

public:
	virtual ~Exception() LASHE_NOEXCEPT;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override
	virtual const char *code2str(const uint32_t x) const LASHE_NOEXCEPT; //@Override
};

// Version 4 UUID generator engine interface
class LASHE_DECL_EXT RandomEngine : public AsheClass
{
public:
	typedef AsheClass motherClass;
	typedef RandomEngine thisClass;

public:
	virtual ~RandomEngine() LASHE_NOEXCEPT;

	// Calls generate()
	virtual UUID operator()() LASHE_NOEXCEPT;

	/* Generates a version 4, randomly generated UUID.
	* Invokes random() method for randomness.
	*/
	virtual UUID generate() LASHE_NOEXCEPT = 0;
	/* randomise()
	* Be sure ot call this method prior to use.
	*/
	virtual thisClass &randomise() LASHE_NOEXCEPT = 0; //@Pure virtual
};

struct __MersenneTwisterEngineContext;

// Random UUID engine implementation that uses std::mt19937_64
class LASHE_DECL_EXT MersenneTwisterEngine : public RandomEngine
{
public:
	typedef RandomEngine motherClass;
	typedef MersenneTwisterEngine thisClass;

protected:
	friend RandomEngine* mkRandomEngine(const char*) LASHE_EXCEPT(Exception);

	__MersenneTwisterEngineContext *__privCtx;
	/* PoolSize
	* Generates 32 random 64bit integers to make a random UUID.
	* Larger the value, the less chance of collision occurrence.
	* But 32 (possible combination = 2^(32*8*8)) is just fine
	*/
	size_t __poolSize;

	MersenneTwisterEngine() LASHE_NOEXCEPT;

public:
	MersenneTwisterEngine(const thisClass&) = delete;
	virtual ~MersenneTwisterEngine() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;

	virtual size_t poolSize() const LASHE_NOEXCEPT;
	virtual thisClass &poolSize(const size_t size) LASHE_NOEXCEPT;

	virtual UUID generate() LASHE_NOEXCEPT; //@Implement
	// Using random_device, current timestamp, thread id, and PID.
	virtual thisClass &randomise() LASHE_NOEXCEPT; //@Implement
};

#pragma pack(pop)

// Reserved UUID namespaces according to RFC4122.
#define LASHE_UUID_NS_DNS			"6ba7b810-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_URL			"6ba7b811-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_ISO_OID		"6ba7b812-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_X500			"6ba7b814-9dad-11d1-80b4-00c04fd430c8"

/* A regular expression of a valid UUID string.
*
* @NOTE
*  - Also validates version and magic digit.
*		Blocks any UUID string that seems legit but isn't because of the magic digits.
*/
// Does not check version digit.
#define LASHE_UUID_REGEX_STRICT	"^(\\s+)?[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[8-9a-b][0-9a-f]{3}-[0-9a-f]{12}(\\s+)?$"
// Does not check magic digit.
#define LASHE_UUID_REGEX_HUSK	"^(\\s+)?[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}(\\s+)?$"
static const size_t RAW_BYTE_SIZE = 16;
// Including null-terminator.
static const size_t STRING_SIZE = 36;

/* implVersion__()
* Implemented levels of UUID generation.
* Will return: {5}
*/
LASHE_DECL_EXT const uint32_t *implVersion__() LASHE_NOEXCEPT;
LASHE_DECL_EXT const char *versionToString__(const uint32_t v) LASHE_NOEXCEPT;

/* ashe::uuid::mkRandomEngine(const char *name) LASHE_EXCEPT(Exception)
* Instantiates RandomEngine for UUID generation.
* Randomise the instance before using it.
* 'name':
*	- "MersenneTwisterEngine"
*/
LASHE_DECL_EXT RandomEngine *mkRandomEngine(const char *name) LASHE_EXCEPT(Exception);

/* # ashe::uuid::generate() LASHE_EXCEPT(Exception)
* Generates random UUID by using LibAshe default random engine(`MersenneTwisterEngine`).
* This function is thread safe. There would be bottleneck throttle if this function
* is used concurrently. Refer to `mkRandomEngine()` if this is the case.
*/
LASHE_DECL_EXT UUID generate() LASHE_EXCEPT(Exception);
LASHE_DECL_EXT UUID fromString(const char *str) LASHE_EXCEPT(Exception);
LASHE_DECL_EXT UUID fromString(const char *str, const size_t len) LASHE_EXCEPT(Exception);
LASHE_DECL_EXT UUID fromRaw(const void *p) LASHE_EXCEPT(Exception);
LASHE_DECL_EXT void validateRaw(const void *p) LASHE_EXCEPT(Exception);
LASHE_DECL_EXT void validateString(const char *str) LASHE_EXCEPT(Exception);
LASHE_DECL_EXT void validateString(const char *str, const size_t len) LASHE_EXCEPT(Exception);

}

#pragma pack(push, LASHE_PUB_ALIGN)

/* UUID
* Class for parsing and generating raw UUID.
* The UUID namespace support is experimental.
*
* - Handling capability: All versions to 5
* - Generating capability: 4, 5
*/
class LASHE_DECL_EXT UUID : public AsheClass
{
public:
	typedef AsheClass motherClass;
	typedef UUID thisClass;

public:
	uint8_t data[uuid::RAW_BYTE_SIZE];

public:
	UUID() LASHE_NOEXCEPT;
	UUID(const thisClass &src) LASHE_NOEXCEPT;
	virtual ~UUID() LASHE_NOEXCEPT;

	virtual thisClass &operator =(const thisClass &src) LASHE_NOEXCEPT;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override

	/* Relational operators. Treats the instance as 128bit integer.
	*
	* @NOTE
	*  - Uses ::memcmp() internally for comparison
	*  - For strings, these methods instantiate an UUID out of the given string and then compares to that.
	*  - So for strings, they can throw Rune when invalid format of UUID string is given.
	*/
	virtual bool operator ==(const thisClass &x) const LASHE_NOEXCEPT;
	virtual bool operator ==(const char *x) const LASHE_EXCEPT(uuid::Exception);
	virtual bool operator !=(const thisClass &x) const LASHE_NOEXCEPT;
	virtual bool operator !=(const char *x) const LASHE_EXCEPT(uuid::Exception);

	// Alias of merge()
	virtual UUID operator +(const thisClass &x) const LASHE_NOEXCEPT;

	virtual uint32_t version() const LASHE_NOEXCEPT;

	// ASSERTION: 'y' must be 36 or more bytes (uuid::STRING_SIZE)
	virtual const thisClass &string(char *y, const bool upper = false) const LASHE_NOEXCEPT;

	// Namespace(merge) operations for version 5 UUID generation.
	virtual UUID merge(const thisClass &x) const LASHE_EXCEPT(uuid::Exception);
};

#pragma pack(pop)

}

#endif
