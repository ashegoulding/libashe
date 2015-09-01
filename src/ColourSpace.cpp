#include "libashe/ColourSpace.h"

namespace ashe
{

ColourSpace::ColourSpace(ColourFormat from, ColourFormat to, const uint32_t width, const uint32_t height)
	throw(Exception)
	: from(from)
	, to(to)
	, width(width)
	, height(height)
	, manoeuvreCount(0)
{
	if (from == to)
	{
		Exception e(Exception::C_NOT_NECESSARY, "Convert what? (from == to)");
		throw e;
	}
	switch (from)
	{
	case CF_GREYSCALE:
	case CF_RGB:
	case CF_RGBA:
	case CF_YUV422:
	case CF_YUV422P:
		break;
	default:
	{
		std::stringstream sb;
		sb << "Unknown value 'from': " << from;
		Exception e(Exception::C_ILLEGAL_ARGUMENT, sb.str());
		throw e;
	}
	}
	switch (to)
	{
	case CF_GREYSCALE:
	case CF_RGB:
	case CF_RGBA:
	case CF_YUV422:
	case CF_YUV422P:
		break;
	default:
	{
		std::stringstream sb;
		sb << "Unknown value 'to': " << to;
		Exception e(Exception::C_ILLEGAL_ARGUMENT, sb.str());
		throw e;
	}
	}
	this->fromSize = this->getFromSize();
	this->deck.setSize(this->getToSize());
}

ColourSpace::ColourSpace(const thisClass &src) throw()
		: from(CF_NONE)
		, to(CF_NONE)
		, fromSize(0)
		, width(0)
		, height(0)
		, manoeuvreCount(0)
{
	this->__construct(src);
}

ColourSpace::~ColourSpace() throw()
{
}

ColourSpace::thisClass & ColourSpace::hand(uint8_t *data, const size_t size) throw(Exception)
{
	if (!data)
	{
		Exception e(Exception::C_NULL_POINTER, "NULL pointer: 'data'");
		throw e;
	}
	else if (size < this->fromSize)
	{
		std::stringstream sb;
		sb << "Short length: 'size' (required over: " << this->getToSize() << ')';
		Exception e(Exception::C_SHORT_LENGTH, sb.str());
		throw e;
	}

	switch (this->from)
	{
	case CF_GREYSCALE:
		switch (this->to)
		{
		case CF_RGB: this->__GREYSCALEToRGB(data, this->fromSize); break;
		case CF_RGBA: this->__GREYSCALEToRGBA(data, this->fromSize); break;
		case CF_YUV422: this->__GREYSCALEToYUV422(data, this->fromSize); break;
		case CF_YUV422P: this->__GREYSCALEToYUV422P(data, this->fromSize); break;
		}
		break;
	case CF_RGB:
		switch (this->to)
		{
		case CF_GREYSCALE: this->__RGBToGREYSCALE(data); break;
		case CF_RGBA: this->__RGBToRGBA(data); break;
		case CF_YUV422: this->__RGBToYUV422(data, this->fromSize); break;
		case CF_YUV422P: this->__RGBToYUV422P(data, this->fromSize); break;
		}
		break;
	case CF_RGBA:
		switch (this->to)
		{
		case CF_RGB: this->__RGBAToRGB(data); break;
		case CF_GREYSCALE: this->__RGBAToGREYSCALE(data); break;
		case CF_YUV422: this->__RGBAToYUV422(data, this->fromSize); break;
		case CF_YUV422P: this->__RGBAToYUV422P(data, this->fromSize); break;
		}
		break;
	case CF_YUV422:
		switch (this->to)
		{
		case CF_RGB: this->__YUV422ToRGB(data, this->fromSize); break;
		case CF_RGBA: this->__YUV422ToRGBA(data, this->fromSize); break;
		case CF_GREYSCALE: this->__YUV422ToGREYSCALE(data); break;
		case CF_YUV422P: this->__YUV422ToYUV422P(data, this->fromSize); break;
		}
		break;
	case CF_YUV422P:
		switch (this->to)
		{
		case CF_RGB: this->__YUV422PToRGB(data); break;
		case CF_RGBA: this->__YUV422PToRGBA(data); break;
		case CF_YUV422: this->__YUV422PToYUV422(data); break;
		case CF_GREYSCALE: this->__YUV422PToGREYSCALE(data); break;
		}
		break;
	}

	++this->manoeuvreCount;
	return *this;
}

ColourSpace::ColourFormat ColourSpace::getFromFormat() const throw()
{
	return this->from;
}

ColourSpace::ColourFormat ColourSpace::getToFormat() const throw()
{
	return this->to;
}

uint32_t ColourSpace::getWidth() const throw()
{
	return this->width;
}

uint32_t ColourSpace::getHeight() const throw()
{
	return this->height;
}

size_t ColourSpace::getFromSize() const throw()
{
	return this->getSizeOf(this->from);
}

size_t ColourSpace::getToSize() const throw()
{
	return this->getSizeOf(this->to);
}

size_t ColourSpace::countPixels() const throw()
{
	return this->width * this->height;
}

size_t ColourSpace::getSizeOf(const ColourFormat format) const throw()
{
	switch (format)
	{
	case CF_GREYSCALE: return this->countPixels();
	case CF_RGB: return this->countPixels() * 3;
	case CF_RGBA: return this->countPixels() * 4;
	case CF_YUV422: return this->countPixels() * 2;
	case CF_YUV422P: return this->countPixels() * 2;
	}
	return 0;
}

uint8_t ColourSpace::clamp__(const int x) throw()
{
	if (x > 255)
		return 255;
	else if (x < 0)
		return 0;
	return (uint8_t)x;
}

void ColourSpace::YUV444toRGB888__(const uint8_t y, const uint8_t u, const uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) throw()
{
	const int
		c = y - 16,
		d = u - 128,
		e = v - 128;

	*r = clamp__((298 * c + 409 * e + 128) >> 8);
	*g = clamp__((298 * c - 100 * d - 208 * e + 128) >> 8);
	*b = clamp__((298 * c + 516 * d + 128) >> 8);
}

ColourSpace::thisClass & ColourSpace::flush() throw()
{
	this->deck.fillZeros();
	this->manoeuvreCount = 0;
	return *this;
}

unsigned long long ColourSpace::getManoeuvreCount() const throw()
{
	return this->manoeuvreCount;
}

void ColourSpace::RGB888toYUV444__(const uint8_t r, const uint8_t g, const uint8_t b, uint8_t *y, uint8_t *u, uint8_t *v) throw()
{
	*y = clamp__((299 * r + 587 * g + 114 * b) / 1000);
	*u = clamp__(128 - (14 * r + 29 * g + 43 * b) / 100);
	*v = clamp__(128 + (36 * r - 29 * g - 7 * b) / 100);
}

ColourSpace::thisClass & ColourSpace::operator=(const thisClass &src) throw()
{
	this->__construct(src);
	return *this;
}

void ColourSpace::__construct(const thisClass &src) throw()
{
	this->width = src.width;
	this->height = src.height;
	this->manoeuvreCount = src.manoeuvreCount;
	this->fromSize = src.fromSize;
	this->from = src.from;
	this->to = src.to;
	this->deck = src.deck;
}

void ColourSpace::__GREYSCALEToRGB(const uint8_t *data, const size_t size) throw()
{
	uint8_t *c = this->deck.data;
	const uint8_t *end = data + size;

	while (data < end)
	{
		*(c++) = *data;
		*(c++) = *data;
		*(c++) = *data;
		++data;
	}
}

void ColourSpace::__GREYSCALEToRGBA(const uint8_t *data, const size_t size) throw()
{
	uint8_t *c = this->deck.data;
	const uint8_t *end = data + size;

	while (data < end)
	{
		*(c++) = *data;
		*(c++) = *data;
		*(c++) = *data;
		*(c++) = 0;
		++data;
	}
}

void ColourSpace::__GREYSCALEToYUV422(const uint8_t *data, const size_t size) throw()
{
	uint8_t
		*y0 = this->deck.data,
		*u = this->deck.data + 1,
		*y1 = this->deck.data + 2,
		*v = this->deck.data + 3;
	const uint8_t *end = data + size;
	uint8_t u0, u1, v0, v1;

	while (data < end)
	{
		thisClass::RGB888toYUV444__(*data, *data, *data, y0, &u0, &v0);
		++data;
		thisClass::RGB888toYUV444__(*data, *data, *data, y1, &u1, &v1);
		++data;
		*u = (uint8_t)((u0 + u1) / 2);
		*v = (uint8_t)((v0 + v1) / 2);

		y0 += 4;
		u += 4;
		y1 += 4;
		v += 4;
	}
}

void ColourSpace::__GREYSCALEToYUV422P(const uint8_t *data, const size_t size) throw()
{
	const size_t CNT = this->countPixels();
	uint8_t
		*y = this->deck.data,
		*u = this->deck.data + CNT,
		*v = this->deck.data + CNT + CNT / 2;
	const uint8_t *end = data + size;
	uint8_t u0, u1, v0, v1;

	while (data < end)
	{
		thisClass::RGB888toYUV444__(*data, *data, *data, y++, &u0, &v0);
		++data;
		thisClass::RGB888toYUV444__(*data, *data, *data, y++, &u1, &v1);
		++data;
		*(u++) = (uint8_t)((u0 + u1) / 2);
		*(v++) = (uint8_t)((v0 + v1) / 2);
	}
}

void ColourSpace::__RGBToGREYSCALE(const uint8_t *data) throw()
{
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		*(p++) = (uint8_t)((data[0] + data[1] + data[2]) / 3);
		data += 3;
	}
}

void ColourSpace::__RGBToRGBA(const uint8_t *data) throw()
{
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		*(p++) = *(data++);
		*(p++) = *(data++);
		*(p++) = *(data++);
		*(p++) = 0;
	}
}

void ColourSpace::__RGBToYUV422(const uint8_t *data, const size_t size) throw()
{
	uint8_t r1, g1, b1, r2, g2, b2;
	const uint8_t *org = data;
	uint8_t
		*y0p = this->deck.data,
		*up = this->deck.data + 1,
		*y1p = this->deck.data + 2,
		*vp = this->deck.data + 3;
	const uint8_t *end = org + size;

	while (org < end)
	{
		r1 = *(org++);
		g1 = *(org++);
		b1 = *(org++);
		r2 = *(org++);
		g2 = *(org++);
		b2 = *(org++);

		*y0p = thisClass::clamp__((299 * r1 + 587 * g1 + 114 * b1) / 1000);
		*up = thisClass::clamp__(128 - ((14 * r1 + 29 * g1 + 43 * b1) + (14 * r2 + 29 * g2 + 43 * b2)) / 200);
		*y1p = thisClass::clamp__((299 * r2 + 587 * g2 + 114 * b2) / 1000);
		*vp = thisClass::clamp__(128 + ((36 * r1 - 29 * g1 - 7 * b1) + (36 * r2 - 29 * g2 - 7 * b2)) / 200);

		y0p += 4;
		up += 4;
		y1p += 4;
		vp += 4;
	}
}

void ColourSpace::__RGBToYUV422P(const uint8_t *data, const size_t size) throw()
{
	uint8_t r1, g1, b1, r2, g2, b2;
	const size_t CNT = this->countPixels();
	const uint8_t *org = data;
	uint8_t
		*yp = this->deck.data,
		*up = this->deck.data + CNT,
		*vp = this->deck.data + CNT + CNT / 2;
	const uint8_t *end = org + size;

	while (org < end)
	{
		r1 = *(org++);
		g1 = *(org++);
		b1 = *(org++);
		r2 = *(org++);
		g2 = *(org++);
		b2 = *(org++);

		*(yp++) = thisClass::clamp__((299 * r1 + 587 * g1 + 114 * b1) / 1000);
		*(yp++) = thisClass::clamp__((299 * r2 + 587 * g2 + 114 * b2) / 1000);
		*(up++) = thisClass::clamp__(128 - ((14 * r1 + 29 * g1 - 43 * b1) + (14 * r2 + 29 * g2 - 43 * b2)) / 200);
		*(vp++) = thisClass::clamp__(128 + ((36 * r1 - 29 * g1 - 7 * b1) + (36 * r2 - 29 * g2 - 7 * b2)) / 200);
	}
}

void ColourSpace::__RGBAToGREYSCALE(const uint8_t *data) throw()
{
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		*(p++) = (uint8_t)((data[0] + data[1] + data[2]) / 3);
		data += 3;
	}
}

void ColourSpace::__RGBAToRGB(const uint8_t *data) throw()
{
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		*(p++) = *(data++);
		*(p++) = *(data++);
		*(p++) = *(data++);
		++data;
	}
}

void ColourSpace::__YUV422ToGREYSCALE(const uint8_t *data) throw()
{
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		*(p++) = *data;
		data += 2;
	}
}

void ColourSpace::__YUV422ToRGB(const uint8_t *data, const size_t size) throw()
{
	uint8_t y0, y1, u, v, r, g, b;
	const uint8_t *org = data;
	uint8_t *p = this->deck.data;
	const uint8_t *end = org + size;

	while (org < end)
	{
		y0 = *(org++);
		u = *(org++);
		y1 = *(org++);
		v = *(org++);
		thisClass::YUV444toRGB888__(y0, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		thisClass::YUV444toRGB888__(y1, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
	}
}

void ColourSpace::__YUV422PToGREYSCALE(const uint8_t *data) throw()
{
	memcpy(this->deck.data, data, this->deck.size);
}

void ColourSpace::__YUV422PToRGB(const uint8_t *data) throw()
{
	uint8_t y0, y1, u, v, r, g, b;
	const size_t CNT = this->countPixels();
	const uint8_t
		*yp = data,
		*up = data + CNT,
		*vp = data + CNT + CNT / 2;
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		y0 = *(yp++);
		u = *(up++);
		y1 = *(yp++);
		v = *(vp++);
		thisClass::YUV444toRGB888__(y0, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		thisClass::YUV444toRGB888__(y1, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
	}
}

void ColourSpace::__RGBAToYUV422(const uint8_t *data, const size_t size) throw()
{
	uint8_t r1, g1, b1, r2, g2, b2;
	const uint8_t *org = data;
	uint8_t
		*y0p = this->deck.data,
		*up = this->deck.data + 1,
		*y1p = this->deck.data + 2,
		*vp = this->deck.data + 3;
	const uint8_t *end = org + size;

	while (org < end)
	{
		r1 = *(org++);
		g1 = *(org++);
		b1 = *(org++);
		++org;
		r2 = *(org++);
		g2 = *(org++);
		b2 = *(org++);
		++org;

		*y0p = thisClass::clamp__((299 * r1 + 587 * g1 + 114 * b1) / 1000);
		*up = thisClass::clamp__(128 - ((14 * r1 + 29 * g1 + 43 * b1) + (14 * r2 + 29 * g2 + 43 * b2)) / 200);
		*y1p = thisClass::clamp__((299 * r2 + 587 * g2 + 114 * b2) / 1000);
		*vp = thisClass::clamp__(128 + ((36 * r1 - 29 * g1 - 7 * b1) + (36 * r2 - 29 * g2 - 7 * b2)) / 200);

		y0p += 4;
		up += 4;
		y1p += 4;
		vp += 4;
	}
}

void ColourSpace::__RGBAToYUV422P(const uint8_t *data, const size_t size) throw()
{
	uint8_t r1, g1, b1, r2, g2, b2;
	const size_t CNT = this->countPixels();
	const uint8_t *org = data;
	uint8_t
		*yp = this->deck.data,
		*up = this->deck.data + CNT,
		*vp = this->deck.data + CNT + CNT / 2;
	const uint8_t *end = org + size;

	while (org < end)
	{
		r1 = *(org++);
		g1 = *(org++);
		b1 = *(org++);
		++org;
		r2 = *(org++);
		g2 = *(org++);
		b2 = *(org++);
		++org;

		*(yp++) = thisClass::clamp__((299 * r1 + 587 * g1 + 114 * b1) / 1000);
		*(yp++) = thisClass::clamp__((299 * r2 + 587 * g2 + 114 * b2) / 1000);
		*(up++) = thisClass::clamp__(128 - ((14 * r1 + 29 * g1 - 43 * b1) + (14 * r2 + 29 * g2 - 43 * b2)) / 200);
		*(vp++) = thisClass::clamp__(128 + ((36 * r1 - 29 * g1 - 7 * b1) + (36 * r2 - 29 * g2 - 7 * b2)) / 200);
	}
}

void ColourSpace::__YUV422ToRGBA(const uint8_t *data, const size_t size) throw()
{
	uint8_t y0, y1, u, v, r, g, b;
	const uint8_t *org = data;
	uint8_t *p = this->deck.data;
	const uint8_t *end = org + size;

	while (org < end)
	{
		y0 = *(org++);
		u = *(org++);
		y1 = *(org++);
		v = *(org++);
		thisClass::YUV444toRGB888__(y0, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		*(p++) = 0;
		thisClass::YUV444toRGB888__(y1, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		*(p++) = 0;
	}
}

void ColourSpace::__YUV422ToYUV422P(const uint8_t *data, const size_t size) throw()
{
	const size_t CNT = this->countPixels();
	const uint8_t
		*y0 = data,
		*u = data + 1,
		*y1 = data + 2,
		*v = data + 3;
	uint8_t
		*outY = this->deck.data,
		*outU = this->deck.data + CNT,
		*outV = this->deck.data + CNT + CNT / 2;
	const uint8_t *end = data + size;

	while (y0 < end)
	{
		*(outY++) = *y0;
		*(outY++) = *y1;
		*(outU++) = *u;
		*(outV++) = *v;

		y0 += 4;
		y1 += 4;
		u += 4;
		v += 4;
	}
}

void ColourSpace::__YUV422PToRGBA(const uint8_t *data) throw()
{
	uint8_t y0, y1, u, v, r, g, b;
	const size_t CNT = this->countPixels();
	const uint8_t
		*yp = data,
		*up = data + CNT,
		*vp = data + CNT + CNT / 2;
	uint8_t *p = this->deck.data;
	const uint8_t *end = p + this->deck.size;

	while (p < end)
	{
		y0 = *(yp++);
		u = *(up++);
		y1 = *(yp++);
		v = *(vp++);
		thisClass::YUV444toRGB888__(y0, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		*(p++) = 0;
		thisClass::YUV444toRGB888__(y1, u, v, &r, &g, &b);
		*(p++) = r;
		*(p++) = g;
		*(p++) = b;
		*(p++) = 0;
	}
}

void ColourSpace::__YUV422PToYUV422(const uint8_t *data) throw()
{
	const size_t CNT = this->countPixels();
	const uint8_t
		*y = data,
		*u = data + CNT,
		*v = data + CNT + CNT / 2;
	uint8_t
		*outY0 = this->deck.data,
		*outU = this->deck.data + 1,
		*outY1 = this->deck.data + 2,
		*outV = this->deck.data + 3;
	const uint8_t *end = outY0 + this->deck.size;

	while (outY0 < end)
	{
		*outY0 = *(y++);
		*outY1 = *(y++);
		*outU = *(u++);
		*outV = *(v++);

		outY0 += 4;
		outY1 += 4;
		outU += 4;
		outV += 4;
	}
}

/* ColourSpace::Exception
*/
ColourSpace::Exception::Exception(const Code code /*= CODE_UNDEFINED*/, const std::string msg /*= ""*/) throw()
	: code(code)
{
	std::stringstream sb;
	sb << '[' << thisClass::codeToString__(code) << "] " << msg;
	this->whatString = sb.str();
}

ColourSpace::Exception::Exception(const thisClass &src) throw()
		: code(C_UNDEFINED)
{
	this->__construct(src);
}

ColourSpace::Exception::~Exception() throw()
{
}

ColourSpace::Exception::thisClass & ColourSpace::Exception::operator=(const thisClass &src) throw()
{
	this->__construct(src);
	return *this;
}

void ColourSpace::Exception::__construct(const thisClass &src) throw()
{
	this->code = src.code;
	this->whatString = src.whatString;
}

const char * ColourSpace::Exception::what() const throw()
{
	return this->whatString.c_str();
}

ColourSpace::Exception::Code ColourSpace::Exception::getCode() const throw()
{
	return this->code;
}

std::string ColourSpace::Exception::toString() const throw()
{
	return this->whatString;
}

std::string ColourSpace::Exception::codeToString__(const Code code) throw()
{
	switch (code)
	{
	case C_NOT_NECESSARY: return "NOT_NECESSARY";
	case C_NULL_POINTER: return "NULL_POINTER";
	case C_SHORT_LENGTH: return "SHORT_LENGTH";
	case C_ILLEGAL_ARGUMENT: return "ILLEGAL_ARGUMENT";
	}
	return "UNDEFINED";
}

} /* namespace 'ashe' */
