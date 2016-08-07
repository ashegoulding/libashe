#ifndef LASHE_MESSAGEDIGEST_H_
#define LASHE_MESSAGEDIGEST_H_

#include "FilterInterface.h"


namespace ashe
{

enum /* uint32_t */ HashAlgorithm
{
	LAHA_NONE,
	LAHA_MD5,
	LAHA_SHA1,
	LAHA_SHA224,
	LAHA_SHA256,
	LAHA_SHA384,
	LAHA_SHA512
};
#pragma pack(push, LASHE_PUB_ALIGN)

struct __MessageDigestContext;

class MessageDigest : public FilterInterface
{
public:
	typedef FilterInterface motherClass;
	typedef MessageDigest thisClass;

protected:
	friend FilterInterface *mkMessageDigest(const /* HashAlgorithm */ uint32_t algo, const uint8_t *rsv/* = nullptr*/) LASHE_EXCEPT(FilterException);

	__MessageDigestContext *__ctx;

	MessageDigest() LASHE_NOEXCEPT;

	virtual void __prepParamPool(const char *key = nullptr) const LASHE_NOEXCEPT; //@Override
	virtual void __prepParamMap() const LASHE_NOEXCEPT; //@Override

public:
	MessageDigest(const thisClass &) = delete;
	virtual ~MessageDigest() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override

	virtual thisClass &open(const uint32_t x) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &close() LASHE_NOEXCEPT; //@Implement
	virtual bool ready() const LASHE_NOEXCEPT; //@Implement
	/* param()
	* Key "string":
	*	Value: <hash string>
	*	A string representation of the result hash(payload).
	*	This kv pair is available when hasPayload() returns true.
	*
	* Key "upper":
	*	Value: <boolean>
	*	Default: false
	*	Sets/gets whether to capitalise "string" param value.
	*/
	virtual thisClass &param(const char *key, const char *val, const /* ParamFlag */ uint32_t *flags = nullptr) LASHE_EXCEPT(FilterException); //@Override
	virtual size_t param(const char *key, const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags = nullptr) const LASHE_NOEXCEPT; //@Override
	virtual size_t param(const ParamEntry **arr, const /* ParamFlag */ uint32_t *flags = nullptr) const LASHE_NOEXCEPT; //@Override
	virtual thisClass &feed(const void *buf, const size_t len) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &finish() LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &clear() LASHE_NOEXCEPT; //@Implement
	virtual bool hasPayload() const LASHE_NOEXCEPT; //@Implement
	virtual size_t payloadSize() const LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &payload(void *buf, const size_t len) LASHE_EXCEPT(FilterException); //@Implement
};

#pragma pack(pop)

enum MessageDigestFlags
{
	LAMDF_NONE,
	// Have digest() return a hash string.
	LAMDF_STRING,
	LAMDF_UPPER
};

LASHE_DECL_EXT FilterInterface *mkMessageDigest(const /* HashAlgorithm */ uint32_t algo, const /* MessageDigestFlags */ uint8_t *flags = nullptr) LASHE_EXCEPT(FilterException);
// In-memory.
LASHE_DECL_EXT FilterResult digest(const void *buf, const size_t len, const /* HashAlgorithm */ uint32_t algo, const /* MessageDigestFlags */ uint8_t *flags = nullptr) LASHE_EXCEPT(FilterException);

}

#endif
