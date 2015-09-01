#include <src/libashe/UUID.h>
#include "__coreutils/sha1.h"
#include <chrono>
#include <vector>
#include <cstring>
#include <array>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <iomanip>

namespace ashe
{

// One can make one's own namespace, in any version
const UUID UUID::UUID_NS_DNS = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";
const UUID UUID::UUID_NS_URL = "6ba7b811-9dad-11d1-80b4-00c04fd430c8";
const UUID UUID::UUID_NS_ISO_OID = "6ba7b812-9dad-11d1-80b4-00c04fd430c8";
const UUID UUID::UUID_NS_X500 = "6ba7b814-9dad-11d1-80b4-00c04fd430c8";

// For version (1 to 5, version digit) + (89ab magic digit)
const std::regex UUID::REGEX("[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[8-9a-b][0-9a-f]{3}-[0-9a-f]{12}");

static std::mutex __mtx__;
static UUID::MersenneTwisterEngine __defaultEngine__;

UUID::UUID() ASHE_NOEXCEPT
{
	this->className = "ashe::UUID";
}

UUID::UUID(const std::array<uint8_t, 8> trailing) ASHE_NOEXCEPT
{
	const uint64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::array<uint8_t, sizeof(uint64_t)> nowBuffer;
	std::array<uint8_t, thisClass::UUID_BYTE_SIZE> buffer;
	memcpy(nowBuffer.data(), &now, sizeof(uint8_t));
#if ASHE_IS_LITTLE_ENDIAN
	// TODO: Into BIG-ENDIAN!
	std::reverse(nowBuffer.begin(), nowBuffer.end());
#endif
	::memcpy(buffer.data(), nowBuffer.data(), nowBuffer.size());
	::memcpy(buffer.data() + sizeof(uint64_t), trailing.data(), trailing.size());
	this->__build(buffer.data(), buffer.size(), thisClass::VER_MAC_AND_DATETIME);
	this->className = "ashe::UUID";
}

UUID::UUID(const std::string& x) throw (Rune)
{
	this->__parse(x.c_str(), x.length()); // Throws parse error (Rune)
	this->className = "ashe::UUID";
}

UUID::UUID(const char* x, const size_t len) throw (Rune)
{
	this->__parse(x, len); // Throws parse error (Rune)
	this->className = "ashe::UUID";
}

UUID::UUID(const void* buffer, const size_t len) throw (Rune)
{
	this->__build(buffer, len); // Throws validity error (Rune)
	this->className = "ashe::UUID";
}

UUID::UUID(const thisClass& src) ASHE_NOEXCEPT
	: motherClass(src)
{
	this->__construct(src);
	this->className = "ashe::UUID";
}

UUID::~UUID() ASHE_NOEXCEPT {}

void UUID::__build(const void* x, const size_t len, const Version ver/* = VER_NONE*/) throw(Rune)
{
	if(len < thisClass::UUID_BYTE_SIZE)
		throw Rune(Rune::C_SHORT_LENGTH, "Tried to construct out of binary");
	::memcpy(this->data, x, thisClass::UUID_BYTE_SIZE);
	if(ver == VER_NONE)
		thisClass::validate__(this->data); // Throws validity error (Rune)
	else
		thisClass::__putGlamour__(this->data, ver);
}

void UUID::__parse(const char* x, const size_t len) throw (Rune)
{
	struct ParseError
	{
		size_t pos;
		char illegalChar;
		Rune::Code code;
	};
	auto throwParseError = [](const size_t pos, const char c, const Rune::Code code)
	{
		ParseError e;
		e.pos = pos;
		e.illegalChar = c;
		e.code = code;
		throw e;
	};
	if(len < thisClass::UUID_STRING_LENGTH)
		throw Rune(Rune::C_SHORT_LENGTH, "Tried to construct out of string");
	std::string str = x;
	const std::string::size_type starting = findStarting__(str);
	str = toLower__(trim__(str)).substr(0, thisClass::UUID_STRING_LENGTH);

	try
	{
		std::string::size_type i = 0, j = 1;
		size_t p = 0;
		std::string digit = "00";

		for(auto &v : str)
		{
			++i;
			switch(i)
			{
			case 9:case 14:case 19:case 24: // Dashes
				if(v != '-')
					throwParseError(i, v, Rune::C_PARSE_ERROR);
				continue;
			case 15: // Version digit
				if(! ('1' <= v && v <= '5'))
					throwParseError(i, v, Rune::C_INVALID_VERSION_DIGIT);
				break;
			case 20: // Magic digit
				switch(v)
				{
				case '8':case '9':case 'a':case 'b': // Magic characters
					break;
				default:
					throwParseError(i, v, Rune::C_INVALID_MAGIC_DIGIT);
				}
				break;
			default:
				if(! (('0' <= v && v <= '9') || ('a' <= v && v <= 'z')))
					throwParseError(i, v, Rune::C_PARSE_ERROR);
			}

			if(! (j%2))
			{
				digit[1] = v;
				this->data[p++] = (uint8_t)std::stoul(digit, 0, 16);
			}
			else
				digit[0] = v;
			++j;
		}
	}
	catch(ParseError &e)
	{
		std::stringstream sb;
		sb << "Illegal character '" << e.illegalChar << "' at " << e.pos + starting;
		throw Rune(e.code, sb.str());
	}
}

void UUID::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	::memcpy(this->data, src.data, thisClass::UUID_BYTE_SIZE);
}

UUID::thisClass& UUID::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->__construct(src);
	return *this;
}

UUID::thisClass& UUID::operator =(const std::string& x) throw (Rune)
{
	this->__parse(x.data(), x.length());
	return *this;
}

UUID::thisClass& UUID::operator =(const char* x) throw (Rune)
{
	this->__parse(x, strlen(x));
	return *this;
}

bool UUID::operator ==(const thisClass& x) const ASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, thisClass::UUID_BYTE_SIZE) == 0;
}

bool UUID::operator ==(const std::string& x) const throw (Rune)
{
	UUID y(x);
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) == 0;
}

bool UUID::operator ==(const char* x) const throw (Rune)
{
	UUID y(x, strlen(x));
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) == 0;
}


bool UUID::operator!=(const thisClass &x) const ASHE_NOEXCEPT
{
	return !this->operator==(x);
}


bool UUID::operator!=(const std::string &x) const throw(Rune)
{
	return !this->operator==(x);
}


bool UUID::operator!=(const char *x) const throw(Rune)
{
	return !this->operator==(x);
}

bool UUID::operator <=(const thisClass& x) const ASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, thisClass::UUID_BYTE_SIZE) <= 0;
}

bool UUID::operator <=(const std::string& x) const throw (Rune)
{
	UUID y(x);
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) <= 0;
}

bool UUID::operator <=(const char* x) const throw (Rune)
{
	UUID y(x, strlen(x));
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) <= 0;
}

bool UUID::operator >=(const thisClass& x) const ASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, thisClass::UUID_BYTE_SIZE) >= 0;
}

bool UUID::operator >=(const std::string& x) const throw (Rune)
{
	UUID y(x);
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) >= 0;
}

bool UUID::operator >=(const char* x) const throw (Rune)
{
	UUID y(x, strlen(x));
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) >= 0;
}

bool UUID::operator <(const thisClass& x) const ASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, thisClass::UUID_BYTE_SIZE) < 0;
}

bool UUID::operator <(const std::string& x) const throw (Rune)
{
	UUID y(x);
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) < 0;
}

bool UUID::operator <(const char* x) const throw (Rune)
{
	UUID y(x, strlen(x));
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) < 0;
}

bool UUID::operator >(const thisClass& x) const ASHE_NOEXCEPT
{
	return ::memcmp(x.data, this->data, thisClass::UUID_BYTE_SIZE) > 0;
}

bool UUID::operator >(const std::string& x) const throw (Rune)
{
	UUID y(x);
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) > 0;
}

bool UUID::operator >(const char* x) const throw (Rune)
{
	UUID y(x, strlen(x));
	return ::memcmp(y.data, this->data, thisClass::UUID_BYTE_SIZE) > 0;
}

UUID UUID::operator +(const thisClass& x) const ASHE_NOEXCEPT
{
	return this->merge(x);
}

UUID UUID::operator +(const std::string& x) const throw (Rune)
{
	return this->merge(x.data(), x.length()); // Throws Rune
}

UUID UUID::operator +(const char* x) const throw (Rune)
{
	return this->merge(x, strlen(x)); // Throws Rune
}

std::array<uint8_t, UUID::UUID_BYTE_SIZE> UUID::toBinary() const ASHE_NOEXCEPT
{
	std::array<uint8_t, UUID::UUID_BYTE_SIZE> y;
	::memcpy(y.data(), this->data, thisClass::UUID_BYTE_SIZE);
	return y;
}

std::string UUID::toString(const bool upper/* = false*/) const ASHE_NOEXCEPT
{
	std::stringstream sb;
	std::string y;
	size_t i;
	sb.unsetf(std::ios::showbase);
	if(upper)
		sb.setf(std::ios::uppercase);
	else
		sb.unsetf(std::ios::uppercase);

	for(i=0; i<thisClass::UUID_BYTE_SIZE; ++i)
		sb << std::setw(2) << std::setfill('0') << std::hex << (uint16_t)this->data[i];

	y = sb.str();
	y.insert(8, "-");
	y.insert(13, "-");
	y.insert(18, "-");
	y.insert(23, "-");

	return y;
}

UUID::Version UUID::version() const ASHE_NOEXCEPT
{
	return (Version)(this->data[6] >> 4);
}

UUID UUID::merge(const void* x, const size_t len) const throw(Rune)
{
	if(! len)
		throw Rune(Rune::C_EMPTY_CONTENT, "Tried to merge with namespace UUID");
	UUID y;
	std::vector<uint8_t> concatenated(thisClass::UUID_BYTE_SIZE + len);
	std::array<uint8_t, SHA1_DIGEST_SIZE> hashed;

	::memcpy(concatenated.data(), this->data, thisClass::UUID_BYTE_SIZE);
	::memcpy(concatenated.data() + thisClass::UUID_BYTE_SIZE, x, len);
	::sha1_buffer((char*)concatenated.data(), concatenated.size(), hashed.data());
	y.__build(hashed.data(), hashed.size(), VER_SHA1_AND_NAMESPACE);

	return y;
}

UUID UUID::merge(const thisClass& x) const ASHE_NOEXCEPT
{
	UUID y;
	std::array<uint8_t, SHA1_DIGEST_SIZE> hashed;
	std::array<uint8_t, thisClass::UUID_BYTE_SIZE*2> concatenated;

	::memcpy(concatenated.data(), this->data, thisClass::UUID_BYTE_SIZE);
	::memcpy(concatenated.data() + thisClass::UUID_BYTE_SIZE, x.data, thisClass::UUID_BYTE_SIZE);
	::sha1_buffer((char*)concatenated.data(), concatenated.size(), hashed.data());
	y.__build(hashed.data(), hashed.size(), VER_SHA1_AND_NAMESPACE);

	return y;
}

UUID UUID::merge(const std::string& x) const throw(Rune)
{
	return this->merge(x.data(), x.length());
}

UUID UUID::generate__() ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> lg(__mtx__);
	return __defaultEngine__.generate();
}

void UUID::__putGlamour__(void* x, const Version ver) throw(StrongRune)
{
	uint8_t *p = (uint8_t*)x;
	uint8_t theDigit;

	switch(ver)
	{
	case VER_MAC_AND_DATETIME: theDigit = 0x10; break;
	case VER_DCE_SECURE: theDigit = 0x20; break;
	case VER_MD5_AND_NAMESPACE: theDigit = 0x30; break;
	case VER_RANDOM: theDigit = 0x40; break;
	case VER_SHA1_AND_NAMESPACE: theDigit = 0x50; break;
	default:
	{
		std::stringstream sb;
		sb << "Unexpected argument 'ver': " << ver;
		throw StrongRune(sb.str());
	}
	}
	p[6] = (p[6] & 0x0F) | theDigit;
	p[8] = (((((p[8] & 0xF0) >> 4) % 4) + 8) << 4) | (p[8] & 0x0F);
}

void UUID::validate__(const void* x) throw (Rune)
{
	const uint8_t *p = (const uint8_t*)x;
	switch(p[6] & 0xF0)
	{
	case 0x10:
	case 0x20:
	case 0x30:
	case 0x40:
	case 0x50:
		break;
	default:
		throw Rune(Rune::C_INVALID_VERSION_DIGIT, "Whilst validation.");
	}
	switch(p[8] & 0xF0)
	{
	case 0x80:
	case 0x90:
	case 0xA0:
	case 0xB0:
		break;
	default:
		throw Rune(Rune::C_INVALID_MAGIC_DIGIT, "Whilst validation.");
	}
}

std::string UUID::versionToString__(const Version v) ASHE_NOEXCEPT
{
	switch(v)
	{
	case VER_MAC_AND_DATETIME: return "1, MAC and datetime";
	case VER_DCE_SECURE: return "2, DCE secure";
	case VER_MD5_AND_NAMESPACE: return "3, MD5 and namespace";
	case VER_RANDOM: return "4, random";
	case VER_SHA1_AND_NAMESPACE: return "5, SHA1 and namespace";
	}
	return "* Undefined";
}

} /* namespace ashe */

namespace ashe
{

UUID::RandomEngine::~RandomEngine() ASHE_NOEXCEPT {}

void UUID::RandomEngine::__construct(const thisClass &src) ASHE_NOEXCEPT
{
	this->poolSize = src.poolSize;
}

UUID::RandomEngine::thisClass& UUID::RandomEngine::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	this->__construct(src);
	return *this;
}

UUID UUID::RandomEngine::operator ()() ASHE_NOEXCEPT
{
	return this->generate();
}

UUID::RandomEngine::thisClass& UUID::RandomEngine::setPoolSize(const size_t poolSize) ASHE_NOEXCEPT
{
	this->poolSize = poolSize;
	return *this;
}

size_t UUID::RandomEngine::getPoolSize() const ASHE_NOEXCEPT
{
	return this->poolSize;
}

UUID UUID::RandomEngine::generate() ASHE_NOEXCEPT
{
	UUID y;
	std::vector<uint64_t> content(this->poolSize);
	std::array<uint8_t, SHA1_DIGEST_SIZE> hashed;

	for(auto &v : content)
		v = this->random();
	::sha1_buffer((char*)content.data(), content.size() * sizeof(uint64_t), hashed.data());
	y.__build(hashed.data(), hashed.size(), UUID::VER_RANDOM);

	return y;
}

}

namespace ashe
{

UUID::MersenneTwisterEngine::MersenneTwisterEngine() ASHE_NOEXCEPT
{
	this->className = "ashe::UUID::MersenneTwisterEngine";
	this->randomise();
}

UUID::MersenneTwisterEngine::MersenneTwisterEngine(const thisClass& src) ASHE_NOEXCEPT
{
	this->className = "ashe::UUID::MersenneTwisterEngine";
	motherClass::__construct(src);
	this->randomise();
}

UUID::MersenneTwisterEngine::~MersenneTwisterEngine() ASHE_NOEXCEPT
{
}

UUID::MersenneTwisterEngine::thisClass& UUID::MersenneTwisterEngine::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->randomise();
	return *this;
}

UUID::MersenneTwisterEngine::thisClass& UUID::MersenneTwisterEngine::randomise() ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> lg(__mtx__);
	std::seed_seq sq = {__defaultEngine__.random(), (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count()};
	this->__engine.seed(sq);
	return *this;
}

uint64_t UUID::MersenneTwisterEngine::random() ASHE_NOEXCEPT
{
	return this->__engine();
}

}

namespace ashe
{

UUID::Rune::Rune(const Code code/* = C_NONE*/, const std::string msg/* = ""*/) ASHE_NOEXCEPT
		: code(code)
{
	std::stringstream sb;
	sb << msg << "(" << thisClass::codeToString__(code) << ")";
	this->whatString = sb.str();
}

UUID::Rune::Rune(const thisClass& src) ASHE_NOEXCEPT
		: motherClass(src)
{
	this->__construct(src);
}

UUID::Rune::~Rune() ASHE_NOEXCEPT {}

UUID::Rune::thisClass& UUID::Rune::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->__construct(src);
	return *this;
}

void UUID::Rune::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->code = src.code;
}

UUID::Rune::Code UUID::Rune::getCode() const ASHE_NOEXCEPT
{
	return this->code;
}

std::string UUID::Rune::codeToString__(const Code x) ASHE_NOEXCEPT
{
	switch(x)
	{
	case C_SHORT_LENGTH: return "Short given length";
	case C_INVALID_VERSION_DIGIT: return "Invalid UUID version";
	case C_INVALID_MAGIC_DIGIT: return "Invalid UUID: the magic digit is neither of 8, 9, A nor B";
	case C_PARSE_ERROR: return "Parse error";
	case C_EMPTY_CONTENT: return "Empty content";
	}
	return "* Undefined";
}

}
