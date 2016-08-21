#include "libashe/MessageDigest.h"
#include "__openssl.h"
#include <iomanip>


namespace ashe
{

FilterInterface *mkMessageDigest(const /* HashAlgorithm */ uint32_t algo, const /* MessageDigestFlags */ uint8_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException)
{
	FilterInterface *ret = nullptr;

	__dropif_opensslUninitialised();
	ret = new MessageDigest();
	try
	{
		if(flags)
		{
			while(*flags)
			{
				switch(*flags)
				{
				case LAMDF_UPPER:
					ret->param("upper", "true");
					break;
				}
				++flags;
			}
		}
		ret->open(algo);
	}
	catch(FilterException &e)
	{
		delete ret;
		throw e;
	}
	return ret;
}

FilterResult digest(const void *buf, const size_t len, const /* HashAlgorithm */ uint32_t algo, const /* MessageDigestFlags */ uint8_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException)
{
	FilterInterface *md = mkMessageDigest(algo);
	FilterResult ret;
	bool retString = false;

	try
	{
		if(flags)
		{
			while(*flags)
			{
				switch(*flags)
				{
				case LAMDF_STRING:
					retString = true;
					break;
				}
				++flags;
			}
		}
		md->feed(buf, len).finish();
		if(retString)
		{
			const FilterInterface::ParamEntry *pd;

			if(0 == md->param("string", &pd))
				__die_critical();
			ret.alloc(::strlen(pd->val));
			::memcpy(ret.ptr, pd->val, ret.size);
		}
		else
		{
			ret.alloc(md->payloadSize());
			md->payload(ret.ptr, ret.size);
		}
	}
	catch(FilterException &e)
	{
		delete md;
		throw e;
	}
	delete md;
	return ret;
}

/********************
* MessageDigest
*/
MessageDigest::MessageDigest() LASHE_NOEXCEPT
	: __ctx(new __MessageDigestContext)
{
}

MessageDigest::~MessageDigest() LASHE_NOEXCEPT
{
	this->close();
	delete this->__ctx;
}

const char* MessageDigest::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::MessageDigest";
	return __name__;
}

MessageDigest& MessageDigest::open(const uint32_t x) LASHE_EXCEPT(FilterException)
{
	const EVP_MD *md;
	EVP_MD_CTX *ctxMD;
	int size;

	switch(x)
	{
	case LAHA_MD5: md = __lashe_mod_opensslBundle.EVP_md5(); break;
	case LAHA_SHA1: md = __lashe_mod_opensslBundle.EVP_sha1(); break;
	case LAHA_SHA224: md = __lashe_mod_opensslBundle.EVP_sha224(); break;
	case LAHA_SHA256: md = __lashe_mod_opensslBundle.EVP_sha256(); break;
	case LAHA_SHA384: md = __lashe_mod_opensslBundle.EVP_sha384(); break;
	case LAHA_SHA512: md = __lashe_mod_opensslBundle.EVP_sha512(); break;
	default:
	{
		FilterException e;
		e.code(FilterException::C_NO_SUBJECT);
		throw e;
	}
	}

	ctxMD = __lashe_mod_opensslBundle.EVP_MD_CTX_new();
	if(nullptr == ctxMD)
	{
		FilterException e;
		e
			.code(FilterException::C_ERROR_FROM_LIB)
			.msg("EVP_MD_CTX_new() returned null.");
		throw e;
	}
	if((size = __lashe_mod_opensslBundle.EVP_MD_size(md)) <= 0)
	{
		FilterException e;
		std::stringstream sb;

		sb << "EVP_MD_size() returned: " << size;
		e
			.code(FilterException::C_ERROR_FROM_LIB)
			.msg(sb.str().c_str());
		throw e;
	}
	__lashe_mod_opensslBundle.EVP_DigestInit_ex(ctxMD, md, nullptr);
	this->close();
	this->__ctx->ctxMD = ctxMD;
	this->__ctx->md = md;
	this->__ctx->msgSize = size;

	return *this;
}

MessageDigest& MessageDigest::close() LASHE_NOEXCEPT
{
	__lashe_mod_opensslBundle.EVP_MD_CTX_free(this->__ctx->ctxMD);
	this->__ctx->ctxMD = nullptr;
	this->__ctx->md = nullptr;
	this->__ctx->msgSize = -1;
	return *this;
}

bool MessageDigest::ready() const LASHE_NOEXCEPT
{
	return this->__ctx->ctxMD != nullptr;
}

MessageDigest& MessageDigest::param(const char* key, const char* val, const/* ParamFlag */ uint32_t* flags) LASHE_EXCEPT(FilterException)
{
	motherClass::__dropif_nullParam__(key, val);
	if(::strcmp(key, "upper") == 0)
	{
		if(std::regex_match(val, *__lashe_format_booleanTrue))
			this->__ctx->upper = true;
		else if(std::regex_match(val, *__lashe_format_booleanFalse))
			this->__ctx->upper = false;
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

size_t MessageDigest::param(const char* key, const ParamEntry** arr, const/* ParamFlag */ uint32_t* flags) const LASHE_NOEXCEPT
{
	motherClass::__dropif_nullParam__(key);
	this->__prepParamMap();
	this->__prepParamPool(key);
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

size_t MessageDigest::param(const ParamEntry** arr, const/* ParamFlag */ uint32_t* flags) const LASHE_NOEXCEPT
{
	this->__prepParamMap();
	this->__prepParamPool();
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

MessageDigest& MessageDigest::feed(const void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	this->__dropif_ready(true);
	__lashe_mod_opensslBundle.EVP_DigestUpdate(this->__ctx->ctxMD, buf, len);
	return *this;
}

MessageDigest& MessageDigest::finish() LASHE_EXCEPT(FilterException)
{
	std::vector<uint8_t> ret;

	this->__dropif_ready(true);
	ret.resize((size_t)this->__ctx->msgSize);
	__lashe_mod_opensslBundle.EVP_DigestFinal_ex(this->__ctx->ctxMD, ret.data(), nullptr);
	__lashe_mod_opensslBundle.EVP_DigestInit_ex(this->__ctx->ctxMD, this->__ctx->md, nullptr);
	this->__ctx->result.swap(ret);
	this->__ctx->paramChanged = true;

	return *this;
}

MessageDigest& MessageDigest::clear() LASHE_NOEXCEPT
{
	this->__ctx->result.clear();
	this->__ctx->result.shrink_to_fit();
	return *this;
}

bool MessageDigest::hasPayload() const LASHE_NOEXCEPT
{
	return !this->__ctx->result.empty();
}

size_t MessageDigest::payloadSize() const LASHE_EXCEPT(FilterException)
{
	const auto ret = this->__ctx->result.size();
	if(0 == ret)
	{
		FilterException e;
		e.code(FilterException::C_NO_DATA);
		throw e;
	}
	return ret;
}

MessageDigest& MessageDigest::payload(void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	const auto ret = this->payloadSize();
	if(ret > len)
		motherClass::__drop_shortBuf__();
	else
		motherClass::__dropif_nullBuf__(buf);
	::memcpy(buf, this->__ctx->result.data(), ret);
	return *this;
}

void MessageDigest::__prepParamMap() const LASHE_NOEXCEPT
{
	if(!this->__ctx->paramChanged)
		return;
	this->__ctx->paramMap.clear();
	{
		std::stringstream sb;

		if(this->__ctx->upper)
			sb.setf(std::ios_base::hex | std::ios_base::uppercase);
		else
			sb.setf(std::ios_base::hex);
		for(const auto &v : this->__ctx->result)
			sb << (int)v;

		this->__ctx->paramMap.insert(std::make_pair("string", sb.str()));
	}
	this->__ctx->paramMap.insert(std::make_pair("upper", this->__ctx->upper? "true" : "false"));

	this->__ctx->paramChanged = false;
}

void MessageDigest::__prepParamPool(const char* key) const LASHE_NOEXCEPT
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

}
