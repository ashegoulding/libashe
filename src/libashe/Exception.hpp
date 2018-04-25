#ifndef LASHE_EXCEPTION_HPP_
#define LASHE_EXCEPTION_HPP_
#include "Class.hpp"


namespace ashe {

#define LASHE_DECL_EXCEPTION(NAME, MOTHER)                                     \
    \
class LASHE_DECL_EXT NAME : public MOTHER                                      \
    {                                                                          \
    public:                                                                    \
        typedef NAME thisClass;                                                \
                                                                               \
    public:                                                                    \
        NAME(const char *msg = nullptr, const Exception *inner = nullptr)      \
        LASHE_NOEXCEPT;                                                        \
        NAME(const thisClass &x) LASHE_NOEXCEPT;                               \
        virtual ~NAME() LASHE_NOEXCEPT;                                        \
        virtual const char *className() const LASHE_NOEXCEPT override;         \
    }

#define LASHE_IMPL_EXCEPTION(NAME, MOTHER)                                     \
    NAME::NAME(const char *msg /* = nullptr*/,                                 \
               const Exception *inner /* = nullptr*/) LASHE_NOEXCEPT           \
        : MOTHER(msg, inner)                                                   \
    {                                                                          \
    }                                                                          \
    NAME::NAME(const thisClass &x) LASHE_NOEXCEPT : MOTHER(x) {}               \
    NAME::~NAME() LASHE_NOEXCEPT {}                                            \
    const char *NAME::className() const LASHE_NOEXCEPT                         \
    {                                                                          \
        return "\"" #NAME "\"";                                                \
    }

#pragma pack(push, LASHE_PUB_ALIGN)

struct __ExceptionPrivData;

class LASHE_DECL_EXT Exception : public Class {
public:
    typedef Exception thisClass;

protected:
    __ExceptionPrivData *__pd;

    void __mkWhatStr(const char *msg, const Exception *inner) LASHE_NOEXCEPT;

public:
    Exception(const char *msg = nullptr,
              const Exception *inner = nullptr) LASHE_NOEXCEPT;
    Exception(const thisClass &x) LASHE_NOEXCEPT;
    virtual ~Exception() LASHE_NOEXCEPT;

    thisClass &operator=(const thisClass &x) LASHE_NOEXCEPT;

    virtual const char *className() const LASHE_NOEXCEPT override;
    virtual const char *what() const LASHE_NOEXCEPT;
};

#pragma pack(pop)
}

#endif
