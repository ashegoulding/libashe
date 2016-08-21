#include <iostream>

#include <string>
#include <vector>

#include <regex>
#include <sstream>
#include <cstring>

#include "libashe/helper.h"
#include "libashe/UUID.h"


const char *DEF_PROG_NAME__ = "asheutil-uuid";
const char *VERSION__ = "0";

static void __printHelp__(std::ostream &s, const char *progName) noexcept;

int main(const int argc, const char **args)
{
	struct __Exception
	{
		std::string msg;
		int exitCode;
	};

	static const std::regex
		__RE_OPT_HELP__("^(\\s+)?\\-\\-help(\\s+)?$"),
		__RE_OPT_GEN__("^(\\s+)?\\-\\-gen(\\s+)?$"),
		__RE_OPT_MERGE__("^(\\s+)?\\-\\-merge(\\s+)?$"),
		__RE_OPT_PARSE__("^(\\s+)?\\-\\-parse(\\s+)?$"),
		__RE_NUMERAL__("^(\\s+)?\\-?[0-9]+(\\s+)?$"),
		__RE_BOOLEAN_TRUE__("^(\\s+)?(false|(\\-?0+))(\\s+)?$", std::regex_constants::icase),
		__RE_BOOLEAN_FALSE__("^(\\s+)?(true|\\-?[0-9]*[1-9][0-9]*)(\\s+)?$", std::regex_constants::icase);
	static const uint32_t __LASHE_INIT_ABILITIES__[] = {ashe::LAANS_OPENSSL, 0};
	static const uint32_t __LASHE_INIT_FLAGS__[] = {ashe::LAIF_OSCODE, ASHE_HOST_OSCODE, 0};

	int exitCode = 0;
	ashe::uuid::RandomEngine *engine = nullptr;

	try
	{
		std::stringstream sb;

		if(argc < 2)
		{
			__printHelp__(std::cerr, (argc > 0? args[0] : nullptr));
			throw 1;
		}

		if(std::regex_match(args[1], __RE_OPT_HELP__))
		{
			__printHelp__(std::cerr, args[0]);
			throw 0;
		}
		else if(std::regex_match(args[1], __RE_OPT_GEN__))
		{
			size_t i, cnt;
			ashe::UUID uuid;
			char str[37];

			if(argc > 2)
			{
				long long tmp;

				if(!std::regex_match(args[2], __RE_NUMERAL__))
				{
					__Exception e;
					sb.str("");
					sb.clear();
					sb << "Invalid value for --gen: " << args[2];
					e.exitCode = 1;
					e.msg = sb.str();
					throw e;
				}
				sb.clear();
				sb.str(args[2]);
				sb >> tmp;
				if(tmp <= 0)
				{
					__Exception e;
					sb.str("");
					sb.clear();
					sb << "Value for --gen is less than 1.";
					e.exitCode = 1;
					e.msg = sb.str();
					throw e;
				}
				cnt = (size_t)tmp;
			}
			else
				cnt = 1;

			ashe::initLibAshe(__LASHE_INIT_ABILITIES__, __LASHE_INIT_FLAGS__);
			engine = ashe::uuid::mkRandomEngine("MersenneTwisterEngine");
			engine->randomise();

			for(i=0; i<cnt; ++i)
			{
				uuid = engine->generate();
				uuid.string(str);
				str[36] = 0;
				std::cout << str << std::endl;
			}
		}
		else if(std::regex_match(args[1], __RE_OPT_MERGE__))
		{
			bool hasMatch;
			ashe::UUID uuid, merged, match;
			char str[37];
			int errUUID;
			size_t len;

			if(argc < 4)
			{
				__Exception e;
				e.exitCode = 1;
				e.msg = "Insufficient arguments.";
				throw e;
			}

			ashe::initLibAshe(__LASHE_INIT_ABILITIES__, __LASHE_INIT_FLAGS__);

			try
			{
				errUUID = 2;
				uuid = ashe::uuid::fromString(args[2]);
				hasMatch = argc > 4;
				if(hasMatch)
				{
					errUUID = 4;
					match = ashe::uuid::fromString(args[4]);
				}
			}
			catch(ashe::uuid::Exception &caught)
			{
				__Exception e;
				sb.str("");
				sb.clear();
				sb << "Error parsing '" << args[errUUID] << "': " << caught.what();
				e.exitCode = 1;
				e.msg = sb.str();
				throw e;
			}

			len = ::strlen(args[3]);
			if(len == 0)
			{
				__Exception e;
				e.exitCode = 1;
				e.msg = "An empty string given.";
				throw e;
			}
			merged = uuid.merge(args[3], len);
			merged.string(str);
			str[36] = 0;
			std::cout << str << std::endl;
			if(hasMatch)
				exitCode = merged == match? 0 : 4;
			else
				exitCode = 0;
		}
		else if(std::regex_match(args[1], __RE_OPT_PARSE__))
		{
			int i;
			ashe::UUID id;

			if(argc < 3)
			{
				__Exception e;
				e.exitCode = 1;
				e.msg = "No UUID given.";
				throw e;
			}

			ashe::initLibAshe(__LASHE_INIT_ABILITIES__, __LASHE_INIT_FLAGS__);

			for(i=2; i<argc; ++i)
			{
				try
				{
					id = ashe::uuid::fromString(args[i]);
					try
					{
						ashe::uuid::validateRaw(id.data);
					}
					catch(ashe::uuid::Exception &ca)
					{
						__Exception e;
						sb.str("");
						sb.clear();
						sb << args[i] << ": " << ca.what();
						e.exitCode = 3;
						e.msg = sb.str();
						throw e;
					}
				}
				catch(ashe::uuid::Exception &ca)
				{
					std::cerr << args[i] << ": " << ca.what() << std::endl;
					exitCode = 1;
				}
			}
		}
		else
		{
			__Exception e;
			sb.str("");
			sb.clear();
			sb << "Unknown option: " << args[1];
			e.exitCode = 1;
			e.msg = sb.str();
			throw e;
		}
	}
	catch(ashe::AsheException &e)
	{
		std::cerr << e.what() << std::endl;
		exitCode = 2;
	}
	catch(__Exception &e)
	{
		std::cerr << e.msg << std::endl;
		exitCode = e.exitCode;
	}
	catch(int &e)
	{
		exitCode = e;
	}

	delete engine;
	ashe::deinitLibAshe();

	return exitCode;
}

void __printHelp__(std::ostream &s, const char *progName) noexcept
{
	const char *programName = progName? progName : ::DEF_PROG_NAME__;

	s << "Ashe's UUID utility version " << VERSION__ << '.' << std::endl
			<< "Usage: " << programName << " <option [values ...]>" << std::endl
			<< "LibAshe: v" << ashe::libAsheVersion__() << std::endl
			<< "Refer https://www.ietf.org/rfc/rfc4122.txt" << std::endl
			<< "RFC4122 Namespaces:" << std::endl
			<< "   DNS:     " LASHE_UUID_NS_DNS << std::endl
			<< "   URL:     " LASHE_UUID_NS_URL << std::endl
			<< "   ISO_OID: " LASHE_UUID_NS_ISO_OID << std::endl
			<< "   X500:    " LASHE_UUID_NS_X500 << std::endl
			<< "Options:" << std::endl
			<< "  --help" << std::endl
			<< "    Print this and exit normally." << std::endl
			<< "  --gen [count = 1]" << std::endl
			<< "    Generate and print out [count] of version 4 UUIDs to stdout." << std::endl
			<< "  --merge <UUID 1> <String to merge> [UUID match]" << std::endl
			<< "    Merge a namesapce UUID with a string and print the result UUID." << std::endl
			<< "    If [UUID match] is given and not identical to the result, exit code will be 4." << std::endl
			<< "    <UUID 1> is usually a namespace UUID," << std::endl
			<< "    which can be the one from RFC4122 or the one that you composed on your own." << std::endl
			<< "  --parse <UUID 1 [UUID n ...]>" << std::endl
			<< "    Parse and validate the given UUIDs." << std::endl
			<< "Exit Codes:" << std::endl
			<< "  0: Normal exit." << std::endl
			<< "  1: Wrong usage or parse error." << std::endl
			<< "  2: LibAshe exception." << std::endl
			<< "  3: Binary validation failure(internal error)." << std::endl
			<< "  4: Operation successful but the result does not match." << std::endl;
}
