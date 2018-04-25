#include "__openssl.hpp"
#include "libashe/Base64Encoder.hpp"


namespace ashe {

/********************
 * Base64Encoder
 */
Base64Encoder::Base64Encoder() LASHE_NOEXCEPT
    : __pd(new __lashe::Base64EncoderPrivData)
{
}

Base64Encoder::~Base64Encoder() LASHE_NOEXCEPT
{
    this->close();
    delete this->__pd;
}

const char *Base64Encoder::className() const LASHE_NOEXCEPT
{
    return "Base64Encoder";
}

Base64Encoder &Base64Encoder::open(const Base64Type x)
{
    BIO *b64 = nullptr, *bio = nullptr;

    try {
        b64 =
            __lashe::openssl->fnp.BIO_new(__lashe::openssl->fnp.BIO_f_base64());
        if (nullptr == b64) {
            throw FilterException("BIO_f_base64() returned null.");
        }
        bio = __lashe::openssl->fnp.BIO_new(__lashe::openssl->fnp.BIO_s_mem());
        if (nullptr == bio) {
            throw FilterException("BIO_s_mem() returned null.");
        }

        __lashe::openssl->fnp.BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        __lashe::openssl->fnp.BIO_push(b64, bio);

        this->close();
        this->__pd->b64 = b64;
        this->__pd->bio = bio;
        this->__pd->type = x;
    }
    catch (Exception &e) {
        if (b64) {
            __lashe::openssl->fnp.BIO_pop(b64);
        }
        __lashe::openssl->fnp.BIO_free(b64);
        __lashe::openssl->fnp.BIO_free(bio);

        throw e;
    }

    return *this;
}

Base64Encoder &Base64Encoder::close() LASHE_NOEXCEPT
{
    __lashe::openssl->fnp.BIO_free_all(this->__pd->b64);

    this->__pd->b64 = this->__pd->bio = nullptr;
    this->__pd->pushed = 0;
    this->__pd->result.free();

    return *this;
}

bool Base64Encoder::ready() const LASHE_NOEXCEPT
{
    return this->__pd->b64 != nullptr && this->__pd->bio != nullptr;
}

Base64Encoder &Base64Encoder::feed(const uint8_t *buf, const size_t len)
{
    this->__dropif_ready(true);

    if (len == 0) {
        return *this;
    }
    else if (buf == nullptr) {
        motherClass::__dropif_nullBuf__(buf);
    }

    __LASHE_OPENSSL_BIO_reset(this->__pd->bio);
    __lashe::openssl->fnp.BIO_write(this->__pd->b64, buf, len);
    this->__pd->pushed += len;

    return *this;
}

Base64Encoder &Base64Encoder::finish()
{
    this->__dropif_ready(true);
    __LASHE_OPENSSL_BIO_reset(this->__pd->bio);
    __LASHE_OPENSSL_BIO_flush(this->__pd->b64);

    return *this;
}

Base64Encoder &Base64Encoder::clear() LASHE_NOEXCEPT
{
    if (this->ready()) {
        __LASHE_OPENSSL_BIO_reset(this->__pd->b64);
        __LASHE_OPENSSL_BIO_reset(this->__pd->bio);
    }
    this->__pd->result.free();

    return *this;
}

const Buffer &Base64Encoder::payload() const LASHE_NOEXCEPT
{
    this->__popResult();
    return this->__pd->result;
}

Buffer &&Base64Encoder::payload() LASHE_NOEXCEPT
{
    this->__popResult();
    return std::move(this->__pd->result);
}

void Base64Encoder::__popResult() const LASHE_NOEXCEPT
{
    char *p;
    size_t size;

    size = (size_t)__LASHE_OPENSSL_BIO_get_mem_data(this->__pd->bio, &p);
    this->__pd->result.alloc(size);
    std::memcpy(this->__pd->result.data(), p, size);

    // Format to URL base64 string.
    if (this->__pd->type == Base64Type::URL) {
        p = (char *)this->__pd->result.data();
        std::for_each(p, p + size, [](char &c) {
            switch (c) {
            case '+':
                c = '-';
                break;
            case '/':
                c = '_';
                break;
            }
        });
    }
}

/********************
 * Library Functions
 */
Buffer encode_base64(const uint8_t *buf, const size_t len,
                     const Base64Type type /* = Base64Type::PLAIN*/)
{
    __lashe::ensure_ability(LAsheAbility::OPENSSL);

    {
        Base64Encoder enc;
        return enc.open(type).feed(buf, len).finish().payload();
    }
}

} // namespace ashe
