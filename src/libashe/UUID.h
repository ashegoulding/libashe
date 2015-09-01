/*
* UUID.h
*
* Ashe's UUID utility with C++11
*
* @Maintained
*  2015 Q1
* @Author
*  Ashe David Sterkhus
*  Blame to: ashe.goulding+blame@gmail.com
* @CONFIRMING_TO
*  RFC 4112, https://www.ietf.org/rfc/rfc4122.txt
* @COLOPHON
*  This file is part of libashe, Ashe's C++11/98 utility stuff
*
* @NOTE
*  xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx (32 or 36)
*  Where M is version number in heximal and N is one of 8, 9, A or B
*/
#ifndef ASHE_UUID_H_
#define ASHE_UUID_H_

#include "Frelish.h"
#include "HostEndian.h"

#if ASHE_IS_PDP_ENDIAN
#error "Unsupported endian: PDP, middle endian"
#endif

#include <regex>
#include <random>
#include <cstdint>

#include <array>

namespace ashe
{

class ASHE_DECL_EXT UUID : public Icebank
{
/* - Handling capability: All versions to 5
* - Generating capability: 1, 4, 5
*
* @NOTE
*  - The implementation of version 1 is not that of RFC.
*  - It uses a key given on construction and timestamp, instead of MAC address.
*/
public:
	typedef Icebank motherClass;
	typedef UUID thisClass;

	// One can make one's own namespace in any version, not invading none of existing namespaces as follows:
	static const UUID UUID_NS_DNS/* = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"*/;
	static const UUID UUID_NS_URL/* = "6ba7b811-9dad-11d1-80b4-00c04fd430c8"*/;
	static const UUID UUID_NS_ISO_OID/* = "6ba7b812-9dad-11d1-80b4-00c04fd430c8"*/;
	static const UUID UUID_NS_X500/* = "6ba7b814-9dad-11d1-80b4-00c04fd430c8"*/;

	/* A regular expression for searching an UUID string within a text or a string
	* It is case sensitive. To search case insensitive, make your own std::regex instance based on this instance.
	*
	* @NOTE
	*  - It also validates version and magic digit. Any UUID string that looks like it but invalid because of the digits,
	*  - does not meets this expression.
	*/
	static const std::regex REGEX/*("[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[8-9a-b][0-9a-f]{3}-[0-9a-f]{12}")*/;

	static const size_t UUID_BYTE_SIZE = 16; // = 128bits
	static const size_t UUID_STRING_LENGTH = 36; // Including dashes

	enum Version
	{
		VER_NONE = 0,
		VER_MAC_AND_DATETIME = 1, // Supported. It does not use MAC in this class
		VER_DCE_SECURE = 2, // Generation not supported but parsing existing one is possible
		VER_MD5_AND_NAMESPACE = 3, // Generation is not supported. This version is not 'preferable'. Parsing existing one is possible
		VER_RANDOM = 4, // Supported
		VER_SHA1_AND_NAMESPACE = 5 // Supported
	};

	class ASHE_DECL_EXT Rune : public WeakRune
	{
	public:
		typedef WeakRune motherClass;
		typedef Rune thisClass;

		enum Code
		{
			C_NONE,
			C_SHORT_LENGTH, // Buffer-overrun safety failure
			C_INVALID_VERSION_DIGIT, // Version digit(M) is not either of 1, 2, 3, 4 or 5
			C_INVALID_MAGIC_DIGIT, // Magic digit(N) is not either of 8, 9, A or B
			C_PARSE_ERROR, // Format error. Dash dislocated or illegal character (only hexadecimal characters and dashes allowed)
			C_EMPTY_CONTENT // Merging a data with a namespace, the given data is empty
		};

		static std::string codeToString__(const Code x) ASHE_NOEXCEPT;

	protected:
		Code code = C_NONE;

		void __construct(const thisClass &src) ASHE_NOEXCEPT;

	public:
		Rune(const Code code = C_NONE, const std::string msg = "") ASHE_NOEXCEPT;
		Rune(const thisClass &src) ASHE_NOEXCEPT;
		virtual ~Rune() ASHE_NOEXCEPT;

		virtual thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

		virtual Code getCode() const ASHE_NOEXCEPT;
	};

	class ASHE_DECL_EXT RandomEngine : public Icebank
	{
	/* Version 4 UUID generator engine interface
	*
	* @NOTE
	*  libashe provides an engine implementation using mt19937 random engine.
	*  If any problem occurs with the implementation(like collision),
	*  Make your own engine.
	*/
	public:
		typedef Icebank motherClass;
		typedef RandomEngine thisClass;

	protected:
		/*
		* Generates 32 random 64bit integers to make a random UUID.
		* Larger the value, the less chance of collision occurrence.
		* But 32 (possible combination = 2^(32*8*8)) is just fine
		*/
		size_t poolSize = 32;

		void __construct(const thisClass &src) ASHE_NOEXCEPT;

	public:
		virtual ~RandomEngine() ASHE_NOEXCEPT;

		// Copies only 'poolSize' member
		virtual thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

		// Alias of generate()
		virtual UUID operator()() ASHE_NOEXCEPT;

		virtual size_t getPoolSize() const ASHE_NOEXCEPT;
		virtual thisClass &setPoolSize(const size_t size) ASHE_NOEXCEPT;

		/* Generates a version 4, randomly generated UUID.
		* Invokes random() method for randomness.
		*/
		virtual UUID generate() ASHE_NOEXCEPT;
		// Randomise the instance's random engine here like: srand(time(NULL)), std::seed() ...
		virtual thisClass &randomise() ASHE_NOEXCEPT = 0; //@Pure virtual
		// Generate and return a random number
		virtual uint64_t random() ASHE_NOEXCEPT = 0; //@Pure virtual
	};

	class ASHE_DECL_EXT MersenneTwisterEngine : public RandomEngine
	{
	// Random UUID generating engine implementation using std::mt19937_64
	public:
		typedef RandomEngine motherClass;
		typedef MersenneTwisterEngine thisClass;

	protected:
		std::mt19937_64 __engine;

	public:
		MersenneTwisterEngine() ASHE_NOEXCEPT;
		MersenneTwisterEngine(const thisClass &src) ASHE_NOEXCEPT;
		virtual ~MersenneTwisterEngine() ASHE_NOEXCEPT;

		virtual thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

		virtual thisClass &randomise() ASHE_NOEXCEPT; //@Implement
		virtual uint64_t random() ASHE_NOEXCEPT; //@Implement
	};

	static const unsigned int IMPL_VERSION = (unsigned int)VER_SHA1_AND_NAMESPACE;

	static std::string versionToString__(const Version v) ASHE_NOEXCEPT;
	// Validates given 16bytes octet array: Version and magic digit
	static void validate__(const void *x) throw(Rune);
	/* Returns a randomly generated version 4 UUID by using default engine of the library.
	*
	* @NOTE
	*  - This method is thread-safe, uses mutex internally hence calling this method from multiple threads is safe.
	*  - But random generation takes a bit of CPU power, eventually causes bottle-neck!
	*  - For frequent usage, instantiate individual random engines for each threads to use.
	* @RETURN
	*  - A randomly generated version 4 UUID instance from library's default random engine.
	*/
	static UUID generate__() ASHE_NOEXCEPT;

protected:
	uint8_t data[UUID_BYTE_SIZE]; // It's 16bytes / 128bits.

	/* Constructs the instance from given raw array, without encoding and dashes.
	*
	* @BEHAVIOUR
	*  - If 'ver' is VER_NONE, the method treats given array as a randomly generated byte array: puts UUID format with __putGlamour__()
	*  - Otherwise it treats the array as existing UUID, tries to validate it.
	* @THROWS: Rune instance
	*  - When 'len' is shorter than 16
	*  - After construction, validation fails: Version and magic digits
	*/
	virtual void __build(const void *x, const size_t len, const Version ver = VER_NONE) throw(Rune);
	/* Parses and then constructs the instance from given string
	*
	* @THROWS: Rune
	*  - C_SHORT_LENGTH when 'len' is shorter than 36
	*  - C_INVALID_VERSION_DIGIT or C_INVALID_MAGIC_DIGIT on invalid UUID format
	*  - C_PARSE_ERROR when one or more dashes are dislocated (They aren't there where they are supposed to be)
	* @NOTE
	*  - Works fine on both upper and lower case hexadecimal characters
	*  - One might think this method is rather 'strict' but it was for avoidance to GUID
	*/
	virtual void __parse(const char* x, const size_t len) throw(Rune);

	/* Formats given array into UUID format.
	*
	* @BEHAVIOUR
	*  - Version digit: as is given with 'ver'
	*  - Magic digit: from given array, N = 8 + (N % 3)
	* @THROWS: StrongRune
	*  - If 'ver' is out of range or VER_NONE
	*/
	static void __putGlamour__(void *x, const Version ver) throw(StrongRune);

	// Empty instance is not allowed to outside
	UUID() ASHE_NOEXCEPT;
	virtual void __construct(const thisClass &src) ASHE_NOEXCEPT;

public:
	/* Version 1
	* Give 'seed' as MAC address if it's possible, makes it follow the recommendation.
	*
	* @BEHAVIOUR
	*  - hashes seed + duration since epoch of high resolution clock.
	*  - constructs the instance out of it by __build()
	* @NOTE
	*  - Wish to give 0 as 'seed'? Just 0 is ambiguous with NULL. Use UUID((unsigned long long)0)
	*  - It is not RFC standard, recommended version 1! Intended for simple, short scalable use.
	*  - Follows big-endian. Reverse the argument manually to represent little-endian
	*  - For security reasons, this version is prone to be avoided
	*/
	UUID(const std::array<uint8_t, 8> trailing) ASHE_NOEXCEPT;
	// Alias of UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH) throw(Rune)
	UUID(const std::string &x) throw(Rune);
	/* Constructs an instance from given string.
	 * This constructor is set of __parse() and __putGlamour__(). As for details, refer to those methods.
	 */
	UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH) throw(Rune);
	/* Constructs an instance from given raw array.
	 * This constructor is set of __build() and validate__(). As for details, refer to those methods.
	 */
	UUID(const void *buffer, const size_t len = thisClass::UUID_BYTE_SIZE) throw(Rune);
	// Copy-constructs from given existing instance.
	UUID(const thisClass &src) ASHE_NOEXCEPT;
	virtual ~UUID() ASHE_NOEXCEPT;

	// Copy-constructs from given existing instance.
	virtual thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;
	// Alias of UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH)
	virtual thisClass &operator =(const std::string &x) throw(Rune);
	// Alias of UUID(const char *x, const size_t len = thisClass::UUID_STRING_LENGTH)
	virtual thisClass &operator =(const char *x) throw(Rune);

	/* Relational operators. Treats the instance as 128bit integer.
	*
	* @NOTE
	*  - Uses ::memcmp() internally for comparison
	*  - For strings, these methods instantiate an UUID out of the given string and then compares to that.
	*  - So for strings, they can throw Rune when invalid format of UUID string is given.
	*/
	virtual bool operator ==(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator ==(const std::string &x) const throw(Rune);
	virtual bool operator ==(const char *x) const throw(Rune);
	virtual bool operator !=(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator !=(const std::string &x) const throw(Rune);
	virtual bool operator !=(const char *x) const throw(Rune);
	virtual bool operator <=(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator <=(const std::string &x) const throw(Rune);
	virtual bool operator <=(const char *x) const throw(Rune);
	virtual bool operator >=(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator >=(const std::string &x) const throw(Rune);
	virtual bool operator >=(const char *x) const throw(Rune);
	virtual bool operator <(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator <(const std::string &x) const throw(Rune);
	virtual bool operator <(const char *x) const throw(Rune);
	virtual bool operator >(const thisClass &x) const ASHE_NOEXCEPT;
	virtual bool operator >(const std::string &x) const throw(Rune);
	virtual bool operator >(const char *x) const throw(Rune);

	// Alias of merge()
	virtual UUID operator +(const thisClass &x) const ASHE_NOEXCEPT;
	// Alias of merge()
	virtual UUID operator +(const std::string &x) const throw(Rune);
	// Alias of merge()
	virtual UUID operator +(const char *x) const throw(Rune);

	// Returns raw array of the instance's UUID data
	virtual std::array<uint8_t, thisClass::UUID_BYTE_SIZE> toBinary() const ASHE_NOEXCEPT;
	/* Returns a string representing the instance's UUID data with dashes, numbers and hexadecimal characters.
	*
	* @NOTE
	*  - Give 'upper' true, wish to take a string in upper cases, though it is not standard according to RFC4112
	*  - Maybe it overrides motherClass::toString() ? Not sure. It depends on what compiler says!
	*/
	virtual std::string toString(const bool upper = false) const ASHE_NOEXCEPT; //@Override ?
	// Returns version of the instance's UUID data
	virtual Version version() const ASHE_NOEXCEPT;

	/* Namespace(merge) operations for version 5 UUID generation
	*
	* @NOTE
	*  - For strings, these methods instantiate an UUID out of the given string and then merges with that.
	*  - So for strings, they can throw Rune when invalid format of UUID string is given.
	*  - To make UUID within the DNS namespace: newUUID = ashe::UUID_NS_DNS + yourUUID
	*  - One can make one's own namespace in any version.
	*/
	virtual UUID merge(const void *x, const size_t len) const throw(Rune);
	virtual UUID merge(const thisClass &x) const ASHE_NOEXCEPT;
	virtual UUID merge(const std::string &x) const throw(Rune);
};

} /* namespace ashe */

#endif /* UUID_H_ */
