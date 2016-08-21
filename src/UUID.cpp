#include "libashe/UUID.h"
#include "__internal.h"
#include "libashe/MessageDigest.h"
#include "libashe/Process.h"

#include <vector>

#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>

#include <thread>


namespace ashe
{
namespace uuid
{

static void __putGlamour__(void *x, const /* uuid::Version */ uint32_t ver) LASHE_NOEXCEPT
{
	uint8_t *p = (uint8_t*)x;
	const uint8_t theDigit = (uint8_t)(0x10 * ver);
	p[6] = (p[6] & 0x0F) | theDigit;
	p[8] = (((((p[8] & 0xF0) >> 4) % 4) + 8) << 4) | (p[8] & 0x0F);
}

/******************
* Exception
*/
Exception::~Exception() LASHE_NOEXCEPT{}

const char *Exception::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::uuid::Exception";
	return __name__;
}

const char *Exception::code2str(const uint32_t x) const LASHE_NOEXCEPT
{
	static const char *__str__[] = {
			"none",
			"LibAshe uninitialised",
			"LibAshe not initialised with this ability",
			"not found",
			"short length given",
			"invalid magic digit",
			"invalid format"
	};

	switch(x)
	{
	case C_NONE:
	case C_LASHE_UNINITIALISED:
	case C_LASHE_NO_ABILITY:
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
struct __MersenneTwisterEngineContext
{
	FilterInterface *md = nullptr;
	std::mt19937_64 rndEng;
};

MersenneTwisterEngine::MersenneTwisterEngine() LASHE_NOEXCEPT
	: __privCtx(new __MersenneTwisterEngineContext)
	, __poolSize(32)
{
	this->__privCtx->md = mkMessageDigest(LAHA_SHA1);
}

MersenneTwisterEngine::~MersenneTwisterEngine() LASHE_NOEXCEPT
{
	delete this->__privCtx;
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

UUID MersenneTwisterEngine::generate() LASHE_NOEXCEPT
{
	UUID y;
	std::vector<uint64_t> content(this->__poolSize);
	std::vector<uint8_t> buf;

	for(auto &v : content)
		v = this->__privCtx->rndEng();
	buf.resize(this->__privCtx->md->feed(content.data(), content.size()*sizeof(uint64_t)).finish().payloadSize());
	this->__privCtx->md->payload(buf.data(), buf.size());
	::memcpy(y.data, buf.data(), RAW_BYTE_SIZE);
	__putGlamour__(y.data, V_RANDOM);

	return y;
}

MersenneTwisterEngine& MersenneTwisterEngine::randomise() LASHE_NOEXCEPT
{
	std::hash<std::thread::id> hasher;
	std::seed_seq seed({
		(uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count(),
		(uint64_t)hasher(std::this_thread::get_id()),
		(uint64_t)pid__()});

	this->__privCtx->rndEng.seed(seed);
	return *this;
}

#undef __INTP_CTX

/******************
* Namespace functions.
*/
const uint32_t *implVersion__() LASHE_NOEXCEPT
{
	static const uint32_t __ARR__[] = {5};
	return __ARR__;
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
	__dropif_noAbility<Exception>(LAANS_OPENSSL);

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

UUID generate() LASHE_EXCEPT(Exception)
{
	UUID ret;

	__dropif_noAbility<Exception>(LAANS_OPENSSL);
	{
		__LOCK_DEF_UUID_ENGINE();
		ret = __lashe_defUUIDEngine->generate();
	}
	return ret;
}

UUID fromString(const char* str) LASHE_EXCEPT(Exception)
{
	return fromString(str, ::strlen(str));
}

UUID fromString(const char* str, const size_t len) LASHE_EXCEPT(Exception)
{
	validateString(str, len);
	std::string __str(str, len);
	UUID ret;
	const char *p;

	__trim__(__str);
	__lower__(__str);
	for(auto &v : __str)
	{
		if('0' <= v && v <= '9')
			v -= '0';
		else if('a' <= v && v <= 'z')
			v -= 'a' - 10;
	}
	p = __str.data();

	ret.data[0] = (uint8_t)((p[0] << 4) | p[1]);
	ret.data[1] = (uint8_t)((p[2] << 4) | p[3]);
	ret.data[2] = (uint8_t)((p[4] << 4) | p[5]);
	ret.data[3] = (uint8_t)((p[6] << 4) | p[7]);
	ret.data[4] = (uint8_t)((p[9] << 4) | p[10]);
	ret.data[5] = (uint8_t)((p[11] << 4) | p[12]);
	ret.data[6] = (uint8_t)((p[14] << 4) | p[15]);
	ret.data[7] = (uint8_t)((p[16] << 4) | p[17]);
	ret.data[8] = (uint8_t)((p[19] << 4) | p[20]);
	ret.data[9] = (uint8_t)((p[21] << 4) | p[22]);
	ret.data[10] = (uint8_t)((p[24] << 4) | p[25]);
	ret.data[11] = (uint8_t)((p[26] << 4) | p[27]);
	ret.data[12] = (uint8_t)((p[28] << 4) | p[29]);
	ret.data[13] = (uint8_t)((p[30] << 4) | p[31]);
	ret.data[14] = (uint8_t)((p[32] << 4) | p[33]);
	ret.data[15] = (uint8_t)((p[34] << 4) | p[35]);

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

	if(!std::regex_match(__str, *__lashe_re_uuidHusk))
	{
		Exception e;
		e.code(Exception::C_INVALID_FORMAT);
		throw e;
	}
	__trim__(__str);

	switch(__str[19])
	{
	case '8':
	case '9':
	case 'a':
	case 'b':
	case 'A':
	case 'B':
		break;
	default:
	{
		Exception e;
		e.code(Exception::C_INVALID_MAGIC_DIGIT);
		throw e;
	}
	}
}

}

/******************
* UUID
*/
UUID::UUID() LASHE_NOEXCEPT
{
	::memset(this->data, 0, uuid::RAW_BYTE_SIZE);
}

UUID::UUID(const thisClass& src) LASHE_NOEXCEPT
{
	::memcpy(this->data, src.data, uuid::RAW_BYTE_SIZE);
}

UUID::~UUID() LASHE_NOEXCEPT{}

UUID& UUID::operator =(const thisClass& src) LASHE_NOEXCEPT
{
	::memcpy(this->data, src.data, uuid::RAW_BYTE_SIZE);
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
	static const char __UPPER__[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	static const char __LOWER__[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	const char *t = upper? __UPPER__ : __LOWER__;

	p[0] = t[this->data[0] >> 4];
	p[1] = t[this->data[0] & 0x0F];
	p[2] = t[this->data[1] >> 4];
	p[3] = t[this->data[1] & 0x0F];
	p[4] = t[this->data[2] >> 4];
	p[5] = t[this->data[2] & 0x0F];
	p[6] = t[this->data[3] >> 4];
	p[7] = t[this->data[3] & 0x0F];
	p[8] = '-';
	p[9] = t[this->data[4] >> 4];
	p[10] = t[this->data[4] & 0x0F];
	p[11] = t[this->data[5] >> 4];
	p[12] = t[this->data[5] & 0x0F];
	p[13] = '-';
	p[14] = t[this->data[6] >> 4];
	p[15] = t[this->data[6] & 0x0F];
	p[16] = t[this->data[7] >> 4];
	p[17] = t[this->data[7] & 0x0F];
	p[18] = '-';
	p[19] = t[this->data[8] >> 4];
	p[20] = t[this->data[8] & 0x0F];
	p[21] = t[this->data[9] >> 4];
	p[22] = t[this->data[9] & 0x0F];
	p[23] = '-';
	p[24] = t[this->data[10] >> 4];
	p[25] = t[this->data[10] & 0x0F];
	p[26] = t[this->data[11] >> 4];
	p[27] = t[this->data[11] & 0x0F];
	p[28] = t[this->data[12] >> 4];
	p[29] = t[this->data[12] & 0x0F];
	p[30] = t[this->data[13] >> 4];
	p[31] = t[this->data[13] & 0x0F];
	p[32] = t[this->data[14] >> 4];
	p[33] = t[this->data[14] & 0x0F];
	p[34] = t[this->data[15] >> 4];
	p[35] = t[this->data[15] & 0x0F];

	return *this;
}

UUID UUID::merge(const thisClass& x) const LASHE_EXCEPT(uuid::Exception)
{
	UUID y;
	FilterResult hashed;
	uint8_t buf[uuid::RAW_BYTE_SIZE*2];

	__dropif_noAbility<uuid::Exception>(LAANS_OPENSSL);

	::memcpy(buf, this->data, uuid::RAW_BYTE_SIZE);
	::memcpy(buf + uuid::RAW_BYTE_SIZE, x.data, uuid::RAW_BYTE_SIZE);
	hashed = digest(buf, uuid::RAW_BYTE_SIZE*2, LAHA_SHA1);
	::memcpy(y.data, hashed.ptr, uuid::RAW_BYTE_SIZE);

	return y;
}

}
