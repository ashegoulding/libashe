#include <iostream>
#include <iomanip>

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

#include <signal.h>

#include "libashe/helper.h"
#include "libashe/UUID.h"
#include "uuid/uuid.h"

static unsigned long long __luuid__ = 0, __lashe__ = 0;
static bool __flag__ = false;
static std::promise<void> __prm__[2];
static std::condition_variable __cv__;
static std::mutex __mtx__;

static void __startSync(const size_t x) noexcept
{
	std::unique_lock<std::mutex> __ul__(::__mtx__);

	::__prm__[x].set_value();
	::__cv__.wait(__ul__);
}

static void __libuuidRun() noexcept
{
	uuid_t id;
	char unparsed[37];

	__startSync(0);
	while(__flag__)
	{
		::uuid_generate_random(id);
		::uuid_unparse(id, unparsed);
		++::__luuid__;
	}
}

static void __asheUUIDRun() noexcept
{
	ashe::UUID id;
	char unparsed[36];

	__startSync(1);
	while(__flag__)
	{
		id = ashe::uuid::generate();
		id.string(unparsed);
		++::__lashe__;
	}
}

void __onSignal(const int sn) noexcept
{
	switch(sn)
	{
	case SIGINT:
		::signal(SIGINT, SIG_DFL);
		::__flag__ = false;
		::__cv__.notify_all();
		std::cerr << "** Interrupted." << std::endl;
		break;
	default: std::abort();
	}
}

int main()
{
	std::thread uuidTh;
	std::thread asheTh;
	unsigned long long uuid, ashe, ruuid, rashe;
	char comp;
	double by;

	try
	{
		static const uint32_t
			initAbilities[] = {ashe::LAANS_OPENSSL, 0},
			initFlags[] = {ashe::LAIF_OSCODE, ASHE_HOST_OSCODE, 0};

		std::future<void> fut[2];

		ashe::initLibAshe(initAbilities, initFlags);

		// Print out samples.
		{
			uuid_t luuid_id;
			ashe::UUID lashe_id;
			char unparsed[37];

			::uuid_generate_random(luuid_id);
			::uuid_unparse(luuid_id, unparsed);
			std::cerr << "libuuid generates: " << unparsed << std::endl;

			lashe_id = ashe::uuid::generate();
			lashe_id.string(unparsed);
			unparsed[36] = 0;
			std::cerr << "libashe generates: " << unparsed << std::endl;
		}

		::__flag__ = true;
		fut[0] = ::__prm__[0].get_future();
		fut[1] = ::__prm__[1].get_future();
		uuidTh = std::thread(__libuuidRun);
		asheTh = std::thread(__asheUUIDRun);
		fut[0].get();
		fut[1].get();
		{
			std::unique_lock<std::mutex> __ul__(::__mtx__);
			::__cv__.notify_all();
		}
	}
	catch(ashe::HelperException &e)
	{
		std::cerr << e.what() << std::endl;
		ashe::deinitLibAshe();
		return 2;
	}
	catch(std::system_error &e)
	{
		std::cerr << e.what() << std::endl;
		ashe::deinitLibAshe();
		return 2;
	}

	::signal(SIGINT, ::__onSignal);
	while(::__flag__)
	{
		uuid = ::__luuid__;
		ashe = ::__lashe__;
		{
			std::unique_lock<std::mutex> __ul__(::__mtx__);
			::__cv__.wait_for(__ul__, std::chrono::seconds(1));
			if(!::__flag__)
				break;
		}

		ruuid = ::__luuid__ - uuid;
		rashe = ::__lashe__ - ashe;
		if(ruuid > rashe)
		{
			comp = '>';
			by = (double)ruuid / (double)rashe;
		}
		else if(ruuid < rashe)
		{
			comp = '<';
			by = (double)rashe / (double)ruuid;
		}
		else
			comp = '=';

		std::cerr << "libuuid: " << ruuid << "uuid/s " << comp << " libashe: " << rashe << "uuid/s";
		if(comp != '=')
			std::cerr << std::setprecision(2) << " (by " << by << ')';
		std::cerr << std::endl;
	}

	uuidTh.join();
	asheTh.join();

	ashe::deinitLibAshe();
	return 0;
}
