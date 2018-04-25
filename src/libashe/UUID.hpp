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
#ifndef LASHE_UUID_HPP_
#define LASHE_UUID_HPP_
#include "Class.hpp"
#include "Exception.hpp"
#include "pub.hpp"


namespace ashe {

#pragma pack(push, LASHE_PUB_ALIGN)
LASHE_DECL_EXCEPTION(UUIDException, Exception);
#pragma pack(pop)

// Reserved UUID namespaces according to RFC4122.
#define LASHE_UUID_NS_DNS "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_URL "6ba7b811-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_ISO_OID "6ba7b812-9dad-11d1-80b4-00c04fd430c8"
#define LASHE_UUID_NS_X500 "6ba7b814-9dad-11d1-80b4-00c04fd430c8"

#pragma pack(push, LASHE_PUB_ALIGN)

struct __UUIDPrivData;

/* Class for parsing and generating raw UUID.
 * The UUID namespace support is experimental.
 *
 * - Handling capability: All versions to 5
 * - Generating capability: 4, 5
 */
class LASHE_DECL_EXT UUID : public Class {
public:
    typedef Class motherClass;
    typedef UUID thisClass;

    static constexpr const size_t RAW_BYTE_SIZE = 16;
    static constexpr const size_t STRING_LENGTH = 36;

    enum class Version {
        NONE = 0,
        MAC_AND_DATETIME,
        DCE_SECURE,
        MD5_AND_NAMESPACE,
        RANDOM,
        SHA1_AND_NAMESPACE
    };

    static const char *tostr(const Version v) LASHE_NOEXCEPT;

public:
    __UUIDPrivData *__pd;

    void __mkStr() const LASHE_NOEXCEPT;

public:
    UUID(const char *str);
    UUID(const uint8_t *buf);
    UUID(const thisClass &src) LASHE_NOEXCEPT;
    virtual ~UUID() LASHE_NOEXCEPT;

    virtual thisClass &operator=(const thisClass &src) LASHE_NOEXCEPT;

    virtual const char *className() const LASHE_NOEXCEPT override;

    /* Relational operators. The operators treat operand instances as 128bit
     * integers.
     *
     * @NOTE
     *  - Uses ::memcmp() internally for comparison
     */
    virtual bool operator==(const thisClass &x) const LASHE_NOEXCEPT;
    virtual bool operator!=(const thisClass &x) const LASHE_NOEXCEPT;

    // Parses version magic.
    virtual Version version() const LASHE_NOEXCEPT;

    virtual const char *str() const LASHE_NOEXCEPT;
    virtual const uint8_t *data() const LASHE_NOEXCEPT;
};

// Version 4 UUID generator engine interface
class LASHE_DECL_EXT RandomUUIDEngineInterface {
public:
    typedef Class motherClass;
    typedef RandomUUIDEngineInterface thisClass;

public:
    virtual ~RandomUUIDEngineInterface() LASHE_NOEXCEPT;

    // Calls generate()
    virtual UUID operator()() LASHE_NOEXCEPT;

    /* Generates a version 4, randomly generated UUID.
     * Invokes random() method for randomness.
     */
    virtual UUID generate() LASHE_NOEXCEPT = 0;
    /* randomise()
     * Be sure ot call this method prior to use.
     */
    virtual thisClass &randomise() LASHE_NOEXCEPT = 0;
};

struct __MersenneTwisterUUIDEnginePrivData;

// Random UUID engine implementation that utilises std::mt19937_64
class LASHE_DECL_EXT MersenneTwisterUUIDEngine
    : public RandomUUIDEngineInterface,
      public Class {
public:
    typedef RandomUUIDEngineInterface motherClass;
    typedef MersenneTwisterUUIDEngine thisClass;

protected:
    __MersenneTwisterUUIDEnginePrivData *__pd;

public:
    MersenneTwisterUUIDEngine();
    MersenneTwisterUUIDEngine(const thisClass &) = delete;
    MersenneTwisterUUIDEngine(thisClass &&) = delete;
    virtual ~MersenneTwisterUUIDEngine() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &) = delete;
    thisClass &operator=(thisClass &&) = delete;

    virtual const char *className() const LASHE_NOEXCEPT override;

    /* PoolSize
     * Generates 32 random 64bit integers to make a random UUID.
     * Larger the value, the less chance of collision occurrence.
     * But 32 (possible combination = 2^(32*8*8)) is just fine
     */
    virtual size_t poolSize() const LASHE_NOEXCEPT;
    virtual thisClass &poolSize(const size_t size) LASHE_NOEXCEPT;

    virtual UUID generate() LASHE_NOEXCEPT override;
    // Using random_device, current timestamp, thread id, and PID.
    virtual thisClass &randomise() LASHE_NOEXCEPT override;
};

LASHE_DECL_EXT bool validate_uuid_string(const char *str,
                                         UUIDException *e = nullptr);

LASHE_DECL_EXT bool validate_uuid_binary(const uint8_t *p,
                                         UUIDException *e = nullptr);

#pragma pack(pop)
}

#endif
