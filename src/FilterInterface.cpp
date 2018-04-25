#include "__openssl.hpp"
#include "libashe/FilterInterface.hpp"


namespace ashe {

LASHE_IMPL_EXCEPTION(FilterException, Exception);

FilterInterface::FilterInterface() LASHE_NOEXCEPT
    : __pd(new __lashe::FilterInterfacePrivData)
{
}

FilterInterface::~FilterInterface() LASHE_NOEXCEPT {}

FilterInterface::operator bool() LASHE_NOEXCEPT { return this->ready(); }

size_t FilterInterface::pushed() const LASHE_NOEXCEPT
{
    return this->__pd->pushed;
}

void FilterInterface::__dropif_ready(const bool x) const
{
    if (this->ready() ^ x) {
        throw FilterException("illegal state.");
    }
}

void FilterInterface::__drop_unimplemented__()
{
    throw FilterException("not implemented.");
}

void FilterInterface::__drop_shortBuf__()
{
    throw FilterException("short buffer.");
}

void FilterInterface::__dropif_nullBuf__(const void *buf)
{
    if (nullptr == buf) {
        throw FilterException("'buf' is given null.");
    }
}

} // namespace ashe
