#include "libashe/UUID.h"
#include "__internal.h"

#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>


namespace ashe
{
namespace uuid
{

/******************
* Exception
*/
Exception::~Exception() LASHE_NOEXCEPT{}

const char *Exception::className() const LASHE_NOEXCEPT //@Override
{
	static const char *__name__ = "ashe::uuid::Exception";
	return __name__;
}

const char *Exception::code2str(const uint32_t x) const LASHE_NOEXCEPT //@Override
{
	static const char *__str__[] = {
			"none",
			"LibAshe uninitialised",
			"not found",
			"short length given",
			"invalid magic digit",
			"invalid format"
	};

	switch(x)
	{
	case C_NONE:
	case C_LASHE_UNINITIALISED:
	case C_NOT_FOUND:
	case C_SHORT_LENGTH:
	case C_INVALID_MAGIC_DIGIT:
	case C_INVALID_FORMAT:
		return __str__[x];
	}
	return motherClass::code2str(x);
}

/******************
* RandomEngine
*/
RandomEngine::~RandomEngine() LASHE_NOEXCEPT{}

UUID RandomEngine::operator()() LASHE_NOEXCEPT
{
	return this->generate();
}

/******************
* MersenneTwisterEngine
*/
#define __INTP_CTX() auto &__ctx = *(std::mt19937_64*)this->__privCtx

MersenneTwisterEngine::MersenneTwisterEngine() LASHE_NOEXCEPT
	: __privCtx(new std::mt19937_64)
	, __poolSize(32)
{
}

MersenneTwisterEngine::~MersenneTwisterEngine() LASHE_NOEXCEPT
{
	delete (std::mt19937_64*)this->__privCtx;
}

size_t MersenneTwisterEngine::poolSize() const LASHE_NOEXCEPT
{
	return this->__poolSize;
}

MersenneTwisterEngine& MersenneTwisterEngine::poolSize(const size_t size) LASHE_NOEXCEPT
{
	this->__poolSize = size;
	return *this;
}

UUID MersenneTwisterEngine::generate() LASHE_NOEXCEPT //@Implement
{
	// TODO
#if 0
	UUID y;
	std::vector<uint64_t> content(this->__poolSize);
	std::array<uint8_t, SHA1_DIGEST_SIZE> hashed;

	for(auto &v : content)
		v = this->random();
	::sha1_buffer((char*)content.data(), content.size() * sizeof(uint64_t), hashed.data());
	y.__build(hashed.data(), hashed.size(), UUID::VER_RANDOM);

	return y;
#endif
}

MersenneTwisterEngine& MersenneTwisterEngine::randomise() LASHE_NOEXCEPT
{
	// TODO: Use PID as seed as well.
	__INTP_CTX();
	return *this;
}

#undef __INTP_CTX

/******************
* Namespace functions.
*/
uint32_t implVersion__() LASHE_NOEXCEPT
{
	return 5;
}

const char* versionToString__(const uint32_t v) LASHE_NOEXCEPT
{
	static const char *__str__[] = {
			"none",
			"mac&datetime",
			"dce secure",
			"md5&namespace",
			"random",
			"sha1&namespace"
	};
	static const char *__unknown__ = "";

	switch(v)
	{
	case V_NONE:
	case V_MAC_AND_DATETIME:
	case V_DCE_SECURE:
	case V_MD5_AND_NAMESPACE:
	case V_RANDOM:
	case V_SHA1_AND_NAMESPACE:
		return __str__[v];
	}
	return __unknown__;
}

RandomEngine* mkRandomEngine(const char* name) LASHE_EXCEPT(Exception)
{
	__dropif_uninitialised<Exception>();

	if(0 == ::strcmp(name, "MersenneTwisterEngine"))
	{
		return new MersenneTwisterEngine();
	}
	else
	{
		Exception e;
		e.code(Exception::C_NOT_FOUND);
		throw e;
	}

	return nullptr;
}

UUID generate(const uint32_t v) LASHE_EXCEPT(Exception)
{
	__dropif_uninitialised<Exception>();
	UUID ret;
	// TODO: Make default engine in ashe::initLibAshe() and use it.
	return ret;
}

UUID fromString(const char* str) LASHE_EXCEPT(Exception)
{
	return fromString(str, 	::strlen(str));
}

UUID fromString(const char* str, const size_t len) LASHE_EXCEPT(Exception)
{
	validateString(str, len);
	std::string __str(str, len);
	UUID ret;
	__trim__(__str);
	// __lower__(__str); XXX

	{
		std::string::size_type i = 0, j = 1;
		size_t p = 0;
		std::string digit = "00";

		for(auto &v : __str)
		{
			++i;
			switch(i)
			{
			case 9:
			case 14:
			case 19:
			case 24:
				// Dashes
				continue;
			}

			if(! (j%2))
			{
				digit[1] = v;
				ret.data[p++] = (uint8_t)std::stoul(digit, 0, 16);
			}
			else
				digit[0] = v;
			++j;
		}
	}

	return ret;
}

UUID fromRaw(const void* p) LASHE_EXCEPT(Exception)
{
	validateRaw(p);
	UUID ret;
	::memcpy(ret.data, p, uuid::RAW_BYTE_SIZE);
	return ret;
}

void validateRaw(const void* x) LASHE_EXCEPT(Exception)
{
	const uint8_t *p = (const uint8_t*)x;
	__dropif_uninitialised<Exception>();

	switch(p[8] & 0xF0)
	{
	case 0x80:
	case 0x90:
	case 0xA0:
	case 0xB0:
		break;
	default:
	{
		Exception e;
		e.code(Exception::C_INVALID_MAGIC_DIGIT);
		throw e;
	}
	}
}

void validateString(const char* str) LASHE_EXCEPT(Exception)
{
	validateString(str, ::strlen(str));
}

void validateString(const char* str, const size_t len) LASHE_EXCEPT(Exception)
{
	__dropif_uninitialised<Exception>();
	std::string __str(str, len);

	__trim__(__str);
	__lower__(__str);
	if(!std::regex_match(__str, *__lashe_re_uuidHusk))
	{
		Exception e;
		e.code(Exception::C_INVALID_FORMAT);
		throw e;
	}

	switch(__str[18])
	{
	case '8':
	case '9':
	case 'a':
	case 'b':
		break;
	default:
	{
		Exception e;
		e.code(Exception::C_INVALID_MAGIC_DIGIT);
		throw e;
	}
	}
}

static void __putGlamour__(void *x, const /* uuid::Version */ uint32_t ver) LASHE_NOEXCEPT
{
	uint8_t *p = (uint8_t*)x;
	const uint8_t theDigit = (uint8_t)(0x10 * ver);
	p[6] = (p[6] & 0x0F) | theDigit;
	p[8] = (((((p[8] & 0xF0) >> 4) % 4) + 8) << 4) | (p[8] & 0x0F);
}

}

/******************
* UUID
*/
UUID::UUID() LASHE_NOEXCEPT
{
	::memset(this->data, 0, sizeof(uuid::RAW_BYTE_SIZE));
}

UUID::UUID(const thisClass& src) LASHE_NOEXCEPT
{
	::memcpy(this->data, src.data, sizeof(uuid::RAW_BYTE_SIZE));
}

UUID::~UUID() LASHE_NOEXCEPT{}

UUID& UUID::operator =(const thisClass& src) LASHE_NOEXCEPT
{
	::memcpy(this->data, src.data, sizeof(uuid::RAW_BYTE_SIZE));
	return *this;
}

const char* UUID::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::UUID";
	return __name__;
}

bool UUID::operator ==(const thisClass& x) const LASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, uuid::RAW_BYTE_SIZE) == 0;
}

bool UUID::operator ==(const char* x) const LASHE_EXCEPT(uuid::Exception)
{
	return uuid::fromString(x) == *this;
}

bool UUID::operator !=(const thisClass& x) const LASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, uuid::RAW_BYTE_SIZE) != 0;
}

bool UUID::operator !=(const char* x) const LASHE_EXCEPT(uuid::Exception)
{
	return uuid::fromString(x) != *this;
}

UUID UUID::operator +(const thisClass& x) const LASHE_NOEXCEPT
{
	this->merge(x);
	return *this;
}

uint32_t UUID::version() const LASHE_NOEXCEPT
{
	return (uint32_t)(this->data[6] >> 4);
}

const UUID& UUID::string(char* p, const bool upper) const LASHE_NOEXCEPT
{
	std::stringstream sb;
	std::string y;
	size_t i;

	sb.unsetf(std::ios::showbase);
	if(upper)
		sb.setf(std::ios::uppercase);
	else
		sb.unsetf(std::ios::uppercase);
	for(i=0; i<uuid::RAW_BYTE_SIZE; ++i)
		sb << std::setw(2) << std::setfill('0') << std::hex << (int)this->data[i];
	y = sb.str();
	y.insert(8, "-");
	y.insert(13, "-");
	y.insert(18, "-");
	y.insert(23, "-");

	::memcpy(p, y.c_str(), uuid::STRING_SIZE);
	return *this;
}

UUID UUID::merge(const thisClass& x) const LASHE_NOEXCEPT
{
	UUID y;
	// TODO
#if 0
	std::array<uint8_t, SHA1_DIGEST_SIZE> hashed;
	std::array<uint8_t, thisClass::UUID_BYTE_SIZE*2> concatenated;

	::memcpy(concatenated.data(), this->data, thisClass::UUID_BYTE_SIZE);
	::memcpy(concatenated.data() + thisClass::UUID_BYTE_SIZE, x.data, thisClass::UUID_BYTE_SIZE);
	::sha1_buffer((char*)concatenated.data(), concatenated.size(), hashed.data());
	y.__build(hashed.data(), hashed.size(), VER_SHA1_AND_NAMESPACE);
#endif
	return y;
}

}
