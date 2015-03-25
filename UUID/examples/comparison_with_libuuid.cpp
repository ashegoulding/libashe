#include <iostream>

#include "UUID/UUID.h"
#include "uuid/uuid.h"

#include <thread>
#include <iomanip>

static unsigned long long luuid = 0, lashe = 0;

void libuuidRun()
{
	uuid_t id;
	char unparsed[37];
	while(true)
	{
		::uuid_generate_random(id);
		::uuid_unparse(id, unparsed);
		++::luuid;
	}
}

void asheUUIDRun()
{
	ashe::UUID::RandomEngine theEngine;
	ashe::UUID id = theEngine();
	while(true)
	{
		id = theEngine();
		id.toString();
		++::lashe;
	}
}

int main()
{
	std::thread uuidTh(libuuidRun);
	std::thread asheTh(asheUUIDRun);

	unsigned long long uuid, ashe, ruuid, rashe;
	char comp;
	double by;

	while(true)
	{
		uuid = ::luuid;
		ashe = ::lashe;
		std::this_thread::sleep_for(std::chrono::seconds(1));

		ruuid = ::luuid - uuid;
		rashe = ::lashe - ashe;
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

	return 0;
}
