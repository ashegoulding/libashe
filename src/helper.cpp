#include "libashe/helper.h"

#include "__internal.h"


namespace ashe
{

LASHE_DECL_EXT void initLibAshe(const uint32_t *abilities, const uint32_t *flags) LASHE_EXCEPT(HelperException)
{
	if(__lashe_initialised)
		return;
	// TODO
	__drop_unimplemented<HelperException>();
}

LASHE_DECL_EXT_NOEXCEPT void deinitLibAshe() LASHE_NOEXCEPT
{
	if(!__lashe_initialised)
		return;
	// TODO
}

LASHE_DECL_EXT const uint32_t *initialisedAbilities() LASHE_EXCEPT(HelperException)
{
	__dropif_uninitialised<HelperException>();
	return __lashe_initialisedAbilities;
}

LASHE_DECL_EXT bool compareLibAsheVersion(const char *a, const uint32_t op, const char *b) LASHE_EXCEPT(HelperException)
{
	static auto __prune__ = [](std::string &x)
	{
		for(auto &v : x)
		{
			if(!('0' <= v && v <= '9'))
				v = ' ';
		}
	};
	// Assuming each digit is less than 2^16
	struct __Version
	{
		int c, r, a;
		uint64_t val;
	};

	__dropif_uninitialised<HelperException>();
	std::string strA = a;
	std::string strB = b;
	std::stringstream sb;
	__Version verA, verB;

	{
		// Check.
		const bool vA = std::regex_match(strA, *__lashe_re_version);
		const bool vB = std::regex_match(strB, *__lashe_re_version);

		if(vA || vB)
		{
			HelperException e;

			if(vA && vB)
				e.msg("Argument 'a' and 'b'.");
			else if(vA)
				e.msg("Argument 'a'.");
			else
				e.msg("Argument 'b'.");
			e.code(HelperException::C_INVALID_FORMAT);
			throw e;
		}
	}

	__prune__(strA);
	__prune__(strB);
	sb.str(strA);
	sb >> verA.c >> verA.r >> verA.a;
	sb.clear();
	sb.str(strB);
	sb >> verB.c >> verB.r >> verB.a;
	verA.val = ((uint64_t)verA.c << 32) + ((uint64_t)verA.r << 16) + (uint64_t)verA.a;
	verB.val = ((uint64_t)verB.c << 32) + ((uint64_t)verB.r << 16) + (uint64_t)verB.a;

	return libAsheCompare__<uint64_t>(verA.val, op, verB.val);
}

LASHE_DECL_EXT_NOEXCEPT const char *libAsheVersion__() LASHE_NOEXCEPT
{
	return PACKAGE_VERSION; // Autoconf or Visual Studio project properties.
}

/********************
* HelperException
*/
HelperException::~HelperException() LASHE_NOEXCEPT{}

const char *HelperException::code2str(const uint32_t x) const LASHE_NOEXCEPT
{
	static const char *__str__[] ={
			"LibAshe uninitialised",
			"module error",
			"invalid format",
			"illegal argument",
			"unimplemented",
			"unsupported"
	};

	switch(x)
	{
	case C_LASHE_UNINITIALISED:
	case C_MODULE_ERROR:
	case C_INVALID_FORMAT:
	case C_ILLEGAL_ARGUMENT:
	case C_UNIMPLEMENTED:
	case C_UNSUPPORTED:
		return __str__[x];
	}
	return motherClass::code2str(x);
}

const char *HelperException::subCode2str(const uint32_t x) const LASHE_NOEXCEPT
{
	static const char *__str__[] ={
			"module open error",
			"function loading error",
			"module preparation error",
			"os related"
	};

	switch(x)
	{
	case SC_MODULE_OPEN:
	case SC_FUNC_LOAD:
	case SC_MODULE_PREP:
	case SC_OS_RELEATED:
		return __str__[x];
	}
	return motherClass::code2str(x);
}

}
