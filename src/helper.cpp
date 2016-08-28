#include "libashe/helper.h"
#include "__internal.h"
#include "__ArrayArgument.h"
#include "__openssl.h"
#include "libashe/UUID.h"
#include "libashe/RegexHelper.h"


namespace ashe
{

LASHE_DECL_EXT void initLibAshe(const uint32_t *abilities, const uint32_t *flags) LASHE_EXCEPT(HelperException)
{
	if(__lashe_initialised)
		return;
	try
	{
		size_t i, iCnt;
		std::set<LibAsheAbilityNamespace> __abilities;
		auto parsed = __aa::parseArrayArgument__<uint32_t>(flags, {{
				{LAIF_OSCODE, 1},
				{LAIF_ABILITY_PERMISSIVE, 0}
		}});
		// Parse flags.
		auto pIt = parsed.find(LAIF_OSCODE);
		if(pIt->second.appeared && (!pIt->second.list.empty()))
		{
			static auto __THROW_INCOMP__ = []()
			{
				HelperException e;
				e.code(HelperException::C_INCOMPATIBLE_HOST_OS);
				throw e;
			};
			const auto &code = pIt->second.list[0];
#if ASHE_ISOS_POSIX()
			if(ASHE_HOST_OSCODE == 0x01)
			{
				if(ASHE_HOST_OSCODE != code)
				{
					// TODO: Warn.
				}
			}
			else if(ASHE_HOST_OSCODE != code)
				__THROW_INCOMP__();
#elif ASHE_ISOS_WIN()
			if(ASHE_HOST_OSCODE > code)
				__THROW_INCOMP__();
			else if(ASHE_HOST_OSCODE < code)
			{
				// TODO: Warn.
			}
#endif
		}
		else
		{
			HelperException e;
			e
				.code(HelperException::C_ILLEGAL_ARGUMENT)
				.msg("'Missing argument: LAIF_OSCODE'")
				.arg1(LAIF_OSCODE);
			throw e;
		}

		const auto &permissive = parsed[LAIF_ABILITY_PERMISSIVE].appeared;

		// Parse requested abilities.
		if(abilities)
		{
			while(*abilities)
			{
				switch(*(abilities++))
				{
				// FIXME
				case LAANS_DESCENDANT:
				case LAANS_NET_SOCKET:
				case LAANS_HUMAN_LANG:
				case LAANS_HTTP:
				case LAANS_WEBSOCKET:
					__drop_unimplemented<HelperException>();
					break;
				case LAANS_OPENSSL:
					__abilities.insert(LAANS_OPENSSL);
					break;
				}
			}
		}
		else
		{
			// Every single ability.
			// __abilities.insert(LAANS_DESCENDANT); FIXME
			// __abilities.insert(LAANS_NET_SOCKET); FIXME
			// __abilities.insert(LAANS_HUMAN_LANG); FIXME
			__abilities.insert(LAANS_OPENSSL);
			// __abilities.insert(LAANS_HTTP); FIXME
			// __abilities.insert(LAANS_WEBSOCKET); FIXME
		}

		// Initialise the modules discrete in order.
		try
		{
			__initOpenSSL();
		}
		catch(HelperException &e)
		{
			if(!permissive)
				throw e;
		}

		// Initialise globals.
		{
			auto it = __abilities.begin();

			iCnt = __abilities.size();
			__lashe_initialisedAbilities = (uint32_t*)::malloc(iCnt * sizeof(uint32_t));
			for(i=0; i<iCnt; ++i)
				__lashe_initialisedAbilities[i] = *(it++);
			__lashe_initialisedAbilitiesSet = new std::set<LibAsheAbilityNamespace>();
			__lashe_initialisedAbilitiesSet->swap(__abilities);
		}
		__lashe_re_version = new std::regex(LASHE_REGEX_LIBASHE_VERSION);
		__lashe_format_numeral = new std::regex(LASHE_REGEX_FORMAT_NUMERAL);
		__lashe_format_booleanTrue = new std::regex(LASHE_REGEX_FORMAT_BOOLEAN_TRUE, std::regex_constants::icase);
		__lashe_format_booleanFalse = new std::regex(LASHE_REGEX_FORMAT_BOOLEAN_FALSE, std::regex_constants::icase);
		__lashe_re_uuidHusk = new std::regex(LASHE_UUID_REGEX_HUSK, std::regex_constants::icase);
		__lashe_re_uuidStrict = new std::regex(LASHE_UUID_REGEX_STRICT, std::regex_constants::icase);
		__lashe_format_base64 = new std::regex(LASHE_REGEX_FORMAT_BASE64, std::regex_constants::icase);
		__lashe_format_base64url = new std::regex(LASHE_REGEX_FORMAT_BASE64URL, std::regex_constants::icase);

		__lashe_initialised = true;

		// Post init.
		if(__hasAbility(LAANS_OPENSSL))
		{
			__lashe_defUUIDEngine = uuid::mkRandomEngine("MersenneTwisterEngine");
			__lashe_defUUIDEngine->randomise();
			__lashe_mtx_defUUIDEngine = new std::mutex();
		}
	}
	catch(HelperException &e)
	{
		deinitLibAshe();
		throw e;
	}
}

LASHE_DECL_EXT_NOEXCEPT void deinitLibAshe() LASHE_NOEXCEPT
{
	if(!__lashe_initialised)
		return;

	// Deinit globals.
	::free(__lashe_initialisedAbilities);
	delete __lashe_initialisedAbilitiesSet;
	delete __lashe_re_version;
	delete __lashe_format_numeral;
	delete __lashe_format_booleanTrue;
	delete __lashe_format_booleanFalse;
	delete __lashe_re_uuidHusk;
	delete __lashe_re_uuidStrict;
	delete __lashe_defUUIDEngine;
	delete __lashe_mtx_defUUIDEngine;
	delete __lashe_format_base64;
	delete __lashe_format_base64url;
	__lashe_initialisedAbilities = nullptr;
	__lashe_initialisedAbilitiesSet = nullptr;
	__lashe_re_version =
		__lashe_format_numeral =
		__lashe_format_booleanTrue =
		__lashe_format_booleanFalse =
		__lashe_re_uuidHusk =
		__lashe_re_uuidStrict =
		__lashe_format_base64 =
		__lashe_format_base64url = nullptr;
	__lashe_defUUIDEngine = nullptr;
	__lashe_mtx_defUUIDEngine = nullptr;

	// Deinit modules.
	__deinitOpenSSL();

	__lashe_initialised = false;
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
			"none",
			"LibAshe uninitialised",
			"module error",
			"invalid format",
			"illegal argument",
			"unimplemented",
			"unsupported",
			"incompatible host os"
	};

	switch(x)
	{
	case C_NONE:
	case C_LASHE_UNINITIALISED:
	case C_MODULE_ERROR:
	case C_INVALID_FORMAT:
	case C_ILLEGAL_ARGUMENT:
	case C_UNIMPLEMENTED:
	case C_UNSUPPORTED:
	case C_INCOMPATIBLE_HOST_OS:
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
