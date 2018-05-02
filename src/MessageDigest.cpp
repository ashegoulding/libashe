#include "__openssl.hpp"
#include "libashe/MessageDigest.hpp"

#include <iomanip>
#include <memory>


namespace ashe {

/********************
 * MessageDigest
 */
MessageDigest::MessageDigest() LASHE_NOEXCEPT
    : __pd(new __lashe::MessageDigestPrivData)
{
}

MessageDigest::~MessageDigest() LASHE_NOEXCEPT
{
    this->close();
    delete this->__pd;
}

const char *MessageDigest::className() const LASHE_NOEXCEPT
{
    return "MessageDigest";
}

MessageDigest &MessageDigest::open(const HashAlgorithm x)
{
    const EVP_MD *md;
    EVP_MD_CTX *ctxMD;
    int size;

    __lashe::ensure_ability(LAsheAbility::OPENSSL);

    switch (x) {
    case HashAlgorithm::MD5:
        md = __lashe::openssl->fnp.EVP_md5();
        break;
    case HashAlgorithm::SHA1:
        md = __lashe::openssl->fnp.EVP_sha1();
        break;
    case HashAlgorithm::SHA224:
        md = __lashe::openssl->fnp.EVP_sha224();
        break;
    case HashAlgorithm::SHA256:
        md = __lashe::openssl->fnp.EVP_sha256();
        break;
    case HashAlgorithm::SHA384:
        md = __lashe::openssl->fnp.EVP_sha384();
        break;
    case HashAlgorithm::SHA512:
        md = __lashe::openssl->fnp.EVP_sha512();
        break;
    default:
        throw FilterException("unimplemented HashAlgorithm.");
    }

    ctxMD = __lashe::openssl->fnp.EVP_MD_CTX_new();
    if (nullptr == ctxMD) {
        throw FilterException("EVP_MD_CTX_new() returned null.");
    }
    if ((size = __lashe::openssl->fnp.EVP_MD_size(md)) <= 0) {
        std::stringstream ss;

        ss << "EVP_MD_size() returned: " << size;
        throw FilterException(ss.str().c_str());
    }

    __lashe::openssl->fnp.EVP_DigestInit_ex(ctxMD, md, nullptr);

    this->close();
    this->__pd->ctxMD = ctxMD;
    this->__pd->md = md;
    this->__pd->msgSize = size;

    return *this;
}

MessageDigest &MessageDigest::close() LASHE_NOEXCEPT
{
    if (__lashe::openssl != nullptr) {
        __lashe::openssl->fnp.EVP_MD_CTX_free(this->__pd->ctxMD);
    }

    this->__pd->pushed = 0;
    this->__pd->ctxMD = nullptr;
    this->__pd->md = nullptr;
    this->__pd->msgSize = -1;
    this->__pd->result.free();

    return *this;
}

bool MessageDigest::ready() const LASHE_NOEXCEPT
{
    return this->__pd->ctxMD != nullptr;
}

MessageDigest &MessageDigest::feed(const uint8_t *buf, const size_t len)
{
    this->__dropif_ready(true);

    __lashe::openssl->fnp.EVP_DigestUpdate(this->__pd->ctxMD, buf, len);
    this->__pd->pushed += len;

    return *this;
}

MessageDigest &MessageDigest::finish()
{
    this->__dropif_ready(true);

    this->__pd->result.alloc((size_t)this->__pd->msgSize);
    __lashe::openssl->fnp.EVP_DigestFinal_ex(
        this->__pd->ctxMD, this->__pd->result.data(), nullptr);
    __lashe::openssl->fnp.EVP_DigestInit_ex(this->__pd->ctxMD, this->__pd->md,
                                            nullptr);

    return *this;
}

MessageDigest &MessageDigest::clear() LASHE_NOEXCEPT
{
    this->__pd->result.free();
    return *this;
}

const Buffer &MessageDigest::payload() const LASHE_NOEXCEPT
{
    return this->__pd->result;
}

Buffer &&MessageDigest::payload() LASHE_NOEXCEPT
{
    return std::move(this->__pd->result);
}

/*********************
 * Library Functions
 */
LASHE_DECL_EXT Buffer
digest(const uint8_t *buf, const size_t len, const HashAlgorithm algo,
       const MessageDigestFlags flags /* = MessageDigestFlags::NONE*/)
{
    std::unique_ptr<MessageDigest> md(new MessageDigest());

    md->open(algo);
    md->feed(buf, len).finish();

    return std::move(md->payload());
}

} // namespace ashe
