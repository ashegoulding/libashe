/*
 * No comments needed
 *
 * @Author
 *  - Ashe David Sterkhus
 *  - Blame to: ashe.goulding+blame@gmail.com
 */
#include <iostream>

#include "Semaphore/Semaphore.h"
#include <thread>

#include <uuid/uuid.h>

#include <array>
#include <fstream>

ashe::Semaphore<double> sem(1000.0);

void run()
{
	std::array<char, 37> buf;
	std::string fileName;
	uuid_t uuid;
	while(true)
	{
		// (1000.0 / 1.0) threads are allowed to run in this section
		ashe::Semaphore<double>::Ticket t(sem);
		//		sem.enter();
		uuid_generate_random(uuid);
		uuid_unparse(uuid, buf.data());
		fileName = "tmp/";
		fileName += buf.data();

		std::ofstream file;
		file.open(fileName.c_str(), std::ofstream::out | std::ofstream::trunc);
		if(file.is_open())
		{
			uuid_generate(uuid);
			file << uuid << std::endl;

			file.close();
			std::remove(fileName.c_str());
		}
		else
			std::cerr << "Could not open file: " << strerror(errno) << std::endl;
//		sem.leave();
	}
}

int main()
{
	const size_t THREADS = 2048;
	size_t i;
	std::thread th[THREADS];

	std::cout << sem.toString() << std::endl;

	for(i=0; i<THREADS; ++i)
		th[i] = std::thread(run);

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << sem.getTraffic() << std::endl;
	}

	return 0;
}
