#include <iostream>

#include "ThreadPool/ThreadPool.h"
#include "Semaphore/Semaphore.h"
#include "Number.h"

#include <chrono>

#include <unistd.h>

std::promise<bool> prom;
ashe::Semaphore<unsigned long long> sem(2);

class FutureWU : public ashe::ThreadPool::WorkUnit
{
public:
	typedef ashe::ThreadPool::WorkUnit motherClass;
	typedef FutureWU thisClass;

	virtual void onTick() noexcept
	{
		prom.set_value(true);
	}
};

class SemWU : public ashe::ThreadPool::WorkUnit
{
public:
	typedef ashe::ThreadPool::WorkUnit motherClass;
	typedef SemWU thisClass;

	virtual void onTick() noexcept
	{
		ashe::Semaphore<unsigned long long>::Ticket t(::sem);
		prom.set_value(true);
	}
};

FutureWU fw;
SemWU sw;

int main(const int argc, const char **args)
{
	ashe::ThreadPool tp;
	std::future<bool> f;
	std::chrono::high_resolution_clock::time_point start, end;
	unsigned int iterateFor = 0, iterator;
	unsigned long long semDel, futDel, futAvg, semAvg;
	bool firstIteration = true, verbosity = true;

	if(argc > 1)
		iterateFor = ashe::Number<unsigned int>::fromString(args[1]);
	while(! iterateFor)
	{
		std::cout << "Iterate for how many? >";
		std::cin >> iterateFor;
		std::cin.sync();
		std::cin.clear();
	}
	if(argc > 2)
	{
		if(std::string(args[2]) == "-")
			verbosity = false;
	}

	for(iterator=0; iterator<iterateFor; ++iterator)
	{
		::prom = std::promise<bool>();
		f = ::prom.get_future();
		start = std::chrono::high_resolution_clock::now();
		tp.labour(&::fw);
		f.get();
		end = std::chrono::high_resolution_clock::now();
		futDel = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		::prom = std::promise<bool>();
		f = ::prom.get_future();
		start = std::chrono::high_resolution_clock::now();
		tp.labour(&::sw);
		f.get();
		end = std::chrono::high_resolution_clock::now();
		semDel = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		if(firstIteration)
		{
			futAvg = futDel;
			semAvg = semDel;
		}
		else
		{
			futAvg = (futDel + futAvg) / 2;
			semAvg = (semDel + semDel) / 2;
		}
		if(verbosity)
			std::cout << iterator << ": Future = " << futDel << "us, Sem = " << semDel << "us" << std::endl;
	}

	if(verbosity)
		std::cout << "===" << std::endl;
	std::cout << "Future: " << futAvg << "us, Semaphore: " << semAvg << "us" << std::endl;

	return 0;
}
