#ifndef LASHE_FILTERINTERFACE_HPP_
#define LASHE_FILTERINTERFACE_HPP_
#include "Buffer.hpp"
#include "Class.hpp"
#include "Exception.hpp"


namespace __lashe {
struct FilterInterfacePrivData;
}

namespace ashe {

#pragma pack(push, LASHE_PUB_ALIGN)

LASHE_DECL_EXCEPTION(FilterException, Exception);

class LASHE_DECL_EXT FilterInterface {
public:
    typedef FilterInterface thisClass;

    static void __drop_unimplemented__();
    static void __drop_shortBuf__();
    static void __dropif_nullBuf__(const void *buf);

protected:
    __lashe::FilterInterfacePrivData *__pd;

    FilterInterface() LASHE_NOEXCEPT;

    virtual void __dropif_ready(const bool x) const;

public:
    FilterInterface(const thisClass &) = delete;
    FilterInterface(thisClass &&) = delete;
    virtual ~FilterInterface() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &) = delete;
    thisClass &operator=(thisClass &&) = delete;
    // Calls ready().
    operator bool() LASHE_NOEXCEPT;

    virtual thisClass &close() LASHE_NOEXCEPT = 0;
    virtual bool ready() const LASHE_NOEXCEPT = 0;

    virtual thisClass &feed(const uint8_t *buf, const size_t len) = 0;
    virtual thisClass &finish() = 0;
    virtual thisClass &clear() LASHE_NOEXCEPT = 0;
    virtual size_t pushed() const LASHE_NOEXCEPT;
    virtual const Buffer &payload() const LASHE_NOEXCEPT = 0;
    virtual Buffer &&payload() LASHE_NOEXCEPT = 0;
};

#pragma pack(pop)
}

#endif
