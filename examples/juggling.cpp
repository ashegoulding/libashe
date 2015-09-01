#include <iostream>

#include "ThreadPool/ThreadPool.h"

class WU : public ashe::ThreadPool::WorkUnit
{
public:
	virtual void onTick() noexcept
	{
		volatile unsigned int i;
		for(i=0; i<0xFFFFF; ++i);
	}
};

WU w;

void run(ashe::ThreadPool *tp)
{
	unsigned short w, r;
	while(true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		tp->count(&r, &w);
		std::cout << "Working: " << w << ", resting: " << r << std::endl;
	}
}

int main()
{
	ashe::ThreadPool tp;
	std::thread th(run, &tp);
	while(true)
		tp.labour(&w);

	return 0;
}
