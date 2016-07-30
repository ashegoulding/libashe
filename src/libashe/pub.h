/* LibAshe Public Definitions
*/
#ifndef LASHE_PUB_H_
#define LASHE_PUB_H_

// For size_t
#include <cstddef>
#include <cstdint>

/* Alignment of public structures and classes.
* Use)
*	#pragma pack(push, LASHE_PUB_ALIGN)
*	// ...: Structure and class definitions.
*	#pragma pack(pop)
*/
#define LASHE_PUB_ALIGN 8

/* Endian testing macros for endian-independent codes.
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
#ifdef _MSC_BUILD
#define LASHE_IS_ENDIAN_LITTLE() (0x41424344UL == 'ABCD')
#define LASHE_IS_ENDIAN_BIG() (0x44434241UL == 'ABCD')
#define LASHE_IS_ENDIAN_PDP() (0x42414443UL == 'ABCD')
#else
#define LASHE_IS_ENDIAN_LITTLE() (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define LASHE_IS_ENDIAN_BIG() (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define LASHE_IS_ENDIAN_PDP() (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#endif

// Endian check.
#if !(LASHE_IS_ENDIAN_LITTLE() || LASHE_IS_ENDIAN_BIG() || LASHE_IS_ENDIAN_PDP())
#error "Unknown endian host."
#endif

/* Macros for Windows portability.
* __declspec for link.exe and nothrow.
* Windows:
*	It is highly recommended to link LibAshe as DLL.
*	However, the benefits of static library is not negligible.
*	LibAshe is made possible to link statically. Define 'LASHE_LINK_STATIC' to do so.
*	'LASHE_BUILD_DLL' is defined when building LibAshe itself.
*	This should be defined when building LibAshe from other software.
* Other platforms:
*	No macro needs to be defined to use LibAshe on other platforms.
*/
#ifdef _MSC_BUILD
#define LASHE_NOEXCEPT
#define LASHE_EXCEPT(...)

#if defined(LASHE_LINK_STATIC) && defined(LASHE_BUILD_DLL)
#error "You joking, right?"
#endif

#ifdef LASHE_BUILD_DLL
#define LASHE_DECL_EXT __declspec(dllexport)
#define LASHE_DECL_EXT_NOEXCEPT __declspec(dllexport, nothrow) // For functions only
#elif !defined(LASHE_LINK_STATIC)
#define LASHE_DECL_EXT __declspec(dllimport)
#define LASHE_DECL_EXT_NOEXCEPT __declspec(dllimport, nothrow) // For functions only
#endif

#else

#define LASHE_NOEXCEPT noexcept
#define LASHE_EXCEPT(...) throw(__VA_ARGS__)
#define LASHE_DECL_EXT
#define LASHE_DECL_EXT_NOEXCEPT

#endif

/* OS Codes.
* 'LASHE_HOST_OSCODE' must be defined as one of the value below:
*	- 'LASHE_OSCODE_POSIX'	0x01
*	- 'LASHE_OSCODE_WIN7'	0x20
*	- 'LASHE_OSCODE_WIN10'	0x21
*/
#ifndef LASHE_HOST_OSCODE
#error "'LASHE_HOST_OSCODE' undefined."
#endif

#define LASHE_ISOS_POSIX() (LASHE_HOST_OSCODE == 0x01)
#define LASHE_ISOS_WIN() (LASHE_HOST_OSCODE == 0x20)

// OS check.
#if !(LASHE_ISOS_POSIX() || LASHE_ISOS_WIN())
#error "Unrecognised 'LASHE_HOST_OSCODE'"
#endif


enum /* uint32_t */ LibAsheAbilityNamespace
{
	LAANS_NONE,
	LAANS_DESCENDANT,
	LAANS_NET_SOCKET,
	LAANS_WEBSOCKET,
	LAANS_HUMAN_LANG
};

enum /* uint32_t */ LibAsheComparisonOperation
{
	LACO_NONE,
	// Equals to
	LACO_EQ,
	// Not equals to
	LACO_NE,
	// Greater than
	LACO_GT,
	// Greater than or equals to
	LACO_GE,
	// Less than
	LACO_LT,
	// Less than or equals to
	LACO_LE
};

#endif
