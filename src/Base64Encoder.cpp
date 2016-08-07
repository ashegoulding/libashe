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
	: __ctx(new __Base64EncoderContext)
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
		if(this->__ctx->flag_noNL)
			__lashe_mod_opensslBundle.BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
		__lashe_mod_opensslBundle.BIO_push(b64, bio);

		this->close();
		this->__ctx->b64 = b64;
		this->__ctx->bio = bio;
	}
	catch(FilterException &e)
	{
		if(b64)
			__lashe_mod_opensslBundle.BIO_pop(b64);
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

Base64Encoder& Base64Encoder::param(const char* key, const char* val, const/* ParamFlag */ uint32_t* flags) LASHE_EXCEPT(FilterException)
{
	motherClass::__dropif_nullParam__(key, val);
	if(::strcmp("BIO_FLAGS_BASE64_NO_NL", key) == 0)
	{
		this->__dropif_ready(false);
		if(std::regex_match(val, *__lashe_format_booleanTrue))
			this->__ctx->flag_noNL = true;
		else if(std::regex_match(val, *__lashe_format_booleanFalse))
			this->__ctx->flag_noNL = false;
		else
		{
			FilterException e;
			e.code(FilterException::C_ILLEGAL_ARGUMENT);
			throw e;
		}
		this->__ctx->paramChanged = true;
	}
	else if(::strcmp("URL", key) == 0)
	{
		if(std::regex_match(val, *__lashe_format_booleanTrue))
			this->__ctx->isURL = true;
		else if(std::regex_match(val, *__lashe_format_booleanFalse))
			this->__ctx->isURL = false;
		else
		{
			FilterException e;
			e.code(FilterException::C_ILLEGAL_ARGUMENT);
			throw e;
		}
		this->__ctx->paramChanged = true;
	}
	else
	{
		FilterException e;
		e.code(FilterException::C_NO_SUBJECT);
		throw e;
	}
	return *this;
}

size_t Base64Encoder::param(const char *key, const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags/* = nullptr*/) const LASHE_NOEXCEPT
{
	motherClass::__dropif_nullParam__(key);
	this->__prepParamMap();
	this->__prepParamPool(key);
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

size_t Base64Encoder::param(const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags/* = nullptr*/) const LASHE_NOEXCEPT
{
	this->__prepParamMap();
	this->__prepParamPool();
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

Base64Encoder& Base64Encoder::feed(const void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	this->__dropif_ready(true);
	if(len == 0)
		return *this;
	else if(buf == nullptr)
		motherClass::__dropif_nullBuf__(buf);

	__LASHE_BIO_reset(this->__ctx->bio);
	__lashe_mod_opensslBundle.BIO_write(this->__ctx->b64, buf, len);
	this->__pushed += len;

	return *this;
}

Base64Encoder& Base64Encoder::finish() LASHE_EXCEPT(FilterException)
{
	this->__dropif_ready(true);
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
	this->__pushed = 0;
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

	if(!this->ready())
		return 0;
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
		motherClass::__drop_shortBuf__();
	else
		motherClass::__dropif_nullBuf__(buf);

	size = (size_t)__LASHE_BIO_get_mem_data(this->__ctx->bio, &p);
	::memcpy(buf, p, size);

	if(this->__ctx->isURL)
	{
		size_t i;

		p = (char*)buf;
		for(i=0; i<size; ++i)
		{
			switch(p[i])
			{
			case '+':
				p[i] = '-';
				break;
			case '/':
				p[i] = '_';
				break;
			}
		}
	}

	return *this;
}

void Base64Encoder::__prepParamPool(const char *key/* = nullptr*/) const LASHE_NOEXCEPT
{
	if(key)
	{
		auto r = this->__ctx->paramMap.equal_range(key);
		this->__ctx->entryPool.resize(std::distance(r.first, r.second));
		auto pIt = this->__ctx->entryPool.begin();

		for(; r.first != r.second; r.first++)
		{
			pIt->key = r.first->first.c_str();
			pIt->key = r.first->second.c_str();
			pIt++;
		}
	}
	else
	{
		this->__ctx->entryPool.resize(this->__ctx->paramMap.size());
		auto pIt = this->__ctx->entryPool.begin();

		for(const auto &v : this->__ctx->paramMap)
		{
			auto r = this->__ctx->paramMap.equal_range(v.first);

			for(; r.first != r.second; r.first++)
			{
				pIt->key = r.first->first.c_str();
				pIt->key = r.first->second.c_str();
				pIt++;
			}
		}
	}
}

void Base64Encoder::__prepParamMap() const LASHE_NOEXCEPT
{
	if(!this->__ctx->paramChanged)
		return;
	this->__ctx->paramMap.clear();

	this->__ctx->paramMap.insert(std::make_pair("BIO_FLAGS_BASE64_NO_NL", this->__ctx->flag_noNL? "true" : "false"));
	this->__ctx->paramMap.insert(std::make_pair("URL", this->__ctx->isURL? "true" : "false"));

	this->__ctx->paramChanged = false;
}

}
