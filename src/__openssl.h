#ifndef __LASHE_OPENSSL_H_
#define __LASHE_OPENSSL_H_

#include "__internal.h"
#include "libashe/FilterInterface.h"

#include <openssl/bio.h>
#include <openssl/evp.h>

#include <map>
#include <vector>


namespace ashe
{

namespace pfn
{

typedef BIO*(*BIO_new)(const BIO_METHOD *type);
typedef int(*BIO_free)(BIO *a);
typedef void(*BIO_vfree)(BIO *a);
typedef void(*BIO_free_all)(BIO *a);

typedef BIO*(*BIO_push)(BIO *b, BIO *append);
typedef BIO*(*BIO_pop)(BIO *b);

typedef int(*BIO_read)(BIO *b, void *buf, int len);
typedef int(*BIO_gets)(BIO *b, char *buf, int size);
typedef int(*BIO_write)(BIO *b, const void *buf, int len);
typedef int(*BIO_puts)(BIO *b, const char *buf);

typedef long(*BIO_ctrl)(BIO *bp, int cmd, long larg, void *parg);
typedef long(*BIO_callback_ctrl)(BIO *b, int cmd, bio_info_cb cb);
typedef char*(*BIO_ptr_ctrl)(BIO *bp, int cmd, long larg);
typedef long(*BIO_int_ctrl)(BIO *bp, int cmd, long larg, int iarg);

typedef int(*BIO_reset)(BIO *b);
typedef int(*BIO_seek)(BIO *b, int ofs);
typedef int(*BIO_tell)(BIO *b);
typedef int(*BIO_flush)(BIO *b);
typedef int(*BIO_eof)(BIO *b);
typedef int(*BIO_set_close)(BIO *b, long flag);
typedef int(*BIO_get_close)(BIO *b);
typedef int(*BIO_pending)(BIO *b);
typedef int(*BIO_wpending)(BIO *b);
typedef size_t(*BIO_ctrl_pending)(BIO *b);
typedef size_t(*BIO_ctrl_wpending)(BIO *b);

typedef int(*BIO_get_info_callback)(BIO *b, bio_info_cb **cbp);
typedef int(*BIO_set_info_callback)(BIO *b, bio_info_cb *cb);

typedef const BIO_METHOD*(*BIO_s_mem)(void);
typedef void(*BIO_set_mem_eof_return)(BIO *b, int v);
typedef long(*BIO_get_mem_data)(BIO *b, char **pp);
typedef void(*BIO_set_mem_buf)(BIO *b, BUF_MEM *bm, int c);
typedef void(*BIO_get_mem_ptr)(BIO *b, BUF_MEM **pp);
typedef BIO*(*BIO_new_mem_buf)(const void *buf, int len);

typedef const BIO_METHOD*(*BIO_f_base64)(void);
typedef void(*BIO_set_flags)(BIO *b, int flags);

typedef EVP_MD_CTX*(*EVP_MD_CTX_new)(void);
typedef void(*EVP_MD_CTX_free)(EVP_MD_CTX *ctx);
typedef int(*EVP_DigestInit_ex)(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
typedef int(*EVP_DigestUpdate)(EVP_MD_CTX *ctx, const void *d, size_t cnt);
typedef int(*EVP_DigestFinal_ex)(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s);
typedef int(*EVP_MD_CTX_copy)(EVP_MD_CTX *out, EVP_MD_CTX *in);
typedef int(*EVP_MD_size)(const EVP_MD *md);
typedef const EVP_MD*(*EVP_md5)(void);
typedef const EVP_MD*(*EVP_sha1)(void);
typedef const EVP_MD*(*EVP_sha224)(void);
typedef const EVP_MD*(*EVP_sha256)(void);
typedef const EVP_MD*(*EVP_sha384)(void);
typedef const EVP_MD*(*EVP_sha512)(void);

}

struct __OpenSSLModuleBundle
{
	pfn::BIO_new BIO_new;
	pfn::BIO_free BIO_free;
	pfn::BIO_vfree BIO_vfree;
	pfn::BIO_free_all BIO_free_all;
	pfn::BIO_push BIO_push;
	pfn::BIO_pop BIO_pop;
	pfn::BIO_read BIO_read;
	pfn::BIO_gets BIO_gets;
	pfn::BIO_write BIO_write;
	pfn::BIO_puts BIO_puts;
	pfn::BIO_ctrl BIO_ctrl;
	pfn::BIO_callback_ctrl BIO_callback_ctrl;
	pfn::BIO_ptr_ctrl BIO_ptr_ctrl;
	pfn::BIO_int_ctrl BIO_int_ctrl;
	pfn::BIO_ctrl_pending BIO_ctrl_pending;
	pfn::BIO_ctrl_wpending BIO_ctrl_wpending;
	pfn::BIO_s_mem BIO_s_mem;
	pfn::BIO_new_mem_buf BIO_new_mem_buf;
	pfn::BIO_f_base64 BIO_f_base64;
	pfn::BIO_set_flags BIO_set_flags;
	pfn::EVP_MD_CTX_new EVP_MD_CTX_new;
	pfn::EVP_MD_CTX_free EVP_MD_CTX_free;
	pfn::EVP_DigestInit_ex EVP_DigestInit_ex;
	pfn::EVP_DigestUpdate EVP_DigestUpdate;
	pfn::EVP_DigestFinal_ex EVP_DigestFinal_ex;
	pfn::EVP_MD_CTX_copy EVP_MD_CTX_copy;
	pfn::EVP_MD_size EVP_MD_size;
	pfn::EVP_md5 EVP_md5;
	pfn::EVP_sha1 EVP_sha1;
	pfn::EVP_sha224 EVP_sha224;
	pfn::EVP_sha256 EVP_sha256;
	pfn::EVP_sha384 EVP_sha384;
	pfn::EVP_sha512 EVP_sha512;
};

struct __FilterInterfaceContext
{
	std::multimap<std::string, std::string> paramMap;
	std::vector<FilterInterface::ParamEntry> entryPool;
	bool paramChanged = false;
};

struct __Base64EncoderContext : __FilterInterfaceContext
{
	BIO *b64 = nullptr, *bio = nullptr;
	bool flag_noNL = false;
	bool isURL = false;
};

struct __Base64DecoderContext : __FilterInterfaceContext
{
	BIO *b64 = nullptr, *bio = nullptr;
	bool isURL = false;
	std::vector<uint8_t> result;
	size_t resultSize;
};

struct __MessageDigestContext : __FilterInterfaceContext
{
	EVP_MD_CTX *ctxMD = nullptr;
	const EVP_MD *md = nullptr;
	std::vector<uint8_t> result;
	int msgSize = -1;
	bool upper = false;
};

extern bool __lashe_inited_openssl;
extern __OpenSSLModuleBundle __lashe_mod_opensslBundle;

#define __LASHE_BIO_seek(b,ofs)					(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_C_FILE_SEEK,ofs,NULL)
#define __LASHE_BIO_tell(b)						(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_C_FILE_TELL,0,NULL)
#define __LASHE_BIO_reset(b)					(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_RESET,0,NULL)
#define __LASHE_BIO_eof(b)						(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_EOF,0,NULL)
#define __LASHE_BIO_set_close(b,c)				(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_SET_CLOSE,(c),NULL)
#define __LASHE_BIO_get_close(b)				(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_GET_CLOSE,0,NULL)
#define __LASHE_BIO_pending(b)					(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_PENDING,0,NULL)
#define __LASHE_BIO_wpending(b)					(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_WPENDING,0,NULL)
#define __LASHE_BIO_flush(b)					(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_FLUSH,0,NULL)
#define __LASHE_BIO_get_info_callback(b,cbp)	(int)__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_GET_CALLBACK,0, cbp)
#define __LASHE_BIO_set_info_callback(b,cb)		(int)__lashe_mod_opensslBundle.BIO_callback_ctrl(b,BIO_CTRL_SET_CALLBACK,cb)
#define __LASHE_BIO_get_mem_data(b,pp)			__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_CTRL_INFO,0,(char *)pp)
#define __LASHE_BIO_set_mem_buf(b,bm,c)			__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_C_SET_BUF_MEM,c,(char *)bm)
#define __LASHE_BIO_get_mem_ptr(b,pp)			__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_C_GET_BUF_MEM_PTR,0,(char *)pp)
#define __LASHE_BIO_set_mem_eof_return(b,v)		__lashe_mod_opensslBundle.BIO_ctrl(b,BIO_C_SET_BUF_MEM_EOF_RETURN,v,NULL)

void __initOpenSSL() LASHE_EXCEPT(HelperException);
void __deinitOpenSSL() LASHE_NOEXCEPT;
void __dropif_opensslUninitialised() LASHE_EXCEPT(FilterException);

}

#endif
