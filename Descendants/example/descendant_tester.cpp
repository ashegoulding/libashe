#include <iostream>

#include "Descendants/DescendantPipe.h"
#include "Descendants/DescendantProcess.h"
#include "UUID/UUID.h"

#include <signal.h>

ashe::DescendantPipe pipeMother;
ashe::DescendantPipe pipeDaughter;

class MyDaughter : public ashe::DescendantProcess
{
public:
	typedef ashe::DescendantProcess motherClass;
	typedef MyDaughter thisClass;

	virtual int main()
	{
		int y = 0;
		std::vector<unsigned char> buf(5);
		std::string msg;

		while(true)
		{
			pipeDaughter.receive(buf, 4);
			buf[4] = 0;
			msg = (char*)buf.data();
			msg = ashe::toLower__(msg);

			if(! msg.compare("ping"))
			{
				msg = "pong";
				pipeMother.post(msg.data(), msg.length());
			}
			else if(! msg.compare("null"))
				*((char*)NULL) = 0;
			else if(! msg.compare("exit"))
				break;
			else if(! msg.compare("self"))
				this->kill(SIGTERM);
			else
			{
				y = 2;
				break;
			}
		}

		return y;
	}
};

MyDaughter daughter;

void onChild(const int signal)
{
	auto joinResult = daughter.join(true);
	auto test = joinResult;
	joinResult = test;

	std::cerr << joinResult.toString() << std::endl;

	exit(0);
}

int main()
{
	::signal(SIGCHLD, ::onChild);

	std::cerr << "My daughter: " << daughter.start().getPID() << std::endl;

	std::vector<unsigned char> buf(5);
	std::string msg;

	while(true)
	{
		std::cin.clear();
		std::cin.sync();
		std::cin >> msg;
		msg = ashe::toLower__(msg.substr(0, 4));
		if(! msg.compare("kill"))
		{
			std::cout << "Killing my daughter ..." << std::endl;
			::daughter.kill(SIGKILL);
			break;
		}
		else if(! msg.compare("term"))
		{
			std::cout << "Terminating my daughter ..." << std::endl;
			::daughter.kill(SIGTERM);
			break;
		}
		pipeDaughter.post(msg.data(), msg.length());
		pipeMother.receive(buf, 4);
		buf[4] = 0;
		std::cout << (msg = (char*)buf.data()) << std::endl;
	}

	pause();

	return 0;
}
