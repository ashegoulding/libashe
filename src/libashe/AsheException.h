#ifndef LASHE_EXCEPTION_H_
#define LASHE_EXCEPTION_H_

#include "AsheClass.h"

namespace ashe
{

#pragma pack(push, LASHE_PUB_ALIGN)

class AsheException : public AsheClass
{
public:
	typedef AsheClass motherClass;
	typedef AsheException thisClass;

protected:
	void *__privCtx;
	void *__extraData;
	size_t __extraDataSize;
	int64_t __arg[2];
	/* Extra data.
	 * Memory allocated/deallocatable with ::malloc()/::free()
	 */
	uint32_t __code;

	virtual const char *__code2str(const uint32_t x) const LASHE_NOEXCEPT = 0;

	void __construct(const thisClass &x) LASHE_NOEXCEPT;
	void __invalidate() LASHE_NOEXCEPT;

public:
	AsheException() LASHE_NOEXCEPT;
	AsheException(const thisClass &x) LASHE_NOEXCEPT;
	virtual ~AsheException() LASHE_NOEXCEPT;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override

	virtual thisClass &operator =(const thisClass &x) LASHE_NOEXCEPT;

	virtual uint32_t code() const LASHE_NOEXCEPT;
	virtual thisClass &code(const uint32_t x) LASHE_NOEXCEPT;
	virtual const char *what() const LASHE_NOEXCEPT;
	virtual const char *msg() const LASHE_NOEXCEPT;
	virtual thisClass &msg(const char *x) LASHE_NOEXCEPT;
	virtual int64_t arg1() const LASHE_NOEXCEPT;
	virtual thisClass &arg1(int64_t x) LASHE_NOEXCEPT;
	virtual int64_t arg2() const LASHE_NOEXCEPT;
	virtual thisClass &arg2(int64_t x) LASHE_NOEXCEPT;
	virtual const char *str1() const LASHE_NOEXCEPT;
	virtual thisClass &str1(const char *x) LASHE_NOEXCEPT;
	virtual const char *str2() const LASHE_NOEXCEPT;
	virtual thisClass &str2(const char *x) LASHE_NOEXCEPT;
	virtual size_t extraDataSize() const LASHE_NOEXCEPT;
	virtual const thisClass &extraData(void *buf) const LASHE_NOEXCEPT;
	virtual thisClass &extraData(const void *buf, const size_t size) LASHE_NOEXCEPT;
};

#pragma pack(pop)

}

#endif
