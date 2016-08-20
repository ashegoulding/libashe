#include "libashe/Endian.h"
#include "__internal.h"
#include <cmath>

namespace ashe
{

static void __dropif_odd__(const size_t x) LASHE_NOEXCEPT
{
	switch(x)
	{
	case 0:
		__die_critical();
		break;
	case 2: // short
	case 4: // int or float
	case 8: // long or double
	case 16: // quad
		break;
	default:
	{
		/* To understand the basics behind this,
		* draw the graph of: y = floor(log(x) / log(2))
		* This formula checks if 'x' is the first value in its horizontal line.
		* (Where floor(a) == floor(b).)
		*
		* No idea on handling overflow or underflow happens here,
		* which some machine would make.
		*/
		static const double __LOG_TWO__ = std::log(2.0F);

		const double a = std::log((double)(x - 1)) / __LOG_TWO__;
		const double b = std::log((double)x) / __LOG_TWO__;

		if(a == NAN || b == NAN || (long long)a == (long long)b)
			__die_critical();
	}
	}
}

#define __INTP_BUF(x) ((uint8_t*)x)

void invertEndian_lb__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	size_t a, b;

	__dropif_odd__(len);
	a = 0;
	b = len - 1;
	while(a < b)
	{
		std::swap(__INTP_BUF(buf)[a], __INTP_BUF(buf)[b]);
		++a;
		--b;
	}
}

void invertEndian_ml__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	size_t a, b;

	__dropif_odd__(len);
	a = 0;
	b = len / 2;
	while(b < len)
	{
		std::swap(__INTP_BUF(buf)[a], __INTP_BUF(buf)[b]);
		++a;
		++b;
	}
}

void invertEndian_mb__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	size_t a, b;

	__dropif_odd__(len);
	a = 0;
	b = len / 2 - 1;
	while(a < b)
	{
		std::swap(__INTP_BUF(buf)[a], __INTP_BUF(buf)[b]);
		++a;
		--b;
	}
	a = len / 2;
	b = len - 1;
	while(a < b)
	{
		std::swap(__INTP_BUF(buf)[a], __INTP_BUF(buf)[b]);
		++a;
		--b;
	}
}

void fromEndian_l__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_BIG()
	invertEndian_lb__(buf, len);
#elif LASHE_IS_ENDIAN_PDP()
	invertEndian_ml__(buf, len);
#endif
}

void fromEndian_b__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_LITTLE()
	invertEndian_lb__(buf, len);
#elif LASHE_IS_ENDIAN_PDP()
	invertEndian_mb__(buf, len);
#endif
}

void fromEndian_m__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_LITTLE()
	invertEndian_ml__(buf, len);
#elif LASHE_IS_ENDIAN_BIG()
	invertEndian_mb__(buf, len);
#endif
}

void toEndian_l__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_BIG()
	invertEndian_lb__(buf, len);
#elif LASHE_IS_ENDIAN_PDP()
	invertEndian_ml__(buf, len);
#endif
}

void toEndian_b__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_LITTLE()
	invertEndian_lb__(buf, len);
#elif LASHE_IS_ENDIAN_PDP()
	invertEndian_mb__(buf, len);
#endif
}

void toEndian_m__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	__dropif_odd__(len);
#if LASHE_IS_ENDIAN_LITTLE()
	invertEndian_ml__(buf, len);
#elif LASHE_IS_ENDIAN_BIG()
	invertEndian_mb__(buf, len);
#endif
}

}
