#include "__openssl.hpp"


namespace __lashe {

OpenSSLGlobal *openssl = nullptr;

void init_openssl()
{
    if (openssl != nullptr) {
        die_critical();
    }

    openssl = new OpenSSLGlobal();
    try {

        openssl->mod_ssl = load_module("libssl.so");
        openssl->mod_crypto = load_module("libcrypto.so");

        openssl->fnp.BIO_new =
            load_func<openssl_fn::BIO_new>(openssl->mod_ssl, 1, "BIO_new");
        openssl->fnp.BIO_free =
            load_func<openssl_fn::BIO_free>(openssl->mod_ssl, 1, "BIO_free");
        openssl->fnp.BIO_vfree = load_func<openssl_fn::BIO_vfree>(
            openssl->mod_crypto, 1, "BIO_vfree");
        openssl->fnp.BIO_free_all = load_func<openssl_fn::BIO_free_all>(
            openssl->mod_ssl, 1, "BIO_free_all");
        openssl->fnp.BIO_push =
            load_func<openssl_fn::BIO_push>(openssl->mod_ssl, 1, "BIO_push");
        openssl->fnp.BIO_pop =
            load_func<openssl_fn::BIO_pop>(openssl->mod_ssl, 1, "BIO_pop");
        openssl->fnp.BIO_read =
            load_func<openssl_fn::BIO_read>(openssl->mod_ssl, 1, "BIO_read");
        openssl->fnp.BIO_gets =
            load_func<openssl_fn::BIO_gets>(openssl->mod_crypto, 1, "BIO_gets");
        openssl->fnp.BIO_write =
            load_func<openssl_fn::BIO_write>(openssl->mod_ssl, 1, "BIO_write");
        openssl->fnp.BIO_puts =
            load_func<openssl_fn::BIO_puts>(openssl->mod_ssl, 1, "BIO_puts");
        openssl->fnp.BIO_ctrl =
            load_func<openssl_fn::BIO_ctrl>(openssl->mod_ssl, 1, "BIO_ctrl");
        openssl->fnp.BIO_callback_ctrl =
            load_func<openssl_fn::BIO_callback_ctrl>(openssl->mod_ssl, 1,
                                                     "BIO_callback_ctrl");
        openssl->fnp.BIO_ptr_ctrl = load_func<openssl_fn::BIO_ptr_ctrl>(
            openssl->mod_crypto, 1, "BIO_ptr_ctrl");
        openssl->fnp.BIO_int_ctrl = load_func<openssl_fn::BIO_int_ctrl>(
            openssl->mod_ssl, 1, "BIO_int_ctrl");
        openssl->fnp.BIO_ctrl_pending = load_func<openssl_fn::BIO_ctrl_pending>(
            openssl->mod_crypto, 1, "BIO_ctrl_pending");
        openssl->fnp.BIO_ctrl_wpending =
            load_func<openssl_fn::BIO_ctrl_wpending>(openssl->mod_crypto, 1,
                                                     "BIO_ctrl_wpending");
        openssl->fnp.BIO_s_mem =
            load_func<openssl_fn::BIO_s_mem>(openssl->mod_ssl, 1, "BIO_s_mem");
        openssl->fnp.BIO_new_mem_buf = load_func<openssl_fn::BIO_new_mem_buf>(
            openssl->mod_crypto, 1, "BIO_new_mem_buf");
        openssl->fnp.BIO_f_base64 = load_func<openssl_fn::BIO_f_base64>(
            openssl->mod_crypto, 1, "BIO_f_base64");
        openssl->fnp.BIO_set_flags = load_func<openssl_fn::BIO_set_flags>(
            openssl->mod_ssl, 1, "BIO_set_flags");
        openssl->fnp.EVP_MD_CTX_new = load_func<openssl_fn::EVP_MD_CTX_new>(
            openssl->mod_ssl, 2, "EVP_MD_CTX_new", "EVP_MD_CTX_create");
        openssl->fnp.EVP_MD_CTX_free = load_func<openssl_fn::EVP_MD_CTX_free>(
            openssl->mod_ssl, 2, "EVP_MD_CTX_free", "EVP_MD_CTX_destroy");
        openssl->fnp.EVP_DigestInit_ex =
            load_func<openssl_fn::EVP_DigestInit_ex>(openssl->mod_ssl, 1,
                                                     "EVP_DigestInit_ex");
        openssl->fnp.EVP_DigestUpdate = load_func<openssl_fn::EVP_DigestUpdate>(
            openssl->mod_ssl, 1, "EVP_DigestUpdate");
        openssl->fnp.EVP_DigestFinal_ex =
            load_func<openssl_fn::EVP_DigestFinal_ex>(openssl->mod_ssl, 1,
                                                      "EVP_DigestFinal_ex");
        openssl->fnp.EVP_MD_CTX_copy = load_func<openssl_fn::EVP_MD_CTX_copy>(
            openssl->mod_ssl, 1, "EVP_MD_CTX_copy");
        openssl->fnp.EVP_MD_size = load_func<openssl_fn::EVP_MD_size>(
            openssl->mod_ssl, 1, "EVP_MD_size");
        openssl->fnp.EVP_md5 =
            load_func<openssl_fn::EVP_md5>(openssl->mod_ssl, 1, "EVP_md5");
        openssl->fnp.EVP_sha1 =
            load_func<openssl_fn::EVP_sha1>(openssl->mod_ssl, 1, "EVP_sha1");
        openssl->fnp.EVP_sha224 = load_func<openssl_fn::EVP_sha224>(
            openssl->mod_ssl, 1, "EVP_sha224");
        openssl->fnp.EVP_sha256 = load_func<openssl_fn::EVP_sha256>(
            openssl->mod_ssl, 1, "EVP_sha256");
        openssl->fnp.EVP_sha384 = load_func<openssl_fn::EVP_sha384>(
            openssl->mod_ssl, 1, "EVP_sha384");
        openssl->fnp.EVP_sha512 = load_func<openssl_fn::EVP_sha512>(
            openssl->mod_ssl, 1, "EVP_sha512");
    }
    catch (ashe::ModuleException &e) {
        deinit_openssl();
        throw e;
    }
}

void deinit_openssl() LASHE_NOEXCEPT
{
    if (openssl == nullptr) {
        return;
    }

    unload_module(openssl->mod_ssl);
    unload_module(openssl->mod_crypto);
    delete openssl;
    openssl = nullptr;
}

} // namespace __lashe
