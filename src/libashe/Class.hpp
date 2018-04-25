#ifndef LASHE_CLASS_HPP_
#define LASHE_CLASS_HPP_
#include "pub.hpp"


namespace ashe {

#pragma pack(push, LASHE_PUB_ALIGN)

/* AsheClass
 * The top-level class.
 */
class LASHE_DECL_EXT Class {
public:
    typedef Class thisClass;

public:
    virtual ~Class() LASHE_NOEXCEPT;

    virtual const char *className() const LASHE_NOEXCEPT = 0;
};

#pragma pack(pop)
}

#endif
