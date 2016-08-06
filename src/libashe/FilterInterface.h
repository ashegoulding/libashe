#ifndef LASHE_DIGESTINTERFACE_H_
#define LASHE_DIGESTINTERFACE_H_

#include "AsheClass.h"
#include "AsheException.h"


namespace ashe
{
#pragma pack(push, LASHE_PUB_ALIGN)

class LASHE_DECL_EXT FilterException : public AsheException
{
public:
	typedef AsheException motherClass;
	typedef FilterException thisClass;

	enum /* uint32_t */ Code
	{
		C_UNDEFINED,
		C_ABILITY_UNINITIALISED,
		C_NOT_READY,
		C_ILLEGAL_ARGUMENT,
		C_NO_DATA,
		C_SHORT_BUFFER,
		C_UNOPENED,
		C_ERROR_FROM_LIB
	};

public:
	virtual ~FilterException() LASHE_NOEXCEPT;
	virtual const char *code2str(const uint32_t x) const LASHE_NOEXCEPT;
};

class LASHE_DECL_EXT FilterInterface : public AsheClass
{
public:
	typedef AsheClass motherClass;
	typedef FilterInterface thisClass;

protected:
	size_t __pushed;

	virtual void __dropif_notReady() const LASHE_EXCEPT(FilterException);

public:
	FilterInterface() LASHE_NOEXCEPT;
	FilterInterface(const thisClass&) = delete;
	virtual ~FilterInterface() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;
	// Calls ready().
	operator bool() LASHE_NOEXCEPT;

	virtual thisClass &open(const uint32_t x) LASHE_EXCEPT(FilterException) = 0; //@Pure
	virtual thisClass &close() LASHE_NOEXCEPT = 0; //@Pure
	virtual bool ready() const LASHE_NOEXCEPT = 0; //@Pure

	virtual thisClass &feed(const void *buf, const size_t len) LASHE_EXCEPT(FilterException) = 0; //@Pure
	virtual thisClass &finish() LASHE_EXCEPT(FilterException) = 0; //@Pure
	virtual thisClass &clear() LASHE_NOEXCEPT = 0; //@Pure
	virtual size_t pushed() const LASHE_NOEXCEPT;

	virtual bool hasPayload() const LASHE_NOEXCEPT = 0; //@Pure
	virtual size_t payloadSize() const LASHE_EXCEPT(FilterException) = 0; //@Pure
	virtual thisClass &payload(void *buf, const size_t len) LASHE_EXCEPT(FilterException) = 0; //@Pure
};

#pragma pack(pop)
}

#endif
