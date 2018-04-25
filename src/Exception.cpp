#include "libashe/Exception.hpp"

#include <sstream>

#include <cstdlib>
#include <cstring>


namespace ashe {

struct __ExceptionPrivData {
    std::string whatStr;
};

Exception::Exception(const char *msg /* = nullptr*/,
                     const Exception *inner /* = nullptr*/) LASHE_NOEXCEPT
    : __pd(new __ExceptionPrivData)
{
    this->__mkWhatStr(msg, inner);
}

Exception::Exception(const thisClass &x) LASHE_NOEXCEPT
    : __pd(new __ExceptionPrivData(*x.__pd))
{
}

Exception::~Exception() LASHE_NOEXCEPT { delete this->__pd; }

const char *Exception::className() const LASHE_NOEXCEPT { return "Exception"; }

Exception &Exception::operator=(const thisClass &x) LASHE_NOEXCEPT
{
    *this->__pd = *x.__pd;
    return *this;
}

const char *Exception::what() const LASHE_NOEXCEPT
{
    return this->__pd->whatStr.c_str();
}

void Exception::__mkWhatStr(const char *msg,
                            const Exception *inner) LASHE_NOEXCEPT
{
    std::stringstream ss;

    if (msg == nullptr) {
        ss << this->className();
    }
    else {
        ss << this->className() << ": " << msg;
    }

    if (inner != nullptr) {
        ss << std::endl << "caused by: " << inner->what();
    }

    this->__pd->whatStr = ss.str();
}

} // namespace ashe
