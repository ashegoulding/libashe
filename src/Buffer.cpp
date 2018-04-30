#include "__internal.hpp"
#include "libashe/Buffer.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>


namespace ashe {

struct __BufferPrivData {
    uint8_t *ptr = nullptr;
    size_t size = 0;
};

Buffer::Buffer(const size_t size /* = 0*/) LASHE_NOEXCEPT
    : __pd(new __BufferPrivData)
{
    this->alloc(size);
}

Buffer::Buffer(const thisClass &x) LASHE_NOEXCEPT : __pd(new __BufferPrivData)
{
    this->alloc(x.__pd->size);
    std::memcpy(this->__pd->ptr, x.__pd->ptr, x.__pd->size);
}

Buffer::Buffer(thisClass &&x) LASHE_NOEXCEPT : __pd(x.__pd)
{
    x.__pd = new __BufferPrivData;
}

Buffer::~Buffer() LASHE_NOEXCEPT
{
    std::free(this->__pd->ptr);
    delete this->__pd;
}

const char *Buffer::className() const LASHE_NOEXCEPT { return "Buffer"; }

Buffer &Buffer::operator=(const thisClass &x) LASHE_NOEXCEPT
{
    const auto size = x.size();

    this->alloc(size);
    std::memcpy(this->__pd->ptr, x.data(), size);

    return *this;
}

Buffer &Buffer::operator=(thisClass &&x) LASHE_NOEXCEPT
{
    this->free();

    delete this->__pd;
    this->__pd = x.__pd;
    x.__pd = new __BufferPrivData;

    return *this;
}

size_t Buffer::size() const LASHE_NOEXCEPT {
    return this->__pd->size;
}

const uint8_t *Buffer::data() const LASHE_NOEXCEPT {
    return this->__pd->ptr;
}

uint8_t *Buffer::data() LASHE_NOEXCEPT {
    return this->__pd->ptr;
}

Buffer &Buffer::alloc(const size_t size) LASHE_NOEXCEPT
{
    std::free(this->__pd->ptr);

    if (size) {
        this->__pd->ptr = (uint8_t *)::malloc(size);
        if (nullptr == this->__pd->ptr) {
            __lashe::die_critical();
        }
    }
    else {
        this->__pd->ptr = nullptr;
    }
    this->__pd->size = size;

    return *this;
}

Buffer &Buffer::free() LASHE_NOEXCEPT
{
    std::free(this->__pd->ptr);
    this->__pd->ptr = nullptr;
    this->__pd->size = 0;

    return *this;
}

Buffer &Buffer::swap(thisClass &x) LASHE_NOEXCEPT
{
    std::swap(x.__pd, this->__pd);
    return *this;
}

} // namespace ashe
