/* Endian testing macros and functions.
 * @EXAMPLE
 *	#if LASHE_IS_ENDIAN_LITTLE()
 *	// Do Something for little endian host.
 *	#elif LASHE_IS_ENDIAN_BIG()
 *	// Do Something for big endian host.
 *	#else
 *	// Do Something for pdp endian host.
 *	#endif
 */
#ifndef LASHE_ENDIAN_HPP_
#define LASHE_ENDIAN_HPP_
#include "Exception.hpp"
#include "pub.hpp"

#include <utility>

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
#if !(LASHE_IS_ENDIAN_LITTLE() || LASHE_IS_ENDIAN_BIG() ||                     \
      LASHE_IS_ENDIAN_PDP())
#error "Unknown endian host."
#endif


namespace ashe {

LASHE_DECL_EXCEPTION(EndianException, Exception);

// Little <-> Big
void LASHE_DECL_EXT invert_endian_lb(uint8_t *buf, const size_t size)
{
    switch (size) {
    case 2: // short
        std::swap(buf[0], buf[1]);
        break;
    case 4: // int
        std::swap(buf[0], buf[3]);
        std::swap(buf[1], buf[2]);
        break;
    case 8: // long
        std::swap(buf[0], buf[7]);
        std::swap(buf[1], buf[6]);
        std::swap(buf[2], buf[5]);
        std::swap(buf[3], buf[4]);
        break;
    case 16: // GUID
        std::swap(buf[0], buf[15]);
        std::swap(buf[1], buf[14]);
        std::swap(buf[2], buf[13]);
        std::swap(buf[3], buf[12]);
        std::swap(buf[4], buf[11]);
        std::swap(buf[5], buf[10]);
        std::swap(buf[6], buf[9]);
        std::swap(buf[7], buf[8]);
        break;
    default:
        if (size % 2 == 0) {
            size_t a, b;

            a = 0;
            b = size - 1;
            while (a < b) {
                std::swap(buf[a], buf[b]);
                a += 1;
                b -= 1;
            }
        }
        else {
            throw EndianException("non multiple of 2 size.");
        }
    }
}

// PDP <-> Little
void LASHE_DECL_EXT invert_endian_ml(uint8_t *buf, const size_t size)
{
    switch (size) {
    case 2: // short
        std::swap(buf[0], buf[1]);
        break;
    case 4: // int
        std::swap(buf[0], buf[2]);
        std::swap(buf[1], buf[3]);
        break;
    case 8: // long
        std::swap(buf[0], buf[4]);
        std::swap(buf[1], buf[5]);
        std::swap(buf[2], buf[6]);
        std::swap(buf[3], buf[7]);
        break;
    case 16: // GUID
        std::swap(buf[0], buf[8]);
        std::swap(buf[1], buf[9]);
        std::swap(buf[2], buf[10]);
        std::swap(buf[3], buf[11]);
        std::swap(buf[4], buf[12]);
        std::swap(buf[5], buf[13]);
        std::swap(buf[6], buf[14]);
        std::swap(buf[7], buf[15]);
        break;
    default:
        if (size % 2 == 0) {
            size_t a, b;

            a = 0;
            b = size / 2;
            while (b < size) {
                std::swap(buf[a], buf[b]);
                a += 1;
                b += 1;
            }
        }
        else {
            throw EndianException("non multiple of 2 size.");
        }
    }
}

// PDP <-> Big
/* @NOTE: The function would not accept non power of 2 sizes because
 * PDP values cannot be expressed in non power of 2 bytes.
 */
void LASHE_DECL_EXT invert_endian_mb(uint8_t *buf, const size_t size)
{
    switch (size) {
    case 2: // short
        break;
    case 4: // int
        std::swap(buf[0], buf[1]);
        std::swap(buf[2], buf[3]);
        break;
    case 8: // long
        std::swap(buf[0], buf[3]);
        std::swap(buf[1], buf[2]);
        std::swap(buf[4], buf[7]);
        std::swap(buf[5], buf[6]);
        break;
    case 16: // GUID
        std::swap(buf[0], buf[7]);
        std::swap(buf[1], buf[6]);
        std::swap(buf[2], buf[5]);
        std::swap(buf[3], buf[4]);
        std::swap(buf[8], buf[15]);
        std::swap(buf[9], buf[14]);
        std::swap(buf[10], buf[13]);
        std::swap(buf[11], buf[12]);
        break;
    default:
        if (size != 0 && (size & (size - 1)) == 0) {
            size_t a, b;

            a = 0;
            b = size / 2 - 1;
            while (a < b) {
                std::swap(buf[a], buf[b]);
                a += 1;
                b -= 1;
            }

            a = size / 2;
            b = size - 1;
            while (a < b) {
                std::swap(buf[a], buf[b]);
                a += 1;
                b -= 1;
            }
        }
        else {
            throw EndianException("non power of 2 size.");
        }
    }
}

/********************
 * CONVERSION TO HOST ENDIAN
 */

// Converts little endian BUF to host endian.
void LASHE_DECL_EXT from_endian_l(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_BIG()
    invert_endian_lb(buf, size);
#elif LASHE_IS_ENDIAN_PDP()
    invert_endian_ml(buf, size);
#endif
}

// Converts big endian BUF to host endian.
void LASHE_DECL_EXT from_endian_b(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_LITTLE()
    invert_endian_lb(buf, size);
#elif LASHE_IS_ENDIAN_PDP()
    invert_endian_mb(buf, size);
#endif
}

// Converts middle endian BUF to host endian.
void LASHE_DECL_EXT from_endian_m(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_LITTLE()
    invert_endian_ml(buf, size);
#elif LASHE_IS_ENDIAN_BIG()
    invert_endian_mb(buf, size);
#endif
}

/********************
 * CONVERSION FROM HOST ENDIAN
 */

// Converts host endian BUF from host endian to little endian.
void LASHE_DECL_EXT to_endian_l(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_BIG()
    invert_endian_lb(buf, size);
#elif LASHE_IS_ENDIAN_PDP()
    invert_endian_ml(buf, size);
#endif
}

// Converts host endian BUF from host endian to big endian.
void LASHE_DECL_EXT to_endian_b(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_LITTLE()
    invert_endian_lb(buf, size);
#elif LASHE_IS_ENDIAN_PDP()
    invert_endian_mb(buf, size);
#endif
}

// Converts host endian BUF from host endian to middle endian.
void LASHE_DECL_EXT to_endian_m(uint8_t *buf, const size_t size)
{
#if LASHE_IS_ENDIAN_LITTLE()
    invert_endian_ml(buf, size);
#elif LASHE_IS_ENDIAN_BIG()
    invert_endian_mb(buf, size);
#endif
}


#define __DECL_ALIAS(FUNC)                                                     \
    \
template<typename T>                                                           \
        \
void LASHE_DECL_EXT                                                            \
        FUNC(T &v)                                                             \
    \
{                                                                         \
        FUNC(reinterpret_cast<uint8_t *>(&v), sizeof(T));                      \
    \
}

__DECL_ALIAS(invert_endian_lb);
__DECL_ALIAS(invert_endian_ml);
__DECL_ALIAS(invert_endian_mb);
__DECL_ALIAS(from_endian_l);
__DECL_ALIAS(from_endian_b);
__DECL_ALIAS(from_endian_m);
__DECL_ALIAS(to_endian_l);
__DECL_ALIAS(to_endian_b);
__DECL_ALIAS(to_endian_m);

#undef __DECL_ALIAS

} // namespace ashe

#endif
