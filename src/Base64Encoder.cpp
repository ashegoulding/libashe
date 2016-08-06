#include "libashe/Base64Encoder.h"
#include "__openssl.h"

namespace ashe
{

FilterInterface *mkBase64Encoder() LASHE_EXCEPT(FilterException)
{
	FilterInterface *ret;

	__dropif_opensslUninitialised();
	ret = new Base64Encoder();
	try
	{
		ret->open(0);
	}
	catch(FilterException &e)
	{
		delete ret;
		throw e;
	}

	return ret;
}

/********************
* Base64Encoder
*/
Base64Encoder::Base64Encoder() LASHE_NOEXCEPT
	: __ctx(new __Base64CodecContext)
{
}

Base64Encoder::~Base64Encoder() LASHE_NOEXCEPT
{
	this->close();
	delete this->__ctx;
}

Base64Encoder& Base64Encoder::open(const uint32_t x) LASHE_EXCEPT(FilterException)
{
	BIO *b64 = nullptr, *bio = nullptr;

	try
	{
		b64 = __lashe_mod_opensslBundle.BIO_new(__lashe_mod_opensslBundle.BIO_f_base64());
		bio = __lashe_mod_opensslBundle.BIO_new(__lashe_mod_opensslBundle.BIO_s_mem());
		if(nullptr == b64)
		{
			FilterException e;
			e
				.code(FilterException::C_ERROR_FROM_LIB)
				.msg("BIO_f_base64() returned null.");
			throw e;
		}
		if(nullptr == bio)
		{
			FilterException e;
			e
			.code(FilterException::C_ERROR_FROM_LIB)
			.msg("BIO_s_mem() returned null.");
			throw e;
		}
		__lashe_mod_opensslBundle.BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
		__lashe_mod_opensslBundle.BIO_push(b64, bio);

		this->__ctx->b64 = b64;
		this->__ctx->bio = bio;
	}
	catch(FilterException &e)
	{
		__lashe_mod_opensslBundle.BIO_free(b64);
		__lashe_mod_opensslBundle.BIO_free(bio);
		throw e;
	}
	return *this;
}

Base64Encoder& Base64Encoder::close() LASHE_NOEXCEPT
{
	__lashe_mod_opensslBundle.BIO_free_all(this->__ctx->b64);
	this->__ctx->b64 = this->__ctx->bio = nullptr;
	this->__pushed = 0;
	return *this;
}

bool Base64Encoder::ready() const LASHE_NOEXCEPT
{
	return this->__ctx->b64 && this->__ctx->bio;
}

Base64Encoder& Base64Encoder::feed(const void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	this->__dropif_notReady();
	if(len == 0)
		return *this;
	else if(buf == nullptr)
	{
		FilterException e;
		e
			.code(FilterException::C_ILLEGAL_ARGUMENT)
			.msg("'buf' is given null.");
		throw e;
	}

	__LASHE_BIO_reset(this->__ctx->bio);
	__lashe_mod_opensslBundle.BIO_write(this->__ctx->b64, buf, len);
	this->__pushed += len;

	return *this;
}

Base64Encoder& Base64Encoder::finish() LASHE_EXCEPT(FilterException)
{
	this->__dropif_notReady();
	__LASHE_BIO_flush(this->__ctx->b64);
	return *this;
}

Base64Encoder& Base64Encoder::clear() LASHE_NOEXCEPT
{
	if(this->ready())
	{
		__LASHE_BIO_reset(this->__ctx->b64);
		__LASHE_BIO_reset(this->__ctx->bio);
	}
	return *this;
}

bool Base64Encoder::hasPayload() const LASHE_NOEXCEPT
{
	if(this->ready())
	{
		char *p;
		return __LASHE_BIO_get_mem_data(this->__ctx->bio, &p) != 0;
	}
	return false;
}

size_t Base64Encoder::payloadSize() const LASHE_EXCEPT(FilterException)
{
	size_t ret;
	char *p;

	this->__dropif_notReady();
	ret = (size_t)__LASHE_BIO_get_mem_data(this->__ctx->bio, &p);
	if(0 == ret)
	{
		FilterException e;
		e.code(FilterException::C_NO_DATA);
		throw e;
	}

	return ret;
}

Base64Encoder& Base64Encoder::payload(void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	char *p;
	size_t size;

	if(this->payloadSize() > len)
	{
		FilterException e;
		e.code(FilterException::C_SHORT_BUFFER);
		throw e;
	}
	else if(nullptr == buf)
	{
		FilterException e;
		e
		.code(FilterException::C_ILLEGAL_ARGUMENT)
		.msg("'buf' is given null.");
		throw e;
	}

	size = (size_t)__LASHE_BIO_get_mem_data(this->__ctx->bio, &p);
	::memcpy(buf, p, size);

	return *this;
}

}
