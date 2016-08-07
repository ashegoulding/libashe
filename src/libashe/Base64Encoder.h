#ifndef LASHE_BASE64ENCODER_H_
#define LASHE_BASE64ENCODER_H_

#include "FilterInterface.h"


namespace ashe
{
#pragma pack(push, LASHE_PUB_ALIGN)

struct __Base64EncoderContext;

class Base64Encoder : public FilterInterface
{
public:
	typedef FilterInterface motherClass;
	typedef Base64Encoder thisClass;

protected:
	friend FilterInterface *mkBase64Encoder(const /* Base64EncodeFlags */ uint32_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException);

	__Base64EncoderContext *__ctx;

	Base64Encoder() LASHE_NOEXCEPT;

	virtual void __prepParamPool(const char *key = nullptr) const LASHE_NOEXCEPT; //@Override
	virtual void __prepParamMap() const LASHE_NOEXCEPT; //@Override

public:
	Base64Encoder(const thisClass&) = delete;
	virtual ~Base64Encoder() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override

	virtual thisClass &open(const uint32_t x) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &close() LASHE_NOEXCEPT; //@Implement
	virtual bool ready() const LASHE_NOEXCEPT; //@Implement
	/* param()
	* Key "BIO_FLAGS_BASE64_NO_NL":
	*	Value format: <boolean>
	*	Default value: false
	*	Formats the output string to fixed number of characters per line if set to false.
	*	Setting the param, an exception will be thrown if the instance is already opened.
	* Key "URL":
	*	Value format: <boolean>
	*	Default value: false
	*	URL encodes the result string(payload).
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

enum /* uint32_t */ Base64EncodeFlags
{
	LB64EF_NONE,
	LB64EF_URL,
	LB64EF_NO_NL
};

LASHE_DECL_EXT FilterInterface *mkBase64Encoder(const /* Base64EncodeFlags */ uint32_t *flags = nullptr) LASHE_EXCEPT(FilterException);
// In-memory encoding.
LASHE_DECL_EXT FilterResult base64Encode(const void *buf, const size_t len, const /* Base64EncodeFlags */ uint32_t *flags = nullptr) LASHE_EXCEPT(FilterException);

}

#endif
