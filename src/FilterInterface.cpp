#include "libashe/FilterInterface.h"
#include "__openssl.h"

#include <algorithm>


namespace ashe
{

/********************
* FilterInterface
*/
FilterInterface::FilterInterface() LASHE_NOEXCEPT
	: __pushed(0)
{
}

FilterInterface::~FilterInterface() LASHE_NOEXCEPT
{
}

FilterInterface::operator bool() LASHE_NOEXCEPT
{
	return this->ready();
}

FilterInterface &FilterInterface::param(const char *key, const char *val, const /* ParamFlag */ uint32_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException)
{
	thisClass::__drop_unimplemented__();
	return *this;
}

size_t FilterInterface::param(const char *key, const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags/* = nullptr*/) const LASHE_NOEXCEPT
{
	return 0;
}

size_t FilterInterface::param(const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags/* = nullptr*/) const LASHE_NOEXCEPT
{
	return 0;
}

size_t FilterInterface::pushed() const LASHE_NOEXCEPT
{
	return this->__pushed;
}

void FilterInterface::__dropif_ready(const bool x) const LASHE_EXCEPT(FilterException)
{
	if(this->ready() ^ x)
	{
		FilterException e;
		e.code(FilterException::C_ILLEGAL_STATE);
		throw e;
	}
}

void FilterInterface::__prepParamPool(const char *key/* = nullptr*/) const LASHE_NOEXCEPT
{
}

void FilterInterface::__prepParamMap() const LASHE_NOEXCEPT
{
}

void FilterInterface::__drop_unimplemented__() LASHE_EXCEPT(FilterException)
{
	FilterException e;
	e.code(FilterException::C_UNIMPLEMENTED);
	throw e;
}

void FilterInterface::__drop_shortBuf__() LASHE_EXCEPT(FilterException)
{
	FilterException e;
	e.code(FilterException::C_SHORT_BUFFER);
	throw e;
}

void FilterInterface::__dropif_nullBuf__(const void *buf) LASHE_EXCEPT(FilterException)
{
	if(nullptr == buf)
	{
		FilterException e;
		e
			.code(FilterException::C_ILLEGAL_ARGUMENT)
			.msg("'buf' is given null.");
		throw e;
	}
}

void FilterInterface::__dropif_nullParam__(const char *key) LASHE_EXCEPT(FilterException)
{
	if(nullptr == key)
	{
		FilterException e;
		e
			.code(FilterException::C_ILLEGAL_ARGUMENT)
			.msg("'key' given null.");
		throw e;
	}
}

void FilterInterface::__dropif_nullParam__(const char *key, const char *val) LASHE_EXCEPT(FilterException)
{
	if(nullptr == key && nullptr == val)
	{
		FilterException e;
		e
			.code(FilterException::C_ILLEGAL_ARGUMENT)
			.msg("'key' and 'val' given null.");
		throw e;
	}
	else if(nullptr == val)
	{
		FilterException e;
		e
			.code(FilterException::C_ILLEGAL_ARGUMENT)
			.msg("'val' given null.");
		throw e;
	}
	else
		thisClass::__dropif_nullParam__(key);
}

/********************
* FilterException
*/
FilterException::~FilterException() LASHE_NOEXCEPT
{
}

const char *FilterException::code2str(const uint32_t x) const LASHE_NOEXCEPT
{
	static const char *__str__[] = {
			"undefined",
			"ability uninitialised",
			"unimplemented",
			"illegal state",
			"illegal argument",
			"no data",
			"short buffer",
			"error from library",
			"no subject",
			"invalid format"
	};
	switch(x)
	{
	case C_UNDEFINED:
	case C_ABILITY_UNINITIALISED:
	case C_UNIMPLEMENTED:
	case C_ILLEGAL_STATE:
	case C_ILLEGAL_ARGUMENT:
	case C_NO_DATA:
	case C_SHORT_BUFFER:
	case C_ERROR_FROM_LIB:
	case C_NO_SUBJECT:
	case C_INVALID_FORMAT:
		return __str__[x];
	}
	return motherClass::code2str(x);
}
/********************
* FilterResult
*/
FilterResult::FilterResult(const size_t size/* = 0*/) LASHE_NOEXCEPT
	: ptr(size? (char*)::malloc(size) : nullptr)
	, size(size)
{
	if(this->size && nullptr == this->ptr)
		__die_critical();
}

FilterResult::FilterResult(thisClass&& x) LASHE_NOEXCEPT
	: ptr(x.ptr)
	, size(x.size)
{
	x.__detach();
}

FilterResult::~FilterResult() LASHE_NOEXCEPT
{
	::free(this->ptr);
}

FilterResult& FilterResult::operator =(thisClass&& x) LASHE_NOEXCEPT
{
	this->free();
	this->ptr = x.ptr;
	this->size = x.size;
	x.__detach();
	return *this;
}

void FilterResult::__detach() LASHE_NOEXCEPT
{
	this->ptr = nullptr;
	this->size = 0;
}

FilterResult& FilterResult::alloc(const size_t size) LASHE_NOEXCEPT
{
	::free(this->ptr);
	if(size)
	{
		this->ptr = (char*)::malloc(size);
		if(nullptr == this->ptr)
			__die_critical();
	}
	else
		this->ptr = nullptr;
	this->size = size;
	return *this;
}

FilterResult& FilterResult::free() LASHE_NOEXCEPT
{
	::free(this->ptr);
	this->__detach();
	return *this;
}

FilterResult& FilterResult::swap(thisClass& x) LASHE_NOEXCEPT
{
	std::swap(x.ptr, this->ptr);
	std::swap(x.size, this->size);
	return *this;
}

FilterResult FilterResult::clone() LASHE_NOEXCEPT
{
	FilterResult ret;
	if(this->size)
	{
		ret.alloc(this->size);
		::memcpy(ret.ptr, this->ptr, this->size);
	}
	return ret;
}

}
