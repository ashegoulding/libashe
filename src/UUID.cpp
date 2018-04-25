#include "__internal.hpp"
#include "__regex.hpp"
#include "libashe/MessageDigest.hpp"
#include "libashe/StringUtils.hpp"
#include "libashe/UUID.hpp"

#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <thread>
#include <vector>

#include <cstring>


namespace ashe {

struct __UUIDPrivData {
    uint8_t data[UUID::RAW_BYTE_SIZE];
    std::string str;

    __UUIDPrivData() { this->str.resize(36); }
};

namespace __uuid {

static void put_glamour(uint8_t *p, const UUID::Version ver) LASHE_NOEXCEPT
{
    const uint8_t theDigit = ((uint8_t)ver * 0x10);

    p[6] = (p[6] & 0x0F) | theDigit;
    p[8] = (((((p[8] & 0xF0) >> 4) % 4) + 8) << 4) | (p[8] & 0x0F);
}

} // namespace __uuid

/******************
 * UUID
 */
const char *UUID::tostr(const Version v) LASHE_NOEXCEPT
{
    switch (v) {
    case Version::NONE:
        return "NONE";
    case Version::MAC_AND_DATETIME:
        return "MAC_AND_DATETIME";
    case Version::DCE_SECURE:
        return "DCE_SECURE";
    case Version::MD5_AND_NAMESPACE:
        return "MD5_AND_NAMESPACE";
    case Version::RANDOM:
        return "RANDOM";
    case Version::SHA1_AND_NAMESPACE:
        return "SHA1_AND_NAMESPACE";
    }
    return nullptr;
}

void UUID::__mkStr() const LASHE_NOEXCEPT
{
    static const char t[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    this->__pd->str[0] = t[this->__pd->data[0] >> 4];
    this->__pd->str[1] = t[this->__pd->data[0] & 0x0F];
    this->__pd->str[2] = t[this->__pd->data[1] >> 4];
    this->__pd->str[3] = t[this->__pd->data[1] & 0x0F];
    this->__pd->str[4] = t[this->__pd->data[2] >> 4];
    this->__pd->str[5] = t[this->__pd->data[2] & 0x0F];
    this->__pd->str[6] = t[this->__pd->data[3] >> 4];
    this->__pd->str[7] = t[this->__pd->data[3] & 0x0F];
    this->__pd->str[8] = '-';
    this->__pd->str[9] = t[this->__pd->data[4] >> 4];
    this->__pd->str[10] = t[this->__pd->data[4] & 0x0F];
    this->__pd->str[11] = t[this->__pd->data[5] >> 4];
    this->__pd->str[12] = t[this->__pd->data[5] & 0x0F];
    this->__pd->str[13] = '-';
    this->__pd->str[14] = t[this->__pd->data[6] >> 4];
    this->__pd->str[15] = t[this->__pd->data[6] & 0x0F];
    this->__pd->str[16] = t[this->__pd->data[7] >> 4];
    this->__pd->str[17] = t[this->__pd->data[7] & 0x0F];
    this->__pd->str[18] = '-';
    this->__pd->str[19] = t[this->__pd->data[8] >> 4];
    this->__pd->str[20] = t[this->__pd->data[8] & 0x0F];
    this->__pd->str[21] = t[this->__pd->data[9] >> 4];
    this->__pd->str[22] = t[this->__pd->data[9] & 0x0F];
    this->__pd->str[23] = '-';
    this->__pd->str[24] = t[this->__pd->data[10] >> 4];
    this->__pd->str[25] = t[this->__pd->data[10] & 0x0F];
    this->__pd->str[26] = t[this->__pd->data[11] >> 4];
    this->__pd->str[27] = t[this->__pd->data[11] & 0x0F];
    this->__pd->str[28] = t[this->__pd->data[12] >> 4];
    this->__pd->str[29] = t[this->__pd->data[12] & 0x0F];
    this->__pd->str[30] = t[this->__pd->data[13] >> 4];
    this->__pd->str[31] = t[this->__pd->data[13] & 0x0F];
    this->__pd->str[32] = t[this->__pd->data[14] >> 4];
    this->__pd->str[33] = t[this->__pd->data[14] & 0x0F];
    this->__pd->str[34] = t[this->__pd->data[15] >> 4];
    this->__pd->str[35] = t[this->__pd->data[15] & 0x0F];
}

UUID::UUID(const char *str) : __pd(nullptr)
{
    std::string __str(str);
    UUIDException e;

    if (!validate_uuid_string(str, &e)) {
        throw e;
    }

    this->__pd = new __UUIDPrivData;
    ashe::trim(__str);
    ashe::lower(__str);

    for (auto &v : __str) {
        if ('0' <= v && v <= '9') {
            v -= '0';
        }
        else if ('a' <= v && v <= 'z') {
            v -= 'a' - 10;
        }
    }

    this->__pd->data[0] = (uint8_t)((__str[0] << 4) | __str[1]);
    this->__pd->data[1] = (uint8_t)((__str[2] << 4) | __str[3]);
    this->__pd->data[2] = (uint8_t)((__str[4] << 4) | __str[5]);
    this->__pd->data[3] = (uint8_t)((__str[6] << 4) | __str[7]);
    this->__pd->data[4] = (uint8_t)((__str[9] << 4) | __str[10]);
    this->__pd->data[5] = (uint8_t)((__str[11] << 4) | __str[12]);
    this->__pd->data[6] = (uint8_t)((__str[14] << 4) | __str[15]);
    this->__pd->data[7] = (uint8_t)((__str[16] << 4) | __str[17]);
    this->__pd->data[8] = (uint8_t)((__str[19] << 4) | __str[20]);
    this->__pd->data[9] = (uint8_t)((__str[21] << 4) | __str[22]);
    this->__pd->data[10] = (uint8_t)((__str[24] << 4) | __str[25]);
    this->__pd->data[11] = (uint8_t)((__str[26] << 4) | __str[27]);
    this->__pd->data[12] = (uint8_t)((__str[28] << 4) | __str[29]);
    this->__pd->data[13] = (uint8_t)((__str[30] << 4) | __str[31]);
    this->__pd->data[14] = (uint8_t)((__str[32] << 4) | __str[33]);
    this->__pd->data[15] = (uint8_t)((__str[34] << 4) | __str[35]);

    this->__mkStr();
}

UUID::UUID(const uint8_t *buf) : __pd(nullptr)
{
    UUIDException e;

    if (!validate_uuid_binary(buf, &e)) {
        throw e;
    }

    this->__pd = new __UUIDPrivData;
    ::memcpy(this->__pd->data, buf, RAW_BYTE_SIZE);

    this->__mkStr();
}

UUID::UUID(const thisClass &src) LASHE_NOEXCEPT
    : __pd(new __UUIDPrivData(*src.__pd))
{
}

UUID::~UUID() LASHE_NOEXCEPT {}

UUID &UUID::operator=(const thisClass &src) LASHE_NOEXCEPT
{
    *this->__pd = *src.__pd;
    return *this;
}

const char *UUID::className() const LASHE_NOEXCEPT { return "UUID"; }

bool UUID::operator==(const thisClass &x) const LASHE_NOEXCEPT
{
    return ::memcmp(x.__pd->data, this->__pd->data, RAW_BYTE_SIZE) == 0;
}

bool UUID::operator!=(const thisClass &x) const LASHE_NOEXCEPT
{
    return ::memcmp(x.__pd->data, this->__pd->data, RAW_BYTE_SIZE) != 0;
}

UUID::Version UUID::version() const LASHE_NOEXCEPT
{
    const auto ret = (Version)(this->__pd->data[6] >> 4);
    return tostr(ret) == nullptr ? Version::NONE : ret;
}

const char *UUID::str() const LASHE_NOEXCEPT { return this->__pd->str.c_str(); }

const uint8_t *UUID::data() const LASHE_NOEXCEPT { return this->__pd->data; }

/******************
 * RandomUUIDEngineInterface
 */
RandomUUIDEngineInterface::~RandomUUIDEngineInterface() LASHE_NOEXCEPT {}

UUID RandomUUIDEngineInterface::operator()() LASHE_NOEXCEPT
{
    return this->generate();
}

/******************
 * MersenneTwisterUUIDEngine
 */
struct __MersenneTwisterUUIDEnginePrivData {
    uint64_t poolSize = 32;
    uint64_t cnt = 0;
    MessageDigest md;
    std::vector<uint64_t> pool;
};

MersenneTwisterUUIDEngine::MersenneTwisterUUIDEngine() : __pd(nullptr)
{
    try {
        this->__pd = new __MersenneTwisterUUIDEnginePrivData;
        this->__pd->md.open(HashAlgorithm::SHA1);
    }
    catch (Exception &e) {
        delete this->__pd;
        this->__pd = nullptr;
        throw e;
    }
}

MersenneTwisterUUIDEngine::~MersenneTwisterUUIDEngine() LASHE_NOEXCEPT
{
    delete this->__pd;
}

size_t MersenneTwisterUUIDEngine::poolSize() const LASHE_NOEXCEPT
{
    return this->__pd->poolSize;
}

MersenneTwisterUUIDEngine &
MersenneTwisterUUIDEngine::poolSize(const size_t size) LASHE_NOEXCEPT
{
    this->__pd->poolSize = size;
    return *this;
}

UUID MersenneTwisterUUIDEngine::generate() LASHE_NOEXCEPT
{
    uint8_t ret[UUID::RAW_BYTE_SIZE];

    if (this->__pd->pool.empty() ||
        this->__pd->cnt == std::numeric_limits<uint64_t>::max()) {
        this->randomise();
    }
    this->__pd->pool[0] = this->__pd->cnt;

    const auto buf = this->__pd->md
                         .feed((const uint8_t *)this->__pd->pool.data(),
                               this->__pd->pool.size() * sizeof(uint64_t))
                         .finish()
                         .payload();
    ::memcpy(ret, buf.data(), UUID::RAW_BYTE_SIZE);
    __uuid::put_glamour(ret, UUID::Version::RANDOM);

    this->__pd->cnt += 1;

    return ret;
}

MersenneTwisterUUIDEngine &MersenneTwisterUUIDEngine::randomise() LASHE_NOEXCEPT
{
    std::vector<uint64_t> pool(4 + this->__pd->poolSize);
    std::hash<std::thread::id> hasher;
    size_t i;
    const auto now =
        (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
    const auto pid = (uint64_t)__lashe::get_pid();
    const auto tid_hash = (uint64_t)hasher(std::this_thread::get_id());

    pool[1] = now;
    pool[2] = pid;
    pool[3] = tid_hash;

    try {
        std::random_device rnd;

        if (rnd.entropy() == 0) {
            throw std::exception();
        }

        for (i = 0; i < this->__pd->poolSize; i += 1) {
            pool[i + 4] = ((uint64_t)rnd() << 32) | (uint64_t)rnd();
        }
    }
    catch (std::exception &) {
        std::mt19937_64 rnd;
        std::seed_seq seq({now, pid, tid_hash});

        rnd.seed(seq);
        for (i = 0; i < this->__pd->poolSize; i += 1) {
            pool[i + 4] = rnd();
        }
    }

    this->__pd->cnt = 0;

    return *this;
}

/********************
 * Library Functions
 */

bool validate_uuid_string(const char *str, UUIDException *e /* = nullptr*/)
{
    const std::string uuid = trim(str);
    auto set_exception = [&e](const char *msg) {
        if (e != nullptr) {
            *e = UUIDException(msg);
        }
    };

    __lashe::ensure_init();

    if (!std::regex_match(uuid, __lashe::regex->uuid_husk)) {
        set_exception("invalid UUID format.");
        return false;
    }

    switch (uuid[19]) {
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'A':
    case 'B':
        break;
    default:
        set_exception("invalid magic digit.");
        return false;
    }

    return true;
}

bool validate_uuid_binary(const uint8_t *p, UUIDException *e /* = nullptr*/)
{
    switch (p[8] & 0xF0) {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
        break;
    default:
        if (e != nullptr) {
            *e = UUIDException("invalid magic digit.");
        }
        return false;
    }

    return true;
}

} // namespace ashe
