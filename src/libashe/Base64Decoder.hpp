#ifndef LASHE_BASE64DECODER_HPP_
#define LASHE_BASE64DECODER_HPP_
#include "FilterInterface.hpp"


namespace __lashe {
struct Base64DecoderPrivData;
}

namespace ashe {
#pragma pack(push, LASHE_PUB_ALIGN)

class Base64Decoder : public FilterInterface, public Class {
public:
    typedef FilterInterface motherClass;
    typedef Base64Decoder thisClass;

protected:
    __lashe::Base64DecoderPrivData *__pd;

public:
    Base64Decoder() LASHE_NOEXCEPT;
    Base64Decoder(const thisClass &) = delete;
    Base64Decoder(thisClass &&) = delete;
    virtual ~Base64Decoder() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &) = delete;
    thisClass &operator=(thisClass &&) = delete;

    virtual const char *className() const LASHE_NOEXCEPT override;

    virtual thisClass &open(const Base64Type x);
    virtual thisClass &close() LASHE_NOEXCEPT override;
    virtual bool ready() const LASHE_NOEXCEPT override;

    virtual thisClass &feed(const char *str, const size_t len);
    virtual thisClass &feed(const uint8_t *buf, const size_t len) override;
    virtual thisClass &finish() override;
    virtual thisClass &clear() LASHE_NOEXCEPT override;
    virtual const Buffer &payload() const LASHE_NOEXCEPT override;
    virtual Buffer &&payload() LASHE_NOEXCEPT override;
};

#pragma pack(pop)

/********************
 * Library Functions
 */

/* in-memory base64 decode function.
 * Use of this function is discouraged for larger amount of data.
 */
LASHE_DECL_EXT Buffer decode_base64(const uint8_t *buf, const size_t len,
                                    const Base64Type type = Base64Type::PLAIN);
}

#endif
