#include <iostream>

#include "WebSocket.h"

#include <sys/poll.h>

#include <sstream>

int forkMain(ashe::WebSocket &ws);

int main(const int argc, const char **args)
{
	unsigned short bindPort;
	std::string bindAddress = "0.0.0.0";
	if(argc < 2)
	{
		std::cerr << "Usage: <Port> [Address]" << std::endl;
		return 1;
	}
	else if(argc > 1)
	{
		std::stringstream sb;
		sb << args[1];
		sb >> bindPort;
	}
	else if(argc > 2)
		bindAddress = args[2];

	std::cout << "Opening server in " << bindAddress << ':' << bindPort << std::endl;
	ashe::WebSocket ws;
	try
	{
		ws.setReuseAddress(true).bind(bindPort, bindAddress).listen(10);
	}
	catch(int &e)
	{
		std::cerr << strerror(e) << std::endl;
		return 2;
	}
	std::cout << "Listening ..." << std::endl;

	std::stringstream sb;
	ashe::WebSocket client;
	pid_t child;
	while(true)
	{
		sb.str(""); sb.clear();
		try
		{
			client = ws.accept();
		}
		catch(int &e)
		{
			client.close();
			std::cerr << strerror(e) << std::endl;
			return 2;
		}
		try
		{
			client.setNonBlock(true);
		}
		catch(int &e)
		{
			client.close();
			std::cerr << strerror(e) << std::endl;
			continue;
		}

		sb << "Accepted from " << ws.getPeerAddressString() << ':' << ws.getPeerPort() << "... ";

		child = fork();
		if(child < 0)
		{
			sb << "Unable to fork(). Dropping this connection!";
			ashe::WebSocket::Frame f;
			std::string msg = "We are sorry. We could not allocate resource to serve you.";
			std::vector<unsigned char> payload((const unsigned char*)msg.c_str(), (const unsigned char*)msg.c_str() + msg.length());
			f.setFin(true).setOpcode(ashe::WebSocket::Frame::OP_TEXT).setPayload(payload);
			client.post(f);

			f.setFin(true).setOpcode(ashe::WebSocket::Frame::OP_CONNECTION_CLOSE).clearPayload();
			client.post(f);
		}
		else if(!child)
		{
			ws.fd = -1;
			return forkMain(client);
		}
		else
			sb << "Charging child: " << child;

		std::cout << sb.str() << std::endl;
	}

	return 0;
}

int forkMain(ashe::WebSocket &ws)
{
	pollfd pf;
	pf.fd = ws.fd;
	pf.events = POLLIN | POLLERR;
	while(true)
	{
		pf.revents = 0;
		poll(&pf, 1, -1);
		try
		{
			ws.handshake();
		}
		catch(std::string &e)
		{
			return 1;
		}
		catch(int &e)
		{
			return 2;
		}
		catch(ssize_t &e)
		{
			if(e < 0)
				pf.events |= POLLOUT;
		}
		break;
	}

	for(const auto &v : ws.getRequestedValues())
		std::cerr << v.first << ": " << v.second << std::endl;

	ashe::WebSocket::Frame f, sending;
	bool running = true;
	while(running)
	{
		pf.events = POLLIN | POLLERR | POLLHUP | POLLRDHUP;
		pf.revents = 0;

		if(poll(&pf, 1, -1) < 0)
			return 2;
		else if((pf.revents & POLLERR) || (pf.revents & POLLHUP) || (pf.revents & POLLRDHUP))
			return 0;
		else if(pf.revents & POLLOUT)
		{
			try
			{
				ws.post();
				pf.events &= ~POLLOUT;
				if(sending.opcode() == ashe::WebSocket::Frame::OP_CONNECTION_CLOSE)
				{
					running = false;
				}
			}
			catch(ssize_t &)
			{
			}
		}
		else if(pf.revents & POLLIN)
		{
			try
			{
				f = ws.receive();
				if(f.opcode() == ashe::WebSocket::Frame::OP_CONNECTION_CLOSE)
				{
					sending.setFin(true).setOpcode(ashe::WebSocket::Frame::OP_CONNECTION_CLOSE).clearPayload();
					try
					{
						ws.post(sending);
						running = false;
					}
					catch(ssize_t &)
					{
						pf.events |= POLLOUT;
					}
				}
				else
				{
					sending.setFin(true).setOpcode(f.opcode()).setPayload(f.getPayload());
					try
					{
						ws.post(sending);
					}
					catch(ssize_t &)
					{
						pf.events |= POLLOUT;
					}
				}
			}
			catch(ssize_t &)
			{
			}
		}
	}

	ws.shutdownBoth();
	ws.close();
	return 0;
}
