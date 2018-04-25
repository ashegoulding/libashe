#ifndef LASHE_BASE64ENCODER_HPP_
#define LASHE_BASE64ENCODER_HPP_
#include "FilterInterface.hpp"


namespace __lashe {
struct Base64EncoderPrivData;
}

namespace ashe {
#pragma pack(push, LASHE_PUB_ALIGN)

class Base64Encoder : public FilterInterface, public Class {
public:
    typedef FilterInterface motherClass;
    typedef Base64Encoder thisClass;

protected:
    __lashe::Base64EncoderPrivData *__pd;

    void __popResult() const LASHE_NOEXCEPT;

public:
    Base64Encoder() LASHE_NOEXCEPT;
    Base64Encoder(const thisClass &) = delete;
    Base64Encoder(thisClass &&) = delete;
    virtual ~Base64Encoder() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &) = delete;
    thisClass &operator=(thisClass &&) = delete;

    virtual const char *className() const LASHE_NOEXCEPT override;

    virtual thisClass &open(const Base64Type x);
    virtual thisClass &close() LASHE_NOEXCEPT override;
    virtual bool ready() const LASHE_NOEXCEPT override;
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
LASHE_DECL_EXT Buffer encode_base64(const uint8_t *buf, const size_t len,
                                    const Base64Type type = Base64Type::PLAIN);
}

#endif
