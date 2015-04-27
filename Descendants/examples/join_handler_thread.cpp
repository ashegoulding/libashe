#include <iostream>

#include "Descendants/DescendantProcess.h"

#include <thread>

class MyChild : public ashe::DescendantProcess
{
protected:
	virtual int main()
	{
		// TODO: Count to 2^32 and return
		volatile unsigned int i;
		for(i=0; i<0xFFFFFFFF; ++i);
		return 0;
	}
};

void run()
{
	MyChild child;

	while(true)
	{
		try
		{
			while(true)
			{
				std::cout << MyChild::join__(false).toString() << std::endl;
				child.start().detach();
			}
		}
		catch(...){}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{
	MyChild child;
	std::thread th(run);
	child.start().detach();

	th.join();

	return 0;
}
