#ifndef __LASHE_OPENSSL_H_
#define __LASHE_OPENSSL_H_

#include "__internal.h"

#include <openssl/bio.h>
#include <openssl/evp.h>


namespace ashe
{

namespace pfn
{

typedef const BIO_METHOD* (*BIO_f_base64)(void);

}

struct __OpenSSLModuleBundle
{
	pfn::BIO_f_base64 BIO_f_base64;
};

extern __ModuleType __lashe_mod_openssl;
extern __OpenSSLModuleBundle __lashe_mod_opensslBundle;

}

#endif
