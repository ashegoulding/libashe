#ifndef LASHE_MESSAGEDIGEST_HPP_
#define LASHE_MESSAGEDIGEST_HPP_
#include "FilterInterface.hpp"


namespace __lashe {
struct MessageDigestPrivData;
}

namespace ashe {

enum class MessageDigestFlags {
    NONE = 0x00,
    // Instruct digest() to return a hash string.
    STRING = 0x01,
    UPPER = 0x02
};

#pragma pack(push, LASHE_PUB_ALIGN)

class MessageDigest : public FilterInterface, public Class {
public:
    typedef FilterInterface motherClass;
    typedef MessageDigest thisClass;

protected:
    __lashe::MessageDigestPrivData *__pd;

public:
    MessageDigest() LASHE_NOEXCEPT;
    MessageDigest(const thisClass &) = delete;
    MessageDigest(thisClass &&) = delete;
    virtual ~MessageDigest() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &) = delete;
    thisClass &operator=(thisClass &&) = delete;

    virtual const char *className() const LASHE_NOEXCEPT override;

    virtual thisClass &open(const HashAlgorithm x);
    virtual thisClass &close() LASHE_NOEXCEPT override;
    virtual bool ready() const LASHE_NOEXCEPT override;
    virtual thisClass &feed(const uint8_t *buf, const size_t len) override;
    virtual thisClass &finish() override;
    virtual thisClass &clear() LASHE_NOEXCEPT override;
    virtual const Buffer &payload() const LASHE_NOEXCEPT override;
    virtual Buffer &&payload() LASHE_NOEXCEPT override;
};

#pragma pack(pop)

/*********************
 * Library Functions
 */
// In-memory.
LASHE_DECL_EXT Buffer
digest(const uint8_t *buf, const size_t len, const HashAlgorithm algo,
       const MessageDigestFlags flags = MessageDigestFlags::NONE);

} // namespace ashe

#endif
