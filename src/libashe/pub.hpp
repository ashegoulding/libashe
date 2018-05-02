/* LibAshe Public Definitions
 */
#ifndef LASHE_PUB_HPP_
#define LASHE_PUB_HPP_

// For size_t
#include <cstddef>
#include <cstdint>

#include "OSCode.hpp"

/* Alignment of public structures and classes.
 * Use)
 *	#pragma pack(push, LASHE_PUB_ALIGN)
 *	// ...: Structure and class definitions.
 *	#pragma pack(pop)
 */
#define LASHE_PUB_ALIGN 8

/* Macros for Windows portability.
 * __declspec for link.exe and nothrow.
 * Windows:
 *	It is highly recommended to link LibAshe as DLL.
 *	However, the benefits of static library is not negligible.
 *	LibAshe is made possible to link statically. Define 'LASHE_LINK_STATIC'
 *to do so. 'LASHE_BUILD_DLL' is defined when building LibAshe itself. This
 *should be defined when building LibAshe from other software. Other platforms:
 *	No macro needs to be defined to use LibAshe on other platforms.
 */
#ifdef _MSC_BUILD
	#define LASHE_NOEXCEPT

	#if defined(LASHE_LINK_STATIC) && defined(LASHE_BUILD_DLL)
		#error "Macro conflict: LASHE_LINK_STATIC, LASHE_BUILD_DLL"
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
	#define LASHE_DECL_EXT
	#define LASHE_DECL_EXT_NOEXCEPT
#endif

// OS check.
#if !(ASHE_ISOS_POSIX() || ASHE_ISOS_WIN())
	#error "Unsupported OS."
#endif


namespace ashe {

enum class LAsheAbility : uint32_t {
    /* OpenSSL
     * Classes:
     *	- Base64Encoder
     *	- Base64Decoder
     *	- MessageDigest
     */
    OPENSSL
    /* Network Socket
     * Optional:
     *	- OpenSSL: Enables TLS layer.
     */
    // NET_SOCKET,
};

enum class LAsheCompOp : uint32_t {
    // Equals to
    EQ,
    // Not equals to
    NE,
    // Greater than
    GT,
    // Greater than or equals to
    GE,
    // Less than
    LT,
    // Less than or equals to
    LE
};

enum class Base64Type {
	PLAIN,
	URL
};

enum class IPVersion {
	NONE,
	V4,
	V6
};

enum class HashAlgorithm {
	NONE,
	MD5,
	SHA1,
	SHA224,
	SHA256,
	SHA384,
	SHA512
};

} // namespace ashe

#endif
