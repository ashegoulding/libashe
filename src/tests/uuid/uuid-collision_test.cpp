#include <iostream>

#include <sstream>
#include <regex>

#include <thread>
#include <condition_variable>
#include <future>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <csignal>

#include <libashe/helper.h>
#include "libashe/UUID.h"
#include "libashe/RegexHelper.h"


#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "ashe.goulding@gmail.com"
#endif
const char *PROGRAM_VERSION__ = "0";

bool interrupted__ = false;
uint8_t *buf__ = nullptr;
size_t bufSize__ = 0;
size_t bufCnt__ = 0;

struct ThreadContext
{
	size_t offset;
	size_t stride;
	std::mutex *mtx;
	std::condition_variable *cv;
	ashe::UUID niddle;
	uint8_t *ignore;
	bool thFlag;
	std::thread th;
	std::promise<void> prmInit;
	std::future<void> futInit;
	std::promise<bool> prmRes;
	std::future<bool> futRes;
};

static void __onSig(const int sn) noexcept;
static void __printHelp__(std::ostream &s, const char *programName) noexcept;
static void __compareThreadRun(ThreadContext *ctx) noexcept;

int main(const int argc, const char **args)
{
	static const std::regex
		__RE_OPT_HELP__("^(\\s+)?\\-\\-help(\\s+)?$"),
		__RE_FMT_NUMERAL__(LASHE_REGEX_FORMAT_NUMERAL);
	struct __Exeption
	{
		int exitCode;
		std::string msg;

		__Exeption(const int exitCode = 4, const std::string msg = "")
			: exitCode(exitCode)
			, msg(msg)
		{}
	};

	int ret = 4;
	unsigned int nb_threads;
	std::stringstream sb;
	std::vector<ThreadContext*> thPool;
	std::condition_variable cv;
	std::mutex mtx;
	ashe::uuid::RandomEngine *engine = nullptr;
	uint_fast64_t nb_gen;
	std::mutex mtxBufReport;
	std::condition_variable cvBufReport;
	std::thread thBufReport;
	bool flagThBufReport;
	auto thBufReportRun = [&flagThBufReport, &mtxBufReport, &cvBufReport](){
		static const double __REPORT_GAP__ = 0.1;
		static const auto __CHECK_INT__ = std::chrono::seconds(1);

		double curPos, pos = 0.0;

		std::cerr << "Initialising buffer: ";
		while(flagThBufReport && pos <= 1.0)
		{
			{
				std::unique_lock<std::mutex> ul(mtxBufReport);

				cvBufReport.wait_for(ul, __CHECK_INT__);
			}
			if(!flagThBufReport)
				break;

			curPos = (double)::bufCnt__ / ::bufSize__;
			if(curPos >= pos)
			{
				pos = __REPORT_GAP__ * ((int)(curPos / __REPORT_GAP__) + 1);
				std::cerr << (int)(curPos * 100) << "%... ";
			}
		}
		std::cerr << std::endl;
	};

	::signal(SIGINT, ::__onSig);

	// Check.
	if(argc < 2)
	{
		__printHelp__(std::cerr, args[0]);
		return 1;
	}
	else if(!std::regex_match(args[1], __RE_FMT_NUMERAL__))
	{
		std::cerr << "Invalid value: " << args[1] << std::endl;
		__printHelp__(std::cerr, args[0]);
		return 1;
	}
	// Parse args.
	{
		// Buffer size.
		long long tmp;

		sb.clear();
		sb.str(args[1]);
		sb >> tmp;
		if(tmp <= 0)
		{
			std::cerr << "Invalid value: " << args[1] << std::endl;
			__printHelp__(std::cerr, args[0]);
			return 1;
		}
		::bufSize__ = tmp * ashe::uuid::RAW_BYTE_SIZE;
		std::cerr << "Using " << ::bufSize__ << "bytes of buffer." << std::endl;
	}
	if(argc >= 3)
	{
		int tmp;

		if(!std::regex_match(args[2], __RE_FMT_NUMERAL__))
		{
			std::cerr << "Invalid value: " << args[2] << std::endl;
			__printHelp__(std::cerr, args[0]);
			return 1;
		}
		sb.clear();
		sb.str(args[2]);
		sb >> tmp;
		if(tmp <= 0)
			nb_threads = std::thread::hardware_concurrency();
		else
			nb_threads = (unsigned int)tmp;
	}
	else
		nb_threads = std::thread::hardware_concurrency();

	try
	{
		ashe::UUID uuid;

		// Init LibAshe.
		try
		{
			static const uint32_t __ABILITIES__[] = {ashe::LAANS_OPENSSL, 0};
			static const int32_t __FLAGS__[] = {ashe::LAIF_OSCODE, ASHE_HOST_OSCODE, 0};

			ashe::initLibAshe(__ABILITIES__, __FLAGS__);
		}
		catch(ashe::HelperException &e)
		{
			sb.str("");
			sb.clear();
			sb << "** Failed to initialise LibAshe: " << e.what();
			throw __Exeption(2, sb.str());
		}
		// Init engine.
		try
		{
			engine = ashe::uuid::mkRandomEngine("MersenneTwisterEngine");
			engine->randomise();
		}
		catch(ashe::uuid::Exception &e)
		{
			sb.str("");
			sb.clear();
			sb << "** Failed to make UUID engine: " << e.what();
			throw __Exeption(2, sb.str());
		}
		// Allocate buffer.
		::buf__ = (uint8_t*)::malloc(::bufSize__);
		if(nullptr == ::buf__)
			throw __Exeption(2, "** Failed to allocate buffer.");
		// Start compare threads.
		try
		{
			const size_t STRIDE = nb_threads * ashe::uuid::RAW_BYTE_SIZE;
			size_t i = 0;
			ThreadContext *ctx;

			thPool.resize(nb_threads);
			for(auto &v : thPool)
			{
				v = ctx = new ThreadContext;
				ctx->offset = i * ashe::uuid::RAW_BYTE_SIZE;
				ctx->stride = STRIDE;
				ctx->mtx = &mtx;
				ctx->cv = &cv;
				ctx->thFlag = true;
				ctx->futInit = ctx->prmInit.get_future();
				ctx->th = std::thread(__compareThreadRun, ctx);

				++i;
			}
			// Sync up.
			for(auto &v : thPool)
				v->futInit.get();
		}
		catch(std::exception &e)
		{
			sb.str("");
			sb.clear();
			sb << "** Exception caught whilst spawning threads: " << e.what();
			throw __Exeption(2, sb.str());
		}
		// Start report thread.
		try
		{
			flagThBufReport = true;
			thBufReport = std::thread(thBufReportRun);
		}
		catch(std::system_error &e)
		{
			sb.str("");
			sb.clear();
			sb << "** Exception caught whilst spawning buffer report thread: " << e.what();
			throw __Exeption(2, sb.str());
		}
		{
			const uint8_t *end = ::buf__ + ::bufSize__;
			uint8_t *p = ::buf__;
			uint8_t *ignore;

			// Fill the pool initially.
			while((!::interrupted__) && p < end)
			{
				::memcpy(p, engine->generate().data, ashe::uuid::RAW_BYTE_SIZE);
				::bufCnt__ += ashe::uuid::RAW_BYTE_SIZE;
				p += ashe::uuid::RAW_BYTE_SIZE;
			}
			p = ::buf__;
			nb_gen = 0;
			while(!::interrupted__)
			{
				if(p < end)
				{
					::memcpy(uuid.data, p, ashe::uuid::RAW_BYTE_SIZE);
					ignore = p;
					p += ashe::uuid::RAW_BYTE_SIZE;
				}
				else
				{
					uuid = engine->generate();
					ignore = nullptr;
				}
				++nb_gen;

				for(auto &v : thPool)
				{
					v->niddle = uuid;
					v->prmRes = std::promise<bool>();
					v->futRes = v->prmRes.get_future();
					v->ignore = ignore;
				}
				{
					std::unique_lock<std::mutex> ul(mtx);
					cv.notify_all();
				}

				for(auto &v : thPool)
				{
					if(v->futRes.get())
					{
						char str[37];

						uuid.string(str);
						str[36] = 0;
						sb.str("");
						sb.clear();
						sb << "Collision found!" << std::endl << str << ": #" << nb_gen;
						throw __Exeption(3, sb.str());
					}
				}

				if(nullptr == ignore)
				{
					::memmove(::buf__ + ashe::uuid::RAW_BYTE_SIZE, ::buf__, ::bufSize__ - ashe::uuid::RAW_BYTE_SIZE);
					::memcpy(::buf__, uuid.data, ashe::uuid::RAW_BYTE_SIZE);
				}
			}
		}

		ret = 0;
	}
	catch(__Exeption &e)
	{
		ret = e.exitCode;
		std::cerr << e.msg << std::endl;
	}

	if(thBufReport.joinable())
	{
		flagThBufReport = false;
		{
			std::unique_lock<std::mutex> ul(mtxBufReport);
			cvBufReport.notify_all();
		}
		thBufReport.join();
	}
	{
		std::unique_lock<std::mutex> ul(mtx);

		for(auto &v : thPool)
		{
			if(v)
				v->thFlag = false;
		}
		cv.notify_all();
	}
	for(auto &v : thPool)
	{
		if(!v)
			continue;
		if(v->th.joinable())
			v->th.join();
		delete v;
	}
	delete engine;
	::free(::buf__);
	ashe::deinitLibAshe();

	return ret;
}

void __compareThreadRun(ThreadContext *ctx) noexcept
{
	bool initSync = true;
	const uint8_t *ptr, *end, *ignore;
	size_t stride;
	bool result;
	uint8_t data[ashe::uuid::RAW_BYTE_SIZE];

	while(ctx->thFlag)
	{
		{
			std::unique_lock<std::mutex> ul(*ctx->mtx);

			if(initSync)
			{
				ctx->prmInit.set_value();
				initSync = false;
			}
			else
				ctx->prmRes.set_value(result);
			ctx->cv->wait(ul);
		}
		if(!ctx->thFlag)
			break;

		ptr = ::buf__ + ctx->offset;
		end = ::buf__ + ::bufCnt__;
		stride = ctx->stride;
		ignore = ctx->ignore;
		::memcpy(data, ctx->niddle.data, ashe::uuid::RAW_BYTE_SIZE);
		result = false;
		while(ptr < end)
		{
			if(ignore != ptr && ::memcmp(ptr, data, ashe::uuid::RAW_BYTE_SIZE) == 0)
			{
				result = true;
				break;
			}
			ptr += stride;
		}
	}
}

void __onSig(const int sn) noexcept
{
	if(SIGINT == sn)
	{
		::interrupted__ = true;
		::signal(SIGINT, SIG_DFL);
		std::cerr << "* Interrupted. Interrupt again to terminate." << std::endl;
	}
}

void __printHelp__(std::ostream &s, const char *programName) noexcept
{
	s << "LibAshe UUID RandomEngine collision test case version " << PROGRAM_VERSION__ << '.' << std::endl
		<< "LibAshe: " << ashe::libAsheVersion__() << std::endl
		<< "Usage: " << programName << " <buffer size> [worker threads]" << std::endl
		<< "Options:" << std::endl
		<< "  <buffer size>: Number of UUIDs to buffer." << std::endl
		<< "    Memory use: <buffer size>x16 bytes." << std::endl
		<< "  [worker threads]: Number of worker threads." << std::endl
		<< "    Less than 1, uses the system's hardware concurrency." << std::endl
		<< std::endl
		<< "Bug report: " PACKAGE_BUGREPORT << std::endl;
}
