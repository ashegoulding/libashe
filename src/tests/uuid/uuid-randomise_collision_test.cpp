#include <iostream>

#include <string>
#include <vector>

#include <thread>
#include <condition_variable>
#include <future>

#include <csignal>

#include "libashe/helper.h"
#include "libashe/UUID.h"

struct __Context
{
	ashe::uuid::RandomEngine *engine = nullptr;
	bool flag = false;
	std::promise<void> initPrm;
	std::promise<ashe::UUID> resultPrm;
	std::future<ashe::UUID> resultFut;
};

struct __Thread
{
	size_t id;
	__Context ctx;
	std::thread th;
};

static std::mutex __mtx__;
static std::condition_variable __cv__;
static bool __int__ = false;

static void __run(__Context *ctx) noexcept
{
	bool initSync = false;
	bool hasResult = false;
	ashe::UUID result;

	while(ctx->flag)
	{
		{
			std::unique_lock<std::mutex> __ul__(::__mtx__);

			if(!initSync)
			{
				ctx->initPrm.set_value();
				initSync = true;
			}
			if(hasResult)
			{
				hasResult = false;
				ctx->resultPrm.set_value(result);
			}
			::__cv__.wait(__ul__);
		}
		result = ctx->engine->randomise().generate();
		hasResult = true;
	}
}

static void __onInt(const int sn) noexcept
{
	switch(sn)
	{
	case SIGINT:
		::__int__ = true;
		::signal(SIGINT, SIG_DFL);
		std::cerr << "** Interrupted." << std::endl;
		break;
	}
}

int main()
{
	struct __Exception
	{
		int exitCode;
		std::string msg;
	};

	int exitCode = 0;
	int pass = 0;
	std::vector<__Thread*> thPool;
	std::vector<ashe::UUID> ids;

	try
	{
		static const uint32_t __ABILITIES__[] = {ashe::LAANS_OPENSSL, 0};
		static const int32_t __FLAGS__[] = {ashe::LAIF_OSCODE, ASHE_HOST_OSCODE, 0};

		std::future<void> initFut;
		size_t i, j, cnt;
		std::vector<ashe::UUID>::iterator it;
		char uuidStr[37];

		// Check: Number of thread.
		cnt = std::thread::hardware_concurrency();
		if(cnt < 2)
		{
			__Exception e({1, "Insufficient number of threads to perform the test. 2 or more threads are needed."});
			throw e;
		}

		ashe::initLibAshe(__ABILITIES__, __FLAGS__);

		// Spawn threads.
		thPool.resize(cnt);
		ids.resize(cnt);
		i = 0;
		for(auto &v : thPool)
		{
			v = new __Thread;
			v->id = i++;
			v->ctx.engine = ashe::uuid::mkRandomEngine("MersenneTwisterEngine");
			v->ctx.flag = true;
			initFut = v->ctx.initPrm.get_future();
			v->th = std::thread(__run, &v->ctx);
			initFut.get(); // Initial sync.
		}

		// Go into loop. Handle interruption as end of loop.
		::signal(SIGINT, __onInt);
		pass = 1;
		while(!::__int__)
		{
			// Prep result UUID futures.
			for(auto &v : thPool)
			{
				v->ctx.resultPrm = std::promise<ashe::UUID>();
				v->ctx.resultFut = v->ctx.resultPrm.get_future();
			}
			// Get the threads to work.
			{
				std::unique_lock<std::mutex> __ul__(::__mtx__);
				::__cv__.notify_all();
			}
			// Sync up with the results.
			it = ids.begin();
			for(auto &v : thPool)
				*(it++) = v->ctx.resultFut.get();
			if(::__int__)
				break;

			// Look for any collision.
			for(i=0; i<cnt; ++i)
			{
				for(j=0; j<cnt; ++j)
				{
					if(i == j)
						continue;
					const auto &uuidI = ids[i];
					const auto &uuidJ = ids[j];
					if(uuidI == uuidJ)
					{
						uuidI.string(uuidStr);
						uuidStr[36] = 0;
						pass = -1;
						std::cerr << "Collision! #" << i << " with #" << j << ": " << uuidStr << std::endl;
					}
				}
			}
		}
	}
	catch(ashe::HelperException &e)
	{
		std::cerr << e.what() << std::endl;
		exitCode = 1;
		pass = 0;
	}
	catch(std::system_error &e)
	{
		std::cerr << e.what() << std::endl;
		exitCode = 1;
		pass = 0;
	}
	catch(__Exception &e)
	{
		std::cerr << e.msg << std::endl;
		exitCode = e.exitCode;
		pass = 0;
	}

	if(pass)
		std::cout << (pass > 0? "PASS" : "FAIL") << std::endl;

	for(auto &v : thPool)
		v->ctx.flag = false;
	{
		std::unique_lock<std::mutex> __ul__(::__mtx__);
		::__cv__.notify_all();
	}
	for(auto &v : thPool)
	{
		if(v->th.joinable())
			v->th.join();
		delete v->ctx.engine;
		delete v;
	}

	ashe::deinitLibAshe();

	return exitCode;
}
