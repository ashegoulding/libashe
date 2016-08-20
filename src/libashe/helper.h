#ifndef LASHE_HELPER_H_
#define LASHE_HELPER_H_

#include "AsheException.h"

namespace ashe
{

class LASHE_DECL_EXT HelperException : public AsheException
{
public:
	typedef AsheException motherClass;
	typedef HelperException thisClass;

	enum /* uint32_t */ Code
	{
		C_NONE,
		C_LASHE_UNINITIALISED,
		C_MODULE_ERROR,
		C_INVALID_FORMAT,
		C_ILLEGAL_ARGUMENT,
		C_UNIMPLEMENTED,
		C_UNSUPPORTED
	};

	enum /* int64_t */ SubCode
	{
		SC_NONE,
		SC_MODULE_OPEN,
		SC_FUNC_LOAD,
		SC_MODULE_PREP,
		SC_OS_RELEATED
	};

public:
	virtual ~HelperException() LASHE_NOEXCEPT;

	virtual const char *code2str(const uint32_t x) const LASHE_NOEXCEPT; //@Implement
	virtual const char *subCode2str(const uint32_t x) const LASHE_NOEXCEPT;
};

enum /* uint32_t */ LibAsheInitFlag
{
	LAIF_NONE,
	// Following value being ASHE_HOST_OSCODE of the software.
	LAIF_OSCODE,
	// Continue on ability initialisation failure.
	LAIF_ABILITY_PERMISSIVE
};

LASHE_DECL_EXT void initLibAshe(const /* LibAsheAbilityNamespace */ uint32_t *abilities, const /* LibAsheInitFlag */ uint32_t *flags) LASHE_EXCEPT(HelperException);
LASHE_DECL_EXT_NOEXCEPT void deinitLibAshe() LASHE_NOEXCEPT;
LASHE_DECL_EXT const /* LibAsheAbilityNamespace */ uint32_t *initialisedAbilities() LASHE_EXCEPT(HelperException);
// Why would anyone need this?
LASHE_DECL_EXT bool compareLibAsheVersion(const char *a, const /* LibAsheComparisonOperation */ uint32_t op, const char *b) LASHE_EXCEPT(HelperException);

LASHE_DECL_EXT_NOEXCEPT const char *libAsheVersion__() LASHE_NOEXCEPT;
template<class T>
bool libAsheCompare__(const T a, const /* LibAsheComparisonOperation */ uint32_t op, const T b) LASHE_EXCEPT(HelperException)
{
	switch(op)
	{
	case LACO_EQ: return a == b;
	case LACO_NE: return a != b;
	case LACO_GT: return a > b;
	case LACO_GE: return a >= b;
	case LACO_LT: return a < b;
	case LACO_LE: return a <= b;
	default:
	{
		HelperException e;
		e.code(HelperException::C_ILLEGAL_ARGUMENT).msg("Unknown value: 'op'");
		throw e;
	}
	}
	return false;
}

}

#endif
