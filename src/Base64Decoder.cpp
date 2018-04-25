#include "__openssl.hpp"
#include "__regex.hpp"
#include "libashe/Base64Decoder.hpp"


namespace ashe {

/********************
 * Base64Decoder
 */
Base64Decoder::Base64Decoder() LASHE_NOEXCEPT
    : __pd(new __lashe::Base64DecoderPrivData)
{
}

Base64Decoder::~Base64Decoder() LASHE_NOEXCEPT
{
    this->close();
    delete this->__pd;
}

const char *Base64Decoder::className() const LASHE_NOEXCEPT
{
    return "Base64Decoder";
}

Base64Decoder &Base64Decoder::open(const Base64Type x)
{
    BIO *b64 = nullptr, *bio = nullptr;

    __lashe::ensure_ability(LAsheAbility::OPENSSL);

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

Base64Decoder &Base64Decoder::close() LASHE_NOEXCEPT
{
    __lashe::openssl->fnp.BIO_free_all(this->__pd->b64);

    this->__pd->b64 = this->__pd->bio = nullptr;
    this->__pd->pushed = 0;
    this->__pd->result.free();

    return *this;
}

bool Base64Decoder::ready() const LASHE_NOEXCEPT
{
    return this->__pd->b64 != nullptr && this->__pd->bio != nullptr;
}

Base64Decoder &Base64Decoder::feed(const char *str, const size_t len)
{
    int ret;
    std::regex *re;
    std::string url_str;
    const char *input_c_str;
    size_t input_len;

    this->__dropif_ready(true);
    if (0 == len) {
        return *this;
    }
    else if (str == nullptr) {
        motherClass::__dropif_nullBuf__(str);
    }

    // Format check.
    if (this->__pd->type == Base64Type::URL) {
        re = &__lashe::regex->fmt_base64_url;
    }
    else {
        re = &__lashe::regex->fmt_base64;
    }

    if (!std::regex_match(str, *re)) {
        throw FilterException("invalid format.");
    }

    if (this->__pd->type == Base64Type::URL) {
        // Transform URL form of base64 to normal one.
        url_str.reserve(len);
        std::for_each(str, str + len, [&url_str](char c) {
            switch (c) {
            case '-':
                c = '+';
                break;
            case '_':
                c = '/';
                break;
            }
            url_str.push_back(c);
        });
        input_c_str = url_str.c_str();
        input_len = url_str.size();
    }
    else {
        input_c_str = str;
        input_len = len;
    }

    __LASHE_OPENSSL_BIO_reset(this->__pd->bio);
    __lashe::openssl->fnp.BIO_write(this->__pd->bio, input_c_str, input_len);
    this->__pd->pushed += len;

    ret = __LASHE_OPENSSL_BIO_pending(this->__pd->bio);
    if (ret < 0) {
        throw FilterException("BIO_pending() returned an error.");
    }
    this->__pd->result.alloc((size_t)ret);
    __lashe::openssl->fnp.BIO_read(this->__pd->b64, this->__pd->result.data(),
                                   this->__pd->result.size());

    return *this;
}

Base64Decoder &Base64Decoder::feed(const uint8_t *buf, const size_t len)
{
    this->feed((const char *)buf, len);
    return *this;
}

Base64Decoder &Base64Decoder::finish()
{
    this->__dropif_ready(true);
    return *this;
}

Base64Decoder &Base64Decoder::clear() LASHE_NOEXCEPT
{
    this->__pd->result.free();

    return *this;
}

const Buffer &Base64Decoder::payload() const LASHE_NOEXCEPT
{
    return this->__pd->result;
}

Buffer &&Base64Decoder::payload() LASHE_NOEXCEPT
{
    return std::move(this->__pd->result);
}

/********************
 * Library Functions
 */

Buffer decode_base64(const uint8_t *buf, const size_t len,
                     const Base64Type type /* = Base64Type::PLAIN*/)
{
    __lashe::ensure_ability(LAsheAbility::OPENSSL);

    {
        Base64Decoder dec;
        return std::move(dec.open(type).feed(buf, len).payload());
    }
}

} // namespace ashe
