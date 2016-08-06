#ifndef LASHE_BASE64ENCODER_H_
#define LASHE_BASE64ENCODER_H_

#include "FilterInterface.h"


namespace ashe
{
#pragma pack(push, LASHE_PUB_ALIGN)

struct __Base64CodecContext;

class Base64Encoder : public FilterInterface
{
public:
	typedef FilterInterface motherClass;
	typedef Base64Encoder thisClass;

protected:
	friend FilterInterface *mkBase64Encoder() LASHE_EXCEPT(FilterException);

	__Base64CodecContext *__ctx;

	Base64Encoder() LASHE_NOEXCEPT;

public:
	Base64Encoder(const thisClass&) = delete;
	virtual ~Base64Encoder() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;

	virtual thisClass &open(const uint32_t x) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &close() LASHE_NOEXCEPT; //@Implement
	virtual bool ready() const LASHE_NOEXCEPT; //@Implement
	virtual thisClass &feed(const void *buf, const size_t len) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &finish() LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &clear() LASHE_NOEXCEPT; //@Implement
	virtual bool hasPayload() const LASHE_NOEXCEPT; //@Implement
	virtual size_t payloadSize() const LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &payload(void *buf, const size_t len) LASHE_EXCEPT(FilterException); //@Implement
};

#pragma pack(pop)

LASHE_DECL_EXT FilterInterface *mkBase64Encoder() LASHE_EXCEPT(FilterException);

}

#endif
