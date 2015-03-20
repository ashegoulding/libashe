#include <iostream>

#include "WebSocket.h"

int main()
{
	ashe::WebSocket ws;
	ws.setReuseAddress(true).bind(8222).listen(1);

	ashe::WebSocket client = ws.accept();
	client.handshake();

	while(true)
	{
		try
		{
			ashe::WebSocket::Frame f;
			try
			{
				f = client.receive();
			}
			catch(ssize_t &)
			{
				continue;
			}
			if(f.opcode() == ashe::WebSocket::Frame::OP_CONNECTION_CLOSE)
				break;
			ashe::WebSocket::Frame sending;
			sending.setFin(true).setOpcode(ashe::WebSocket::Frame::OP_TEXT).setPayload(f.getPayload());
			try
			{
				client.post(sending);
			}
			catch(ssize_t &)
			{
				while(true)
				{
					try
					{
						client.post();
						break;
					}
					catch(ssize_t &){}
				}
			}
		}
		catch(int &e)
		{
			std::cerr << strerror(e) << std::endl;
			break;
		}
		catch(ssize_t &e)
		{
			std::cerr << "ssize_t: " << e << std::endl;
			break;
		}
		catch(std::string &e)
		{
			std::cerr << e << std::endl;
			break;
		}
	}

	return 0;
}
