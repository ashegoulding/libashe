#ifndef ASHE_MFCLINKAGE_H_
#define ASHE_MFCLINKAGE_H_

#ifdef _MSC_BUILD
/* Preprocessors for MOTHERFUCKING Microsoft's MFC. Seriously, fuck Microsoft.
*
* For now, optimisations with noexcept does not work.
*/
#include "stdafx.h"
#define ASHE_NOEXCEPT

#ifdef _AFXEXT
#define ASHE_DECL_EXT __declspec(dllexport)
#define ASHE_DECL_EXT_NOEXCEPT __declspec(dllexport, nothrow) // For functions only
#else
#define ASHE_DECL_EXT __declspec(dllimport)
#define ASHE_DECL_EXT_NOEXCEPT __declspec(dllimport, nothrow) // For functions only
#endif

#else

#define ASHE_NOEXCEPT noexcept
#define AFX_EXT_API
#define ASHE_DECL_EXT
#define ASHE_DECL_EXT_NOEXCEPT

#endif

#endif
