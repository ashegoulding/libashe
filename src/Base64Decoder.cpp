#include "libashe/Base64Decoder.h"
#include "__openssl.h"


namespace ashe
{

FilterInterface* mkBase64Decoder(const /* Base64DecodeFlags */uint32_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException)
{
	FilterInterface *ret;

	__dropif_opensslUninitialised();
	ret = new Base64Decoder();
	try
	{
		if(flags)
		{
			while(*flags)
			{
				switch(*flags)
				{
				case LB64DF_URL:
					ret->param("URL", "true");
					break;
				}
				++flags;
			}
		}
		ret->open(0);
	}
	catch(FilterException &e)
	{
		delete ret;
		throw e;
	}

	return ret;
}

FilterResult base64Decode(const void *buf, const size_t len, const /* Base64EncodeFlags */ uint32_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException)
{
	FilterInterface *dec = mkBase64Decoder(flags);
	FilterResult ret;

	try
	{
		dec->feed(buf, len);
		if(dec->hasPayload())
		{
			ret.alloc(dec->payloadSize());
			dec->payload(ret.ptr, ret.size);
		}
	}
	catch(FilterException &e)
	{
		delete dec;
		throw e;
	}
	delete dec;
	return ret;
}

/********************
* Base64Decoder
*/
Base64Decoder::Base64Decoder() LASHE_NOEXCEPT
	: __ctx(new __Base64DecoderContext)
{
}

Base64Decoder::~Base64Decoder() LASHE_NOEXCEPT
{
	this->close();
	delete this->__ctx;
}

const char* Base64Decoder::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::Base64Decoder";
	return __name__;
}

Base64Decoder& Base64Decoder::open(const uint32_t x) LASHE_EXCEPT(FilterException)
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

Base64Decoder& Base64Decoder::close() LASHE_NOEXCEPT
{
	__lashe_mod_opensslBundle.BIO_free_all(this->__ctx->b64);
	this->__ctx->b64 = this->__ctx->bio = nullptr;
	this->__pushed = 0;
	return *this;
}

bool Base64Decoder::ready() const LASHE_NOEXCEPT
{
	return this->__ctx->b64 && this->__ctx->bio;
}

Base64Decoder& Base64Decoder::param(const char* key, const char* val, const /* ParamFlag */ uint32_t* flags) LASHE_EXCEPT(FilterException)
{
	motherClass::__dropif_nullParam__(key, val);
	if(::strcmp(key, "URL") == 0)
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

size_t Base64Decoder::param(const char* key, const ParamEntry** arr, const/* ParamFlag */ uint32_t* flags) const LASHE_NOEXCEPT
{
	motherClass::__dropif_nullParam__(key);
	this->__prepParamMap();
	this->__prepParamPool(key);
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

size_t Base64Decoder::param(const ParamEntry** arr, const/* ParamFlag */ uint32_t* flags) const LASHE_NOEXCEPT
{
	this->__prepParamMap();
	this->__prepParamPool();
	if(arr)
		*arr = this->__ctx->entryPool.data();
	return this->__ctx->entryPool.size();
}

Base64Decoder& Base64Decoder::feed(const void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	std::string val;
	int ret;

	this->__dropif_ready(true);
	if(0 == len)
		return *this;
	else if(buf == nullptr)
		motherClass::__dropif_nullBuf__(buf);

	// Check validity.
	val.assign((char*)buf, len);
	if(this->__ctx->isURL)
	{
		if(!std::regex_match(val, *__lashe_format_base64url))
		{
			FilterException e;
			e.code(FilterException::C_INVALID_FORMAT);
			throw e;
		}
	}
	else
	{
		if(!std::regex_match(val, *__lashe_format_base64))
		{
			FilterException e;
			e.code(FilterException::C_INVALID_FORMAT);
			throw e;
		}
	}
	{
		// Filter out unnecessary characters and convert URL characters.
		std::string::iterator it = val.begin();
		std::string::iterator itEnd = val.end();
		std::string::iterator tmp;

		while(itEnd != it)
		{
			switch(*it)
			{
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '+':
			case '/':
			case '=':
				it++;
				break;
			case '-':
				*it = '+';
				it++;
				break;
			case '_':
				*it = '/';
				it++;
				break;
			default:
				tmp = it + 1;
				val.erase(it);
				it = tmp;
				itEnd = val.end();
			}
		}
	}

	__LASHE_BIO_reset(this->__ctx->bio);
	__lashe_mod_opensslBundle.BIO_write(this->__ctx->bio, val.data(), val.size());
	this->__pushed += len;

	this->__ctx->result.resize(val.size());
	ret = __lashe_mod_opensslBundle.BIO_read(this->__ctx->b64, this->__ctx->result.data(), this->__ctx->result.size());
	this->__ctx->resultSize = ret >= 0? (size_t)ret : 0;

	return *this;
}

Base64Decoder& Base64Decoder::finish() LASHE_EXCEPT(FilterException)
{
	this->__dropif_ready(true);
	return *this;
}

Base64Decoder& Base64Decoder::clear() LASHE_NOEXCEPT
{
	this->__ctx->result.clear();
	this->__ctx->result.shrink_to_fit();
	this->__ctx->resultSize = 0;
	this->__pushed = 0;
	return *this;
}

bool Base64Decoder::hasPayload() const LASHE_NOEXCEPT
{
	return this->__ctx->resultSize != 0;
}

size_t Base64Decoder::payloadSize() const LASHE_EXCEPT(FilterException)
{
	const size_t ret = this->__ctx->resultSize;
	if(0 == ret)
	{
		FilterException e;
		e.code(FilterException::C_NO_DATA);
		throw e;
	}
	return ret;
}

Base64Decoder& Base64Decoder::payload(void* buf, const size_t len) LASHE_EXCEPT(FilterException)
{
	if(this->payloadSize() > len)
		motherClass::__drop_shortBuf__();
	else
		motherClass::__dropif_nullBuf__(buf);
	::memcpy(buf, this->__ctx->result.data(), this->__ctx->resultSize);
	return *this;
}

void Base64Decoder::__prepParamPool(const char* key) const LASHE_NOEXCEPT
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

void Base64Decoder::__prepParamMap() const LASHE_NOEXCEPT
{
	if(!this->__ctx->paramChanged)
		return;
	this->__ctx->paramMap.clear();

	this->__ctx->paramMap.insert(std::make_pair("URL", this->__ctx->isURL? "true" : "false"));

	this->__ctx->paramChanged = false;
}

}
