#ifndef LASHE_BUFFER_HPP_
#define LASHE_BUFFER_HPP_
#include "Class.hpp"
#include "pub.hpp"


namespace ashe {

struct __BufferPrivData;

/* Simple buffer holder class that wraps around malloc() and free()
 */
class Buffer : public Class {
public:
    typedef Class motherClass;
    typedef Buffer thisClass;

protected:
    // Managed with ::malloc() and ::free().
    __BufferPrivData *__pd;

public:
    Buffer(const size_t size = 0) LASHE_NOEXCEPT;
    // Move constructor.
    Buffer(const thisClass &x) LASHE_NOEXCEPT;
    Buffer(thisClass &&x) LASHE_NOEXCEPT;
    virtual ~Buffer() LASHE_NOEXCEPT;

    virtual const char *className() const LASHE_NOEXCEPT override;

    thisClass &operator=(const thisClass &x) LASHE_NOEXCEPT;
    thisClass &operator=(thisClass &&x) LASHE_NOEXCEPT;

    virtual size_t size() const LASHE_NOEXCEPT;
    virtual const uint8_t *data() const LASHE_NOEXCEPT;
    virtual uint8_t *data() LASHE_NOEXCEPT;

    // This will call ::free() first. 0 is an acceptable argument.
    virtual thisClass &alloc(const size_t size) LASHE_NOEXCEPT;
    // Frees 'ptr' and calls detach().
    virtual thisClass &free() LASHE_NOEXCEPT;
    virtual thisClass &swap(thisClass &x) LASHE_NOEXCEPT;
};

} // namespace ashe

#endif
