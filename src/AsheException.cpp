#include "libashe/AsheException.h"

#include <string>

#include <cstdlib>
#include <cstring>
#include <sstream>

namespace ashe
{

struct __AsheExceptionCtx
{
	std::string msg;
	std::string str[2];
	std::string whatStr;
	bool invalidated = true;
};
#define __INTP_AECTX() auto &__ctx = *(__AsheExceptionCtx*)this->__privCtx

AsheException::AsheException() LASHE_NOEXCEPT
	: __privCtx(new __AsheExceptionCtx)
	, __extraData(nullptr)
	, __extraDataSize(0)
	, __code(0)
{
	this->__arg[0] = this->__arg[1] = 0;
}

AsheException::AsheException(const thisClass& x) LASHE_NOEXCEPT
	: __privCtx(new __AsheExceptionCtx)
	, __extraData(nullptr)
	, __extraDataSize(0)
	, __code(0)
{
	this->__arg[0] = this->__arg[1] = 0;
	this->__construct(x);
}

AsheException::~AsheException() LASHE_NOEXCEPT
{
	::free(this->__extraData);
}

const char* AsheException::className() const LASHE_NOEXCEPT
{
	static const char *__name__ = "ashe::AsheException";
	return __name__;
}

AsheException& AsheException::operator =(const thisClass& x) LASHE_NOEXCEPT
{
	::free(this->__extraData);
	this->__construct(x);
	return *this;
}

uint32_t AsheException::code() const LASHE_NOEXCEPT
{
	return this->__code;
}

AsheException& AsheException::code(const uint32_t x) LASHE_NOEXCEPT
{
	this->__code = x;
	this->__invalidate();
	return *this;
}

const char* AsheException::what() const LASHE_NOEXCEPT
{
	__INTP_AECTX();

	if(__ctx.invalidated)
	{
		std::stringstream sb;

		sb << '[' << this->__code2str(this->__code) << "] " << __ctx.msg;
		__ctx.whatStr = sb.str();
		__ctx.invalidated = false;
	}
	return __ctx.whatStr.c_str();
}

const char* AsheException::msg() const LASHE_NOEXCEPT
{
	__INTP_AECTX();
	return __ctx.msg.c_str();
}

AsheException& AsheException::msg(const char* x) LASHE_NOEXCEPT
{
	__INTP_AECTX();
	__ctx.msg = x;
	this->__invalidate();
	return *this;
}

int64_t AsheException::arg1() const LASHE_NOEXCEPT
{
	return this->__arg[0];
}

AsheException& AsheException::arg1(int64_t x) LASHE_NOEXCEPT
{
	this->__arg[0] = x;
	this->__invalidate();
	return *this;
}

int64_t AsheException::arg2() const LASHE_NOEXCEPT
{
	return this->__arg[1];
}

AsheException& AsheException::arg2(int64_t x) LASHE_NOEXCEPT
{
	this->__arg[1] = x;
	this->__invalidate();
	return *this;
}

const char* AsheException::str1() const LASHE_NOEXCEPT
{
	__INTP_AECTX();
	return __ctx.str[0].c_str();
}

AsheException& AsheException::str1(const char* x) LASHE_NOEXCEPT
{
	__INTP_AECTX();
	__ctx.str[0] = x;
	this->__invalidate();
	return *this;
}

const char* AsheException::str2() const LASHE_NOEXCEPT
{
	__INTP_AECTX();
	return __ctx.str[1].c_str();
}

AsheException& AsheException::str2(const char* x) LASHE_NOEXCEPT
{
	__INTP_AECTX();
	__ctx.str[1] = x;
	this->__invalidate();
	return *this;
}

size_t AsheException::extraDataSize() const LASHE_NOEXCEPT
{
	return this->__extraDataSize;
}

const AsheException& AsheException::extraData(void* buf) const LASHE_NOEXCEPT
{
	::memcpy(buf, this->__extraData, this->__extraDataSize);
	return *this;
}

AsheException& AsheException::extraData(const void* buf, const size_t size) LASHE_NOEXCEPT
{
	if((this->__extraDataSize = size))
	{
		::free(this->__extraData);
		this->__extraData = ::malloc(this->__extraDataSize);
		::memcpy(this->__extraData, buf, this->__extraDataSize);
	}
	else
		this->__extraData = nullptr;
	this->__invalidate();
	return *this;
}

void AsheException::__construct(const thisClass& x) LASHE_NOEXCEPT
{
	__INTP_AECTX();
	auto &__theirs = *(__AsheExceptionCtx*)x.__privCtx;

	__ctx = __theirs;
	this->__arg[0] = x.__arg[0];
	this->__arg[1] = x.__arg[1];
	this->__code = x.__code;
	// Extra data.
	if((this->__extraDataSize = x.__extraDataSize))
	{
		this->__extraData = ::malloc(this->__extraDataSize);
		::memcpy(this->__extraData, x.__extraData, this->__extraDataSize);
	}
	else
		this->__extraData = nullptr;
}

void AsheException::__invalidate() LASHE_NOEXCEPT
{
	__INTP_AECTX();
	__ctx.invalidated = true;
}

}
