#ifndef __LASHE_OPENSSL_HPP_
#define __LASHE_OPENSSL_HPP_
#include "__internal.hpp"
#include "libashe/FilterInterface.hpp"

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <map>
#include <vector>


namespace __lashe {

namespace openssl_fn {

typedef BIO *(*BIO_new)(const BIO_METHOD *type);
typedef int (*BIO_free)(BIO *a);
typedef void (*BIO_vfree)(BIO *a);
typedef void (*BIO_free_all)(BIO *a);

typedef BIO *(*BIO_push)(BIO *b, BIO *append);
typedef BIO *(*BIO_pop)(BIO *b);

typedef int (*BIO_read)(BIO *b, void *buf, int len);
typedef int (*BIO_gets)(BIO *b, char *buf, int size);
typedef int (*BIO_write)(BIO *b, const void *buf, int len);
typedef int (*BIO_puts)(BIO *b, const char *buf);

typedef long (*BIO_ctrl)(BIO *bp, int cmd, long larg, void *parg);
typedef long (*BIO_callback_ctrl)(BIO *b, int cmd, bio_info_cb cb);
typedef char *(*BIO_ptr_ctrl)(BIO *bp, int cmd, long larg);
typedef long (*BIO_int_ctrl)(BIO *bp, int cmd, long larg, int iarg);

typedef int (*BIO_reset)(BIO *b);
typedef int (*BIO_seek)(BIO *b, int ofs);
typedef int (*BIO_tell)(BIO *b);
typedef int (*BIO_flush)(BIO *b);
typedef int (*BIO_eof)(BIO *b);
typedef int (*BIO_set_close)(BIO *b, long flag);
typedef int (*BIO_get_close)(BIO *b);
typedef int (*BIO_pending)(BIO *b);
typedef int (*BIO_wpending)(BIO *b);
typedef size_t (*BIO_ctrl_pending)(BIO *b);
typedef size_t (*BIO_ctrl_wpending)(BIO *b);

typedef int (*BIO_get_info_callback)(BIO *b, bio_info_cb **cbp);
typedef int (*BIO_set_info_callback)(BIO *b, bio_info_cb *cb);

typedef const BIO_METHOD *(*BIO_s_mem)(void);
typedef void (*BIO_set_mem_eof_return)(BIO *b, int v);
typedef long (*BIO_get_mem_data)(BIO *b, char **pp);
typedef void (*BIO_set_mem_buf)(BIO *b, BUF_MEM *bm, int c);
typedef void (*BIO_get_mem_ptr)(BIO *b, BUF_MEM **pp);
typedef BIO *(*BIO_new_mem_buf)(const void *buf, int len);

typedef const BIO_METHOD *(*BIO_f_base64)(void);
typedef void (*BIO_set_flags)(BIO *b, int flags);

typedef EVP_MD_CTX *(*EVP_MD_CTX_new)(void);
typedef void (*EVP_MD_CTX_free)(EVP_MD_CTX *ctx);
typedef int (*EVP_DigestInit_ex)(EVP_MD_CTX *ctx, const EVP_MD *type,
                                 ENGINE *impl);
typedef int (*EVP_DigestUpdate)(EVP_MD_CTX *ctx, const void *d, size_t cnt);
typedef int (*EVP_DigestFinal_ex)(EVP_MD_CTX *ctx, unsigned char *md,
                                  unsigned int *s);
typedef int (*EVP_MD_CTX_copy)(EVP_MD_CTX *out, EVP_MD_CTX *in);
typedef int (*EVP_MD_size)(const EVP_MD *md);
typedef const EVP_MD *(*EVP_md5)(void);
typedef const EVP_MD *(*EVP_sha1)(void);
typedef const EVP_MD *(*EVP_sha224)(void);
typedef const EVP_MD *(*EVP_sha256)(void);
typedef const EVP_MD *(*EVP_sha384)(void);
typedef const EVP_MD *(*EVP_sha512)(void);

} // namespace openssl_fn

struct OpenSSLFNP {
    openssl_fn::BIO_new BIO_new = nullptr;
    openssl_fn::BIO_free BIO_free = nullptr;
    openssl_fn::BIO_vfree BIO_vfree = nullptr;
    openssl_fn::BIO_free_all BIO_free_all = nullptr;
    openssl_fn::BIO_push BIO_push = nullptr;
    openssl_fn::BIO_pop BIO_pop = nullptr;
    openssl_fn::BIO_read BIO_read = nullptr;
    openssl_fn::BIO_gets BIO_gets = nullptr;
    openssl_fn::BIO_write BIO_write = nullptr;
    openssl_fn::BIO_puts BIO_puts = nullptr;
    openssl_fn::BIO_ctrl BIO_ctrl = nullptr;
    openssl_fn::BIO_callback_ctrl BIO_callback_ctrl = nullptr;
    openssl_fn::BIO_ptr_ctrl BIO_ptr_ctrl = nullptr;
    openssl_fn::BIO_int_ctrl BIO_int_ctrl = nullptr;
    openssl_fn::BIO_ctrl_pending BIO_ctrl_pending = nullptr;
    openssl_fn::BIO_ctrl_wpending BIO_ctrl_wpending = nullptr;
    openssl_fn::BIO_s_mem BIO_s_mem = nullptr;
    openssl_fn::BIO_new_mem_buf BIO_new_mem_buf = nullptr;
    openssl_fn::BIO_f_base64 BIO_f_base64 = nullptr;
    openssl_fn::BIO_set_flags BIO_set_flags = nullptr;
    openssl_fn::EVP_MD_CTX_new EVP_MD_CTX_new = nullptr;
    openssl_fn::EVP_MD_CTX_free EVP_MD_CTX_free = nullptr;
    openssl_fn::EVP_DigestInit_ex EVP_DigestInit_ex = nullptr;
    openssl_fn::EVP_DigestUpdate EVP_DigestUpdate = nullptr;
    openssl_fn::EVP_DigestFinal_ex EVP_DigestFinal_ex = nullptr;
    openssl_fn::EVP_MD_CTX_copy EVP_MD_CTX_copy = nullptr;
    openssl_fn::EVP_MD_size EVP_MD_size = nullptr;
    openssl_fn::EVP_md5 EVP_md5 = nullptr;
    openssl_fn::EVP_sha1 EVP_sha1 = nullptr;
    openssl_fn::EVP_sha224 EVP_sha224 = nullptr;
    openssl_fn::EVP_sha256 EVP_sha256 = nullptr;
    openssl_fn::EVP_sha384 EVP_sha384 = nullptr;
    openssl_fn::EVP_sha512 EVP_sha512 = nullptr;
};

struct FilterInterfacePrivData {
    size_t pushed = 0;
};

struct Base64PrivData : FilterInterfacePrivData {
    BIO *b64 = nullptr, *bio = nullptr;
    ashe::Base64Type type;
    ashe::Buffer result;
};

struct Base64DecoderPrivData : Base64PrivData {
};

struct Base64EncoderPrivData : Base64PrivData {
};

struct MessageDigestPrivData : FilterInterfacePrivData {
    EVP_MD_CTX *ctxMD = nullptr;
    const EVP_MD *md = nullptr;
    int msgSize = -1;
    ashe::Buffer result;
};

struct OpenSSLGlobal {
    OpenSSLFNP fnp;
    ModuleType mod_ssl = NULL;
    ModuleType mod_crypto = NULL;
};

extern OpenSSLGlobal *openssl;

#define __LASHE_OPENSSL_BIO_seek(b, ofs)                                       \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_C_FILE_SEEK, ofs, NULL)
#define __LASHE_OPENSSL_BIO_tell(b)                                            \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_C_FILE_TELL, 0, NULL)
#define __LASHE_OPENSSL_BIO_reset(b)                                           \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_RESET, 0, NULL)
#define __LASHE_OPENSSL_BIO_eof(b)                                             \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_EOF, 0, NULL)
#define __LASHE_OPENSSL_BIO_set_close(b, c)                                    \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_SET_CLOSE, (c), NULL)
#define __LASHE_OPENSSL_BIO_get_close(b)                                       \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_GET_CLOSE, 0, NULL)
#define __LASHE_OPENSSL_BIO_pending(b)                                         \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_PENDING, 0, NULL)
#define __LASHE_OPENSSL_BIO_wpending(b)                                        \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_WPENDING, 0, NULL)
#define __LASHE_OPENSSL_BIO_flush(b)                                           \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_FLUSH, 0, NULL)
#define __LASHE_OPENSSL_BIO_get_info_callback(b, cbp)                          \
    (int)__lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_GET_CALLBACK, 0, cbp)
#define __LASHE_OPENSSL_BIO_set_info_callback(b, cb)                           \
    (int)__lashe::openssl->fnp.BIO_callback_ctrl(b, BIO_CTRL_SET_CALLBACK, cb)
#define __LASHE_OPENSSL_BIO_get_mem_data(b, pp)                                \
    __lashe::openssl->fnp.BIO_ctrl(b, BIO_CTRL_INFO, 0, (char *)pp)
#define __LASHE_OPENSSL_BIO_set_mem_buf(b, bm, c)                              \
    __lashe::openssl->fnp.BIO_ctrl(b, BIO_C_SET_BUF_MEM, c, (char *)bm)
#define __LASHE_OPENSSL_BIO_get_mem_ptr(b, pp)                                 \
    __lashe::openssl->fnp.BIO_ctrl(b, BIO_C_GET_BUF_MEM_PTR, 0, (char *)pp)
#define __LASHE_OPENSSL_BIO_set_mem_eof_return(b, v)                           \
    __lashe::openssl->fnp.BIO_ctrl(b, BIO_C_SET_BUF_MEM_EOF_RETURN, v, NULL)

void init_openssl();
void deinit_openssl() LASHE_NOEXCEPT;

} // namespace __lashe

#endif
