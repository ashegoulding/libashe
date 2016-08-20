/* Endian testing macros and functions.
* @COMPILER
* Ex:
*	#if LASHE_IS_ENDIAN_LITTLE()
*	// TODO: Something for little endian host.
*	#elif LASHE_IS_ENDIAN_BIG()
*	// TODO: Something for big endian host.
*	#else
*	// TODO: Something for pdp endian host.
*	#endif
*/
#ifndef LASHE_ENDIAN_H_
#define LASHE_ENDIAN_H_

#include "pub.h"


#ifdef _MSC_BUILD
#define LASHE_IS_ENDIAN_LITTLE() (0x41424344UL == 'ABCD')
#define LASHE_IS_ENDIAN_BIG() (0x44434241UL == 'ABCD')
#define LASHE_IS_ENDIAN_PDP() (0x42414443UL == 'ABCD')
#elif __BYTE_ORDER__
#define LASHE_IS_ENDIAN_LITTLE() (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define LASHE_IS_ENDIAN_BIG() (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define LASHE_IS_ENDIAN_PDP() (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#else
#error "No way to determine host endianness."
#endif

// Endian check.
#if !(LASHE_IS_ENDIAN_LITTLE() || LASHE_IS_ENDIAN_BIG() || LASHE_IS_ENDIAN_PDP())
#error "Unknown endian host."
#endif

namespace ashe
{

// Little <-> Big
void LASHE_DECL_EXT_NOEXCEPT invertEndian_lb__(void *buf, const size_t len) LASHE_NOEXCEPT;
// PDP <-> Little
void LASHE_DECL_EXT_NOEXCEPT invertEndian_ml__(void *buf, const size_t len) LASHE_NOEXCEPT;
// PDP <-> Big
void LASHE_DECL_EXT_NOEXCEPT invertEndian_mb__(void *buf, const size_t len) LASHE_NOEXCEPT;

// Assures 'buf' to be in host endian.
void LASHE_DECL_EXT_NOEXCEPT fromEndian_l__(void *buf, const size_t len) LASHE_NOEXCEPT;
void LASHE_DECL_EXT_NOEXCEPT fromEndian_b__(void *buf, const size_t len) LASHE_NOEXCEPT;
void LASHE_DECL_EXT_NOEXCEPT fromEndian_m__(void *buf, const size_t len) LASHE_NOEXCEPT;
// Converts 'buf' to the endian specified.
void LASHE_DECL_EXT_NOEXCEPT toEndian_l__(void *buf, const size_t len) LASHE_NOEXCEPT;
void LASHE_DECL_EXT_NOEXCEPT toEndian_b__(void *buf, const size_t len) LASHE_NOEXCEPT;
void LASHE_DECL_EXT_NOEXCEPT toEndian_m__(void *buf, const size_t len) LASHE_NOEXCEPT;

#define LASHE_INVERTENDIAN_LB(x) ashe::invertEndian_lb(&x, sizeof(x))
#define LASHE_INVERTENDIAN_ML(x) ashe::invertEndian_ml(&x, sizeof(x))
#define LASHE_INVERTENDIAN_MB(x) ashe::invertEndian_mb(&x, sizeof(x))

#define LASHE_FROMENDIAN_L(x) ashe::fromEndian_l(&x, sizeof(x))
#define LASHE_FROMENDIAN_B(x) ashe::fromEndian_b(&x, sizeof(x))
#define LASHE_FROMENDIAN_M(x) ashe::fromEndian_m(&x, sizeof(x))
#define LASHE_TOENDIAN_L(x) ashe::toEndian_l(&x, sizeof(x))
#define LASHE_TOENDIAN_B(x) ashe::toEndian_b(&x, sizeof(x))
#define LASHE_TOENDIAN_M(x) ashe::toEndian_m(&x, sizeof(x))

}

#endif
