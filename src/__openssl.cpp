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

		__lashe_mod_opensslBundle.BIO_new = __loadFunc__<pfn::BIO_new>(__lashe_mod_openssl, "BIO_new");
		__lashe_mod_opensslBundle.BIO_free = __loadFunc__<pfn::BIO_free>(__lashe_mod_openssl, "BIO_free");
		__lashe_mod_opensslBundle.BIO_vfree = __loadFunc__<pfn::BIO_vfree>(__lashe_mod_crypto, "BIO_vfree");
		__lashe_mod_opensslBundle.BIO_free_all = __loadFunc__<pfn::BIO_free_all>(__lashe_mod_openssl, "BIO_free_all");
		__lashe_mod_opensslBundle.BIO_push = __loadFunc__<pfn::BIO_push>(__lashe_mod_openssl, "BIO_push");
		__lashe_mod_opensslBundle.BIO_pop = __loadFunc__<pfn::BIO_pop>(__lashe_mod_openssl, "BIO_pop");
		__lashe_mod_opensslBundle.BIO_read = __loadFunc__<pfn::BIO_read>(__lashe_mod_openssl, "BIO_read");
		__lashe_mod_opensslBundle.BIO_gets = __loadFunc__<pfn::BIO_gets>(__lashe_mod_crypto, "BIO_gets");
		__lashe_mod_opensslBundle.BIO_write = __loadFunc__<pfn::BIO_write>(__lashe_mod_openssl, "BIO_write");
		__lashe_mod_opensslBundle.BIO_puts = __loadFunc__<pfn::BIO_puts>(__lashe_mod_openssl, "BIO_puts");
		__lashe_mod_opensslBundle.BIO_ctrl = __loadFunc__<pfn::BIO_ctrl>(__lashe_mod_openssl, "BIO_ctrl");
		__lashe_mod_opensslBundle.BIO_callback_ctrl = __loadFunc__<pfn::BIO_callback_ctrl>(__lashe_mod_openssl, "BIO_callback_ctrl");
		__lashe_mod_opensslBundle.BIO_ptr_ctrl = __loadFunc__<pfn::BIO_ptr_ctrl>(__lashe_mod_crypto, "BIO_ptr_ctrl");
		__lashe_mod_opensslBundle.BIO_int_ctrl = __loadFunc__<pfn::BIO_int_ctrl>(__lashe_mod_openssl, "BIO_int_ctrl");
		__lashe_mod_opensslBundle.BIO_ctrl_pending = __loadFunc__<pfn::BIO_ctrl_pending>(__lashe_mod_crypto, "BIO_ctrl_pending");
		__lashe_mod_opensslBundle.BIO_ctrl_wpending = __loadFunc__<pfn::BIO_ctrl_wpending>(__lashe_mod_crypto, "BIO_ctrl_wpending");
		__lashe_mod_opensslBundle.BIO_s_mem = __loadFunc__<pfn::BIO_s_mem>(__lashe_mod_openssl, "BIO_s_mem");
		__lashe_mod_opensslBundle.BIO_new_mem_buf = __loadFunc__<pfn::BIO_new_mem_buf>(__lashe_mod_crypto, "BIO_new_mem_buf");
		__lashe_mod_opensslBundle.BIO_f_base64 = __loadFunc__<pfn::BIO_f_base64>(__lashe_mod_crypto, "BIO_f_base64");
		__lashe_mod_opensslBundle.BIO_set_flags = __loadFunc__<pfn::BIO_set_flags>(__lashe_mod_openssl, "BIO_set_flags");

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
