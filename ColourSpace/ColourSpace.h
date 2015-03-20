#ifndef ASHE_COLOURSPACE_H_
#define ASHE_COLOURSPACE_H_

#include <string>
#include <sstream>

#include <exception>

#include "ManagedBinary.h"

#ifdef _MSC_BUILD

#include "stdafx.h"
#pragma warning(disable:4819) // �ҽ� ���ڵ� (cp949���� utf-8 ���ڿ�)
#pragma warning(disable:4290) // throw() or noexpt
#define _CRT_SECURE_NO_WARNINGS // c std lib

#else

#define AFX_EXT_API

#endif

namespace ashe
{

class AFX_EXT_API ColourSpace
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

	class AFX_EXT_API Exception : public std::exception
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
	unsigned int width, height;
	unsigned long long manoeuvreCount;
	size_t fromSize;
	ColourFormat from, to;

	virtual void __construct(const thisClass &src) throw();

	virtual void __GREYSCALEToRGB(const unsigned char *data, const size_t size) throw();
	virtual void __GREYSCALEToRGBA(const unsigned char *data, const size_t size) throw();
	virtual void __GREYSCALEToYUV422(const unsigned char *data, const size_t size) throw();
	virtual void __GREYSCALEToYUV422P(const unsigned char *data, const size_t size) throw();

	virtual void __RGBToGREYSCALE(const unsigned char *data) throw();
	virtual void __RGBToRGBA(const unsigned char *data) throw();
	virtual void __RGBToYUV422(const unsigned char *data, const size_t size) throw();
	virtual void __RGBToYUV422P(const unsigned char *data, const size_t size) throw();

	virtual void __RGBAToGREYSCALE(const unsigned char *data) throw();
	virtual void __RGBAToRGB(const unsigned char *data) throw();
	virtual void __RGBAToYUV422(const unsigned char *data, const size_t size) throw();
	virtual void __RGBAToYUV422P(const unsigned char *data, const size_t size) throw();

	virtual void __YUV422ToGREYSCALE(const unsigned char *data) throw();
	virtual void __YUV422ToRGB(const unsigned char *data, const size_t size) throw();
	virtual void __YUV422ToRGBA(const unsigned char *data, const size_t size) throw();
	virtual void __YUV422ToYUV422P(const unsigned char *data, const size_t size) throw();

	virtual void __YUV422PToGREYSCALE(const unsigned char *data) throw();
	virtual void __YUV422PToRGB(const unsigned char *data) throw();
	virtual void __YUV422PToRGBA(const unsigned char *data) throw();
	virtual void __YUV422PToYUV422(const unsigned char *data) throw();

public:
	ashe::ManagedBinary<unsigned char> deck;

	ColourSpace(const ColourFormat from, const ColourFormat to, const unsigned int width, const unsigned int height)
		throw(Exception);
	ColourSpace(const thisClass &src) throw();
	virtual ~ColourSpace() throw();

	thisClass &operator =(const thisClass &src) throw();

	virtual thisClass &hand(unsigned char *data, const size_t size) throw(Exception);
	virtual thisClass &flush() throw();

	virtual ColourFormat getFromFormat() const throw();
	virtual ColourFormat getToFormat() const throw();
	virtual unsigned int getWidth() const throw();
	virtual unsigned int getHeight() const throw();
	virtual size_t getFromSize() const throw();
	virtual size_t getToSize() const throw();
	virtual size_t countPixels() const throw();
	virtual size_t getSizeOf(const ColourFormat format) const throw();
	virtual unsigned long long getManoeuvreCount() const throw();

	static unsigned char clamp__(const int x) throw();
	static void YUV444toRGB888__(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char *r, unsigned char *g, unsigned char *b) throw();
	static void RGB888toYUV444__(const unsigned char r, const unsigned char g, const unsigned char b, unsigned char *y, unsigned char *u, unsigned char *v) throw();
};

} /* namespace 'ashe' */

#endif
