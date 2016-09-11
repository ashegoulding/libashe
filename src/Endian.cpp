#include "libashe/Endian.h"
#include "__internal.h"
#include <cmath>

namespace ashe
{

static void __dropif_odd__(const size_t x) LASHE_NOEXCEPT
{
	if(0 == x)
		__die_critical();
	else
	{
		/* To understand the basics behind this,
		 * draw the graph of: y = floor(log(x) / log(2))
		 * This formula checks if 'x' is the first value in its horizontal line.
		 * (Where floor(a) == floor(b).)
		 *
		 * No idea on handling overflow or underflow happens here,
		 * which some machine would make.
		 */
		const double a = std::log2(x - 1);
		const double b = std::log2(x);

		if((long long)a == (long long)b)
			__die_critical();
	}
}

#define __INTP_BUF(x) ((uint8_t*)x)

void invertEndian_lb__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	switch(len)
	{
	case 2: // short
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[1]);
		break;
	case 4: // int
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[3]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[2]);
		break;
	case 8: // long
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[7]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[6]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[5]);
		std::swap(__INTP_BUF(buf)[3], __INTP_BUF(buf)[4]);
		break;
	case 16: // GUID
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[15]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[14]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[13]);
		std::swap(__INTP_BUF(buf)[3], __INTP_BUF(buf)[12]);
		std::swap(__INTP_BUF(buf)[4], __INTP_BUF(buf)[11]);
		std::swap(__INTP_BUF(buf)[5], __INTP_BUF(buf)[10]);
		std::swap(__INTP_BUF(buf)[6], __INTP_BUF(buf)[9]);
		std::swap(__INTP_BUF(buf)[7], __INTP_BUF(buf)[8]);
		break;
	default:
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
	}
}

void invertEndian_ml__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	switch(len)
	{
	case 2: // short
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[1]);
		break;
	case 4: // int
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[2]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[3]);
		break;
	case 8: // long
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[4]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[5]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[6]);
		std::swap(__INTP_BUF(buf)[3], __INTP_BUF(buf)[7]);
		break;
	case 16: // GUID
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[8]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[9]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[10]);
		std::swap(__INTP_BUF(buf)[3], __INTP_BUF(buf)[11]);
		std::swap(__INTP_BUF(buf)[4], __INTP_BUF(buf)[12]);
		std::swap(__INTP_BUF(buf)[5], __INTP_BUF(buf)[13]);
		std::swap(__INTP_BUF(buf)[6], __INTP_BUF(buf)[14]);
		std::swap(__INTP_BUF(buf)[7], __INTP_BUF(buf)[15]);
		break;
	default:
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
	}
}

void invertEndian_mb__(void* buf, const size_t len) LASHE_NOEXCEPT
{
	switch(len)
	{
	case 2: // short
		break;
	case 4: // int
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[1]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[3]);
		break;
	case 8: // long
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[3]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[2]);
		std::swap(__INTP_BUF(buf)[4], __INTP_BUF(buf)[7]);
		std::swap(__INTP_BUF(buf)[5], __INTP_BUF(buf)[6]);
		break;
	case 16: // GUID
		std::swap(__INTP_BUF(buf)[0], __INTP_BUF(buf)[7]);
		std::swap(__INTP_BUF(buf)[1], __INTP_BUF(buf)[6]);
		std::swap(__INTP_BUF(buf)[2], __INTP_BUF(buf)[5]);
		std::swap(__INTP_BUF(buf)[3], __INTP_BUF(buf)[4]);
		std::swap(__INTP_BUF(buf)[8], __INTP_BUF(buf)[15]);
		std::swap(__INTP_BUF(buf)[9], __INTP_BUF(buf)[14]);
		std::swap(__INTP_BUF(buf)[10], __INTP_BUF(buf)[13]);
		std::swap(__INTP_BUF(buf)[11], __INTP_BUF(buf)[12]);
		break;
	default:
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
