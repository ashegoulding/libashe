#include <iostream>

#include "UUID/UUID.h"

#include <set>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

#include <unistd.h>
#include <fstream>

bool flag = true;

std::atomic<unsigned long long> collisions(0);
std::mutex mtx;

void run()
{
	typedef std::set<ashe::UUID> SetType;
	const SetType::size_type POOL_SIZE = 1024*1024;
	SetType idSet;
	ashe::UUID::RandomEngine theEngine;
	ashe::UUID id(1);

	theEngine.setPoolSize(256);

	while(::flag)
	{
		id = theEngine();
		if(idSet.find(id) != idSet.end())
		{
			std::lock_guard<std::mutex> l(::mtx);
			std::ofstream file;

			std::cerr << "Collision found: " << ++collisions << std::endl;
			file.open("collisions", std::ofstream::out);
			file << collisions << std::endl;

			continue;
		}
		else if(idSet.size() >= POOL_SIZE)
			idSet.erase(idSet.begin());
		idSet.insert(id);
	}
}

int main()
{
	ashe::UUID id(1);
	ashe::UUID::RandomEngine theEngine;
	std::vector<std::thread> threadVector(std::thread::hardware_concurrency());

	for(auto &v : threadVector)
		v = std::thread(run);

	while(true)
	{
		id = theEngine();
		id.toString();
	}

//	pause();
//	::flag = false;
//
//	for(auto &v : threadVector)
//	{
//		if(v.joinable())
//			v.join();
//	}

	return 0;
}
