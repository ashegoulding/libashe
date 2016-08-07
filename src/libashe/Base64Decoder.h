#ifndef LASHE_BASE64DECODER_H_
#define LASHE_BASE64DECODER_H_

#include "FilterInterface.h"


namespace ashe
{
#pragma pack(push, LASHE_PUB_ALIGN)

struct __Base64DecoderContext;

class Base64Decoder : public FilterInterface
{
public:
	typedef FilterInterface motherClass;
	typedef Base64Decoder thisClass;

protected:
	friend FilterInterface* mkBase64Decoder(const /* Base64DecodeFlags */uint32_t *flags/* = nullptr*/) LASHE_EXCEPT(FilterException);

	__Base64DecoderContext *__ctx;

	Base64Decoder() LASHE_NOEXCEPT;

	virtual void __prepParamPool(const char *key = nullptr) const LASHE_NOEXCEPT; //@Override
	virtual void __prepParamMap() const LASHE_NOEXCEPT; //@Override

public:
	Base64Decoder(const thisClass&) = delete;
	virtual ~Base64Decoder() LASHE_NOEXCEPT;

	thisClass &operator =(const thisClass&) = delete;

	virtual const char *className() const LASHE_NOEXCEPT; //@Override

	virtual thisClass &open(const uint32_t x) LASHE_EXCEPT(FilterException); //@Implement
	virtual thisClass &close() LASHE_NOEXCEPT; //@Implement
	virtual bool ready() const LASHE_NOEXCEPT; //@Implement
	/* param()
	* Key "URL":
	*	Value format: <boolean>
	*	Default value: false
	*	Treats the fed string value as an URL encoded string.
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

enum /* uint32_t */ Base64DecodeFlags
{
	LB64DF_NONE,
	LB64DF_URL
};

LASHE_DECL_EXT FilterInterface *mkBase64Decoder(const /* Base64DecodeFlags */uint32_t *flags = nullptr) LASHE_EXCEPT(FilterException);
// In-memory decoding.
LASHE_DECL_EXT FilterResult base64Decode(const void *buf, const size_t len, const /* Base64EncodeFlags */ uint32_t *flags = nullptr) LASHE_EXCEPT(FilterException);

}

#endif
