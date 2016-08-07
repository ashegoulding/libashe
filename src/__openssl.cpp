#include "__openssl.h"
#include "libashe/helper.h"

namespace ashe
{

bool __lashe_inited_openssl = false;
__OpenSSLModuleBundle __lashe_mod_opensslBundle;
static __ModuleType __lashe_mod_openssl = NULL;
static __ModuleType __lashe_mod_crypto = NULL;


void __initOpenSSL() LASHE_EXCEPT(HelperException)
{
	if(__lashe_inited_openssl)
		return;

	try
	{
		__lashe_mod_openssl = __loadModule__("libssl.so");
		__lashe_mod_crypto = __loadModule__("libcrypto.so");

		__lashe_mod_opensslBundle.BIO_new = __loadFunc__<pfn::BIO_new>(__lashe_mod_openssl, 1, "BIO_new");
		__lashe_mod_opensslBundle.BIO_free = __loadFunc__<pfn::BIO_free>(__lashe_mod_openssl, 1, "BIO_free");
		__lashe_mod_opensslBundle.BIO_vfree = __loadFunc__<pfn::BIO_vfree>(__lashe_mod_crypto, 1, "BIO_vfree");
		__lashe_mod_opensslBundle.BIO_free_all = __loadFunc__<pfn::BIO_free_all>(__lashe_mod_openssl, 1, "BIO_free_all");
		__lashe_mod_opensslBundle.BIO_push = __loadFunc__<pfn::BIO_push>(__lashe_mod_openssl, 1, "BIO_push");
		__lashe_mod_opensslBundle.BIO_pop = __loadFunc__<pfn::BIO_pop>(__lashe_mod_openssl, 1, "BIO_pop");
		__lashe_mod_opensslBundle.BIO_read = __loadFunc__<pfn::BIO_read>(__lashe_mod_openssl, 1, "BIO_read");
		__lashe_mod_opensslBundle.BIO_gets = __loadFunc__<pfn::BIO_gets>(__lashe_mod_crypto, 1, "BIO_gets");
		__lashe_mod_opensslBundle.BIO_write = __loadFunc__<pfn::BIO_write>(__lashe_mod_openssl, 1, "BIO_write");
		__lashe_mod_opensslBundle.BIO_puts = __loadFunc__<pfn::BIO_puts>(__lashe_mod_openssl, 1, "BIO_puts");
		__lashe_mod_opensslBundle.BIO_ctrl = __loadFunc__<pfn::BIO_ctrl>(__lashe_mod_openssl, 1, "BIO_ctrl");
		__lashe_mod_opensslBundle.BIO_callback_ctrl = __loadFunc__<pfn::BIO_callback_ctrl>(__lashe_mod_openssl, 1, "BIO_callback_ctrl");
		__lashe_mod_opensslBundle.BIO_ptr_ctrl = __loadFunc__<pfn::BIO_ptr_ctrl>(__lashe_mod_crypto, 1, "BIO_ptr_ctrl");
		__lashe_mod_opensslBundle.BIO_int_ctrl = __loadFunc__<pfn::BIO_int_ctrl>(__lashe_mod_openssl, 1, "BIO_int_ctrl");
		__lashe_mod_opensslBundle.BIO_ctrl_pending = __loadFunc__<pfn::BIO_ctrl_pending>(__lashe_mod_crypto, 1, "BIO_ctrl_pending");
		__lashe_mod_opensslBundle.BIO_ctrl_wpending = __loadFunc__<pfn::BIO_ctrl_wpending>(__lashe_mod_crypto, 1, "BIO_ctrl_wpending");
		__lashe_mod_opensslBundle.BIO_s_mem = __loadFunc__<pfn::BIO_s_mem>(__lashe_mod_openssl, 1, "BIO_s_mem");
		__lashe_mod_opensslBundle.BIO_new_mem_buf = __loadFunc__<pfn::BIO_new_mem_buf>(__lashe_mod_crypto, 1, "BIO_new_mem_buf");
		__lashe_mod_opensslBundle.BIO_f_base64 = __loadFunc__<pfn::BIO_f_base64>(__lashe_mod_crypto, 1, "BIO_f_base64");
		__lashe_mod_opensslBundle.BIO_set_flags = __loadFunc__<pfn::BIO_set_flags>(__lashe_mod_openssl, 1, "BIO_set_flags");
		__lashe_mod_opensslBundle.EVP_MD_CTX_new = __loadFunc__<pfn::EVP_MD_CTX_new>(__lashe_mod_openssl, 2, "EVP_MD_CTX_new", "EVP_MD_CTX_create");
		__lashe_mod_opensslBundle.EVP_MD_CTX_free = __loadFunc__<pfn::EVP_MD_CTX_free>(__lashe_mod_openssl, 2, "EVP_MD_CTX_free", "EVP_MD_CTX_destroy");
		__lashe_mod_opensslBundle.EVP_DigestInit_ex = __loadFunc__<pfn::EVP_DigestInit_ex>(__lashe_mod_openssl, 1, "EVP_DigestInit_ex");
		__lashe_mod_opensslBundle.EVP_DigestUpdate = __loadFunc__<pfn::EVP_DigestUpdate>(__lashe_mod_openssl, 1, "EVP_DigestUpdate");
		__lashe_mod_opensslBundle.EVP_DigestFinal_ex = __loadFunc__<pfn::EVP_DigestFinal_ex>(__lashe_mod_openssl, 1, "EVP_DigestFinal_ex");
		__lashe_mod_opensslBundle.EVP_MD_CTX_copy = __loadFunc__<pfn::EVP_MD_CTX_copy>(__lashe_mod_openssl, 1, "EVP_MD_CTX_copy");
		__lashe_mod_opensslBundle.EVP_MD_size = __loadFunc__<pfn::EVP_MD_size>(__lashe_mod_openssl, 1, "EVP_MD_size");
		__lashe_mod_opensslBundle.EVP_md5 = __loadFunc__<pfn::EVP_md5>(__lashe_mod_openssl, 1, "EVP_md5");
		__lashe_mod_opensslBundle.EVP_sha1 = __loadFunc__<pfn::EVP_sha1>(__lashe_mod_openssl, 1, "EVP_sha1");
		__lashe_mod_opensslBundle.EVP_sha224 = __loadFunc__<pfn::EVP_sha224>(__lashe_mod_openssl, 1, "EVP_sha224");
		__lashe_mod_opensslBundle.EVP_sha256 = __loadFunc__<pfn::EVP_sha256>(__lashe_mod_openssl, 1, "EVP_sha256");
		__lashe_mod_opensslBundle.EVP_sha384 = __loadFunc__<pfn::EVP_sha384>(__lashe_mod_openssl, 1, "EVP_sha384");
		__lashe_mod_opensslBundle.EVP_sha512 = __loadFunc__<pfn::EVP_sha512>(__lashe_mod_openssl, 1, "EVP_sha512");

		__lashe_inited_openssl = true;
	}
	catch(HelperException &e)
	{
		__deinitOpenSSL();
		throw e;
	}
}

void __deinitOpenSSL() LASHE_NOEXCEPT
{
	__unloadModule__(&__lashe_mod_openssl);
	__unloadModule__(&__lashe_mod_crypto);
	::memset(&__lashe_mod_opensslBundle, 0, sizeof(__OpenSSLModuleBundle));
	__lashe_inited_openssl = false;
}

void __dropif_opensslUninitialised() LASHE_EXCEPT(FilterException)
{
	if(!__lashe_inited_openssl)
	{
		FilterException e;
		e.code(FilterException::C_ABILITY_UNINITIALISED);
		throw e;
	}
}

}
