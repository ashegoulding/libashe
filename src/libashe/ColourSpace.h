#ifndef ASHE_COLOURSPACE_H_
#define ASHE_COLOURSPACE_H_

#include <src/libashe/ManagedBinary.h>
#include <src/libashe/MFCLinkage.h>
#include <string>
#include <sstream>

#include <exception>

#include <cstdint>

namespace ashe
{

class ASHE_DECL_EXT ColourSpace
{
public:
	typedef ColourSpace thisClass;
	enum ColourFormat
	{
		CF_NONE,
		CF_GREYSCALE,
		CF_RGB,
		CF_RGBA,
		CF_YUV422,
		CF_YUV422P
	};

	class ASHE_DECL_EXT Exception : public std::exception
	{
	public:
		typedef std::exception motherClass;
		typedef Exception thisClass;

		enum Code
		{
			C_UNDEFINED,
			C_NOT_NECESSARY,
			C_NULL_POINTER,
			C_SHORT_LENGTH,
			C_ILLEGAL_ARGUMENT
		};

		static std::string codeToString__(const Code code) throw();

	protected:
		std::string whatString;
		Code code;

		void __construct(const thisClass &src) throw();

	public:
		Exception(const Code code = C_UNDEFINED, const std::string msg = "") throw();
		Exception(const thisClass &src) throw();
		virtual ~Exception() throw();

		thisClass &operator =(const thisClass &src) throw();

		virtual const char *what() const throw(); // @Override

		virtual Code getCode() const throw();
		virtual std::string toString() const throw(); // Alias of 'what()'
	};

protected:	
	uint32_t width, height;
	uint64_t manoeuvreCount;
	size_t fromSize;
	ColourFormat from, to;

	virtual void __construct(const thisClass &src) throw();

	virtual void __GREYSCALEToRGB(const uint8_t *data, const size_t size) throw();
	virtual void __GREYSCALEToRGBA(const uint8_t *data, const size_t size) throw();
	virtual void __GREYSCALEToYUV422(const uint8_t *data, const size_t size) throw();
	virtual void __GREYSCALEToYUV422P(const uint8_t *data, const size_t size) throw();

	virtual void __RGBToGREYSCALE(const uint8_t *data) throw();
	virtual void __RGBToRGBA(const uint8_t *data) throw();
	virtual void __RGBToYUV422(const uint8_t *data, const size_t size) throw();
	virtual void __RGBToYUV422P(const uint8_t *data, const size_t size) throw();

	virtual void __RGBAToGREYSCALE(const uint8_t *data) throw();
	virtual void __RGBAToRGB(const uint8_t *data) throw();
	virtual void __RGBAToYUV422(const uint8_t *data, const size_t size) throw();
	virtual void __RGBAToYUV422P(const uint8_t *data, const size_t size) throw();

	virtual void __YUV422ToGREYSCALE(const uint8_t *data) throw();
	virtual void __YUV422ToRGB(const uint8_t *data, const size_t size) throw();
	virtual void __YUV422ToRGBA(const uint8_t *data, const size_t size) throw();
	virtual void __YUV422ToYUV422P(const uint8_t *data, const size_t size) throw();

	virtual void __YUV422PToGREYSCALE(const uint8_t *data) throw();
	virtual void __YUV422PToRGB(const uint8_t *data) throw();
	virtual void __YUV422PToRGBA(const uint8_t *data) throw();
	virtual void __YUV422PToYUV422(const uint8_t *data) throw();

public:
	ashe::ManagedBinary<uint8_t> deck;

	ColourSpace(const ColourFormat from, const ColourFormat to, const uint32_t width, const uint32_t height)
		throw(Exception);
	ColourSpace(const thisClass &src) throw();
	virtual ~ColourSpace() throw();

	thisClass &operator =(const thisClass &src) throw();

	virtual thisClass &hand(uint8_t *data, const size_t size) throw(Exception);
	virtual thisClass &flush() throw();

	virtual ColourFormat getFromFormat() const throw();
	virtual ColourFormat getToFormat() const throw();
	virtual uint32_t getWidth() const throw();
	virtual uint32_t getHeight() const throw();
	virtual size_t getFromSize() const throw();
	virtual size_t getToSize() const throw();
	virtual size_t countPixels() const throw();
	virtual size_t getSizeOf(const ColourFormat format) const throw();
	virtual unsigned long long getManoeuvreCount() const throw();

	static uint8_t clamp__(const int x) throw();
	static void YUV444toRGB888__(const uint8_t y, const uint8_t u, const uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) throw();
	static void RGB888toYUV444__(const uint8_t r, const uint8_t g, const uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v) throw();
};

} /* namespace 'ashe' */

#endif
