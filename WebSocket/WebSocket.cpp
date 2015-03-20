/*
 * WebSocket.cpp
 *
 *  Created on: 2015. 2. 10.
 *      Author: david
 */

#include "WebSocket.h"

#include "coreutils/base64.h"
#include "coreutils/sha1.h"

namespace ashe
{
/* WebSocket implementation
 *
 */
WebSocket::WebSocket() noexcept
{
}

WebSocket::WebSocket(const WebSocket& src) noexcept
{
	this->__construct(src);
}

WebSocket::~WebSocket() noexcept
{
	if(this->closeOnDelete)
		this->close(true);
}


WebSocket& WebSocket::operator =(const WebSocket& src) noexcept
{
	this->__construct(src);
	return *this;
}

WebSocket& WebSocket::bind(const unsigned short port, const std::string address) throw (int)
{
	struct sockaddr_in addr;
	int reuse = this->reuseAddr? 1 : 0;
	if((this->fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw errno;
	else if(::setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
		throw errno;
	::memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr = inet_addr(address.c_str());
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(::bind(this->fd, (const sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
		throw errno;
	return *this;
}

WebSocket& WebSocket::listen(const int backlog) throw (int)
{
	if((::listen(this->fd, backlog)) < 0)
		throw errno;
	return *this;
}

WebSocket WebSocket::accept() throw (int)
{
	WebSocket ny;
	if((ny.fd = ::accept(this->fd, NULL, NULL)) < 0)
		throw errno;
	ny.closeOnDelete = false;
	return ny;
}

WebSocket& WebSocket::close(const bool urgently) noexcept
{
	if(urgently)
	{
		struct linger val;
		val.l_onoff = 1;
		val.l_linger = 0;
		::setsockopt(this->fd, SOL_SOCKET, SO_LINGER, &val, sizeof(struct linger));
	}
	else
	{
		try
		{
			this->shutdownBoth();
		}
		catch(...){}
	}
	::close(this->fd);
	this->fd = -1;

	this->__clearStash();
	this->__clearRequest();
	this->closeOnDelete = false;
	return *this;
}

WebSocket::Frame WebSocket::receive() throw (ssize_t, int, std::string)
{
	std::vector<unsigned char> buf;
	buf.resize(this->bufferSize);
	ssize_t readSize = ::read(this->fd, buf.data(), buf.size());
	if(readSize < 0 && this->stash[0].empty())
		throw errno;
	else if(readSize)
		this->stash[0].insert(this->stash[0].end(), buf.begin(), buf.begin() + readSize);

	if(this->remainingRead)
	{
		if(this->remainingRead > this->stash[0].size())
		{
			ssize_t req = this->remainingRead -= this->stash[0].size();
			req *= -1;
			throw req;
		}
		else
			this->remainingRead = 0;
	}

	try
	{
		return Frame(this->stash[0], this->frameSizeLimit);
	}
	catch(ssize_t &e)
	{
		if(e < 0)
			this->remainingRead += -e;
		throw e;
	}
	return Frame();
}

WebSocket &WebSocket::post(const Frame& frame) throw (int, ssize_t)
{
	if(this->stash[1].size())
	{
		ssize_t toThrow = this->stash[1].size();
		throw toThrow;
	}
	frame.toBinary().swap(this->stash[1]);

	ssize_t writeSize = ::write(this->fd, this->stash[1].data(), this->stash[1].size());
	if(writeSize < 0)
		throw errno;
	else if(writeSize != (ssize_t)this->stash[1].size())
	{
		ssize_t toThrow = this->stash[1].size() - writeSize;
		this->stash[1].erase(this->stash[1].begin(), this->stash[1].begin()+writeSize);
		throw toThrow;
	}

	this->stash[1].clear();
	return *this;
}

WebSocket &WebSocket::post() throw (int, ssize_t)
{
	if(this->stash[1].size())
	{
		ssize_t writeSize = ::write(this->fd, this->stash[1].data(), this->stash[1].size());
		if(writeSize < 0)
			throw errno;
		else if(writeSize != (ssize_t)this->stash[1].size())
		{
			ssize_t toThrow = this->stash[1].size() - writeSize;
			this->stash[1].erase(this->stash[1].begin(), this->stash[1].begin()+writeSize);
			throw toThrow;
		}
	}

	this->stash[1].clear();
	return *this;
}

std::string WebSocket::getPeerAddressString() const noexcept
{
	return ::inet_ntoa(*(in_addr*)this->getPeerAddress().data());
}

std::vector<unsigned char> WebSocket::getPeerAddress() const noexcept
{
	socklen_t len;
	struct sockaddr_storage addr;
	::getpeername(this->fd, (struct sockaddr*)&addr, &len);
	struct sockaddr_in *v4 = (struct sockaddr_in*)&addr;
	unsigned char *p = (unsigned char*)&v4->sin_addr.s_addr;
	return std::vector<unsigned char>(p, p + 4);
}

unsigned short WebSocket::getBindPort() const noexcept
{
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	if(::getsockname(this->fd, (sockaddr*)&sin, &len) > 0)
		return ntohs(sin.sin_port);
	return 0;
}

void WebSocket::__construct(const thisClass& src) noexcept
{
	this->fd = src.fd;
	this->__clearStash();
}

WebSocket& WebSocket::handshake() throw (std::string, int, ssize_t)
{
	ssize_t rwSize;
	if(this->handshook)
		throw std::string("Handshake already done.");
	else if(this->wasHandshaking)
	{
		rwSize = ::write(this->fd, this->handshakeStash.c_str(), this->handshakeStash.length());
		if(rwSize < 0)
			throw errno;
		else if(rwSize != (ssize_t)this->handshakeStash.length())
		{
			this->handshakeStash.erase(this->handshakeStash.begin(), this->handshakeStash.begin()+rwSize);
			rwSize *= -1;
			throw rwSize;
		}

		this->wasHandshaking = false;
		this->handshook = true;
		return *this;
	}

	std::string::size_type ending;
	do
	{
		this->buffer.clear();
		this->buffer.resize(this->bufferSize);

		rwSize = ::read(this->fd, this->buffer.data(), this->buffer.size()-1);
		if(rwSize < 0)
			throw errno;
		else if(rwSize == 0)
			throw rwSize;
		else if(rwSize + this->handshakeStash.length() > this->headerLimit)
			throw std::string("Header length has reached the limit.");

		this->buffer[rwSize] = 0;
		this->handshakeStash += (const char*)this->buffer.data();
		ending = this->handshakeStash.find("\r\n\r\n");
	}
	while(ending == std::string::npos);

	std::regex
		lineExp("(.+)"),
		headerNameExp("(.+):"),
		headerValueExp(":(.+)"),
		cgiExp("\\?(.+)");
	std::regex_iterator<std::string::iterator> lineIt(this->handshakeStash.begin(), this->handshakeStash.end(), lineExp), lineItEnd;
	std::smatch result;
	std::string line;
	bool statusLine = false, duplicatable;
	std::map<std::string, std::string> header;
	std::map<std::string, std::string> values;
	std::string headerName, headerValue;

	while(lineIt != lineItEnd)
	{
		line = (lineIt++)->str();
		if(line.length() > this->headerLineLimit)
			throw std::string("Header line limit reached.");
		line = thisClass::trim__(line);

		if(statusLine)
		{
			std::regex_search(line, result, headerNameExp);
			if(! result.size())
				continue;
			headerName = thisClass::toLower__(thisClass::trim__(result[1].str()));

			std::regex_search(line, result, headerValueExp);
			if(! result.size())
				continue;
			headerValue = thisClass::trim__(result[1].str());

			duplicatable = true;
			if(! headerValue.compare("host"))
				duplicatable = false;
			else if(! headerValue.compare("sec-websocket-key"))
				duplicatable = false;

			if(header.find(headerName) == header.end())
				header[headerName] = headerValue;
			else if(duplicatable)
				header[headerName] += ", " + headerValue;
			else
				throw std::string("Header '" + headerName + "' appeared more than once.");
		}
		else
		{
			std::vector<std::string> exploded(thisClass::explodeSpace__(line));
			if(exploded.size() != 3)
				throw std::string("Invalid status: " + lineIt->str());
			else if(thisClass::toUpper__(exploded[0]).compare("GET"))
				throw std::string("Invalid method: " + exploded[0]);

			this->requestedUrl = exploded[1];
			std::regex_iterator<std::string::iterator> it(this->requestedUrl.begin(), this->requestedUrl.end(), cgiExp), itEnd;
			if(it != itEnd)
			{
				std::set<std::string> delim, delimEnt;
				std::vector<std::string> ent;
				std::string name, val;
				delim.insert(std::string("?"));
				delim.insert(std::string("&"));
				delimEnt.insert(std::string("="));
				for(const auto &v : thisClass::explode__(it->str(), delim))
				{
					ent = thisClass::explode__(v, delimEnt);
					name.clear();
					val.clear();
					if(ent.size() > 1)
						val = ent[1];
					if(ent.size() > 0)
						name = ent[0];
					else
						continue;
					values[name] = val;
				}
			}
			statusLine = true;
		}
	}


	if(header.find(std::string("sec-websocket-key")) == header.end())
	{
		std::string badReq = "HTTP/1.1 400 Bad Request\r\n\r\n"
				"<h1>Bad Request</h1>"
				"This is WebSocket server.\r\n";
		::write(this->fd, badReq.c_str(), badReq.length());
		throw std::string("Header 'sec-websocket-key' is missing. No way to do handshake.");
	}
	std::string responseKeyString = header["sec-websocket-key"] + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::vector<unsigned char> responseKey;
	responseKey.resize(SHA1_DIGEST_SIZE);
	sha1_buffer(responseKeyString.c_str(), responseKeyString.length(), responseKey.data());
	char *acceptKey;
	base64_encode_alloc((const char*)responseKey.data(), responseKey.size(), &acceptKey);
	std::string acceptKeyString = acceptKey;

	std::string responseHeader;
	responseHeader += "HTTP/1.1 101 Switching Protocols\r\n";
	responseHeader += "Upgrade: websocket\r\n";
	responseHeader += "Connection: Upgrade\r\n";
	responseHeader += "Sec-WebSocket-Accept: " + acceptKeyString + "\r\n";
	responseHeader += "\r\n";
	free(acceptKey);

	rwSize = ::write(this->fd, responseHeader.c_str(), responseHeader.length());
	if(rwSize < 0)
		throw errno;
	else if(rwSize != (ssize_t)responseHeader.length())
	{
		responseHeader.erase(responseHeader.begin(), responseHeader.begin() + rwSize);
		this->handshakeStash = responseHeader;
		this->wasHandshaking = true;
		rwSize *= -1;
		throw rwSize;
	}

	this->handshook = true;
	this->wasHandshaking = false;
	this->values.swap(values);
	this->header.swap(header);
	this->handshakeStash.clear();
	return *this;
}

unsigned short WebSocket::getPeerPort() const noexcept
{
	socklen_t len;
	struct sockaddr_storage addr;
	::getpeername(this->fd, (struct sockaddr*)&addr, &len);
	struct sockaddr_in *v4 = (struct sockaddr_in*)&addr;
	return ntohs(v4->sin_port);
}

size_t WebSocket::getHeaderLimit() const noexcept
{
	return this->headerLimit;
}

WebSocket& WebSocket::setHeaderLimit(const size_t limit) noexcept
{
	this->headerLimit = limit;
	return *this;
}

size_t WebSocket::getHeaderLineLimit() const noexcept
{
	return this->headerLineLimit;
}

WebSocket& WebSocket::setHeaderLineLimit(const size_t limit) noexcept
{
	this->headerLineLimit = limit;
	return *this;
}

size_t WebSocket::getBufferSize() const noexcept
{
	return this->bufferSize;
}

WebSocket& WebSocket::setBufferSize(const size_t size) noexcept
{
	int tmp = (int)size;
	socklen_t tmp1 = sizeof(int);
	if(::setsockopt(this->fd, SOL_SOCKET, SO_RCVBUF, &tmp, sizeof(int)) >= 0)
	{
		if(::getsockopt(this->fd, SOL_SOCKET, SO_RCVBUF, &tmp, &tmp1) >= 0)
			this->bufferSize = tmp;
	}
	return *this;
}

void WebSocket::__clearStash() noexcept
{
	for(auto&v : this->stash)
		v.clear();
	this->handshakeStash.clear();
	this->remainingRead = 0;
	this->buffer.clear();
	this->wasHandshaking = false;
}

WebSocket& WebSocket::setNonBlock(const bool nonBlock) throw (int)
{
	int flags = ::fcntl(this->fd, F_GETFL, 0);
	if(flags < 0)
		throw errno;
	else if(nonBlock)
		flags |= O_NONBLOCK;
	else
		flags &= O_NONBLOCK;

	if(::fcntl(this->fd, F_SETFL, flags) < 0)
		throw errno;
	return *this;
}

bool WebSocket::isNonBlock() const throw (int)
{
	const int flags = ::fcntl(this->fd, F_GETFL, 0);
	if(flags < 0)
		throw errno;
	return (flags & O_NONBLOCK) != 0;
}

std::string WebSocket::toLower__(const std::string& x) noexcept
{
	std::string y = x;
	for(auto &v : y)
	{
		if('A' <= v && v <= 'Z')
			v += 32;
	}

	return y;
}

WebSocket& WebSocket::setFrameSizeLimit(const size_t size) noexcept
{
	this->frameSizeLimit = size;
	return *this;
}

size_t WebSocket::getFrameSizeLimit() const noexcept
{
	return this->frameSizeLimit;
}

std::map<std::string, std::string> WebSocket::getHeader() throw (std::string)
{
	return this->header;
}

std::string WebSocket::toUpper__(const std::string& x) noexcept
{
	std::string y = x;
	for(auto &v : y)
	{
		if('a' <= v && v <= 'z')
			v -= 32;
	}

	return y;
}

WebSocket& WebSocket::setReuseAddress(const bool reuse) throw(int)
{
	this->reuseAddr = reuse;
	return *this;
}

bool WebSocket::isReuseAddress() throw(int)
{
	return this->reuseAddr;
}

WebSocket& WebSocket::shutdown(const bool input, const bool output) throw (int)
{
	int how = -1;
	if(input && output)
		how = SHUT_RDWR;
	else if(input)
		how = SHUT_RD;
	else if(output)
		how = SHUT_WR;

	if(how >= 0)
	{
		if(::shutdown(this->fd, how) < 0)
			throw errno;
	}
	return *this;
}

WebSocket& WebSocket::shutdownBoth() throw (int)
{
	this->shutdown(true, true);
	return *this;
}

std::map<std::string, std::string> WebSocket::getRequestedValues() noexcept
{
	return this->values;
}

std::string WebSocket::getRequestedUrl() noexcept
{
	return this->requestedUrl;
}

std::string WebSocket::trim__(const std::string& x) noexcept
{
	std::string y = x;

	while(y.begin() != y.end())
	{
		if(std::isspace(*y.begin()))
			y.erase(y.begin());
		else
			break;
	}
	while(y.begin() != y.end() && y.end() - 1 != y.end())
	{
		if(std::isspace(*(y.end()-1)))
			y.erase(y.end()-1);
		else
			break;
	}

	return y;
}

std::vector<std::string> WebSocket::explodeSpace__(const std::string& x) noexcept
{
	std::string arg = x;
	std::vector<std::string> y;
	std::string::size_type i, cnt;
	while(true)
	{
		arg = thisClass::trim__(arg);
		cnt = arg.length();
		for(i=0; i<cnt; ++i)
		{
			if(std::isspace(arg[i]))
			{
				y.push_back(arg.substr(0, i));
				arg.erase(arg.begin(), arg.begin()+i);
				break;
			}
		}
		if(i >= cnt)
		{
			if(! arg.empty())
				y.push_back(arg);
			break;
		}
	}
	return y;
}

std::vector<std::string> WebSocket::explode__(const std::string& x, const std::set<std::string>& delimiters) noexcept
{
	std::string arg = x;
	std::vector<std::string> y;
	std::string::size_type i, cnt;

	auto trimDelimiters = [&arg, &delimiters]()
	{
		while(arg.begin() != arg.end())
		{
			if(delimiters.find(std::string(1, *arg.begin())) != delimiters.end())
				arg.erase(arg.begin());
			else
				break;
		}
		while(arg.begin() != arg.end() && arg.end() - 1 != arg.end())
		{
			if(delimiters.find(std::string(1, *(arg.end() - 1))) != delimiters.end())
				arg.erase(arg.end()-1);
			else
				break;
		}
	};

	while(true)
	{
		trimDelimiters();
		cnt = arg.length();
		for(i=0; i<cnt; ++i)
		{
			if(delimiters.find(std::string(1, arg[i])) != delimiters.end())
			{
				y.push_back(arg.substr(0, i));
				arg.erase(arg.begin(), arg.begin()+i);
				break;
			}
		}
		if(i >= cnt)
		{
			if(! arg.empty())
				y.push_back(arg);
			break;
		}
	}
	return y;
}

void WebSocket::__clearRequest() noexcept
{
	this->handshook = false;
	this->requestedUrl.clear();
	this->header.clear();
	this->values.clear();
}

}

namespace ashe
{
/* WebSocket::Frame implementation
 *
 */
std::vector<unsigned char> WebSocket::Frame::__maskPayload() noexcept
{
	std::vector<unsigned char> y = this->payload;
	std::vector<unsigned char>::size_type i = 0;
	for(auto &v : y)
		v ^= this->mask[(i++)%4];
	return y;
}

WebSocket::Frame::Frame() noexcept
{
}

WebSocket::Frame::Frame(std::vector<unsigned char>& v, const size_t frameSizeLimit) throw (ssize_t, std::string)
{
	ssize_t req = 0, pos = 0;
	if(v.size() < 2)
	{
		req = 2 - v.size();
		throw req;
	}
	this->firstBytes = *((unsigned char*)v.data()) << 8;
	this->firstBytes |= *((unsigned char*)v.data() + 1);
	if(this->rsv(0) || this->rsv(1) || this->rsv(2))
		throw std::string("Protocol error.");
	pos += 2;
	if(this->hasMask())
		req = 4;

	size_t payloadSize;
	switch(this->firstBytes & 0b0000000001111111)
	{
	case 126:
		if((ssize_t)v.size() < 2+pos)
		{
			req += 2;
			throw req;
		}
		payloadSize = (size_t)*((unsigned char*)(v.data() + pos)) << 8;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 1));
		pos += 2;
		break;
	case 127:
		if((ssize_t)v.size() < 8+pos)
		{
			req += 8;
			throw req;
		}
		payloadSize = (size_t)*((unsigned char*)(v.data() + pos)) << 56;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 1)) << 48;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 2)) << 40;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 3)) << 32;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 4)) << 24;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 5)) << 16;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 6)) << 8;
		payloadSize |= (size_t)*((unsigned char*)(v.data() + pos + 7));
		pos += 8;
		break;
	default:
		payloadSize = (size_t)(this->firstBytes & 0b0000000001111111);
	}
	if(payloadSize + this->getFrameSize() > frameSizeLimit)
	{
		req = payloadSize + this->getFrameSize();
		throw req;
	}

	unsigned char *p;
	if(v.size() < this->getFrameSize() + payloadSize)
	{
		req =  payloadSize + this->getFrameSize() - v.size();
		req *= -1;
		throw req;
	}
	else if(this->hasMask())
	{
		p = v.data() + pos;
		pos += 4;
		this->mask.push_back(*(p++));
		this->mask.push_back(*(p++));
		this->mask.push_back(*(p++));
		this->mask.push_back(*(p++));
	}

	p = v.data() + pos;
	pos += payloadSize;
	std::vector<unsigned char> payload(p, p+payloadSize);
	this->payload.swap(payload);
	if(this->hasMask())
		this->__maskPayload().swap(this->payload);
	v.erase(v.begin(), v.begin() + pos);
}

bool WebSocket::Frame::fin() const noexcept
{
	return (this->firstBytes & 0b1000000000000000) != 0;
}

WebSocket::Frame& WebSocket::Frame::setFin(const bool fin) noexcept
{
	if(fin)
		this->firstBytes |= 0b1000000000000000;
	else
		this->firstBytes &= 0b0111111111111111;
	return *this;
}

bool WebSocket::Frame::rsv(const unsigned char idx) const noexcept
{
	assert(idx < 3);
	return (this->firstBytes & (0b0001000000000000 << idx)) != 0;
}

WebSocket::Frame::OPCode WebSocket::Frame::opcode() const noexcept
{
	return (OPCode)((this->firstBytes & 0b0000111100000000) >> 8);
}

WebSocket::Frame& WebSocket::Frame::setOpcode(const OPCode opcode) noexcept
{
	this->firstBytes &= 0b1111000011111111;
	this->firstBytes |= (unsigned short)opcode << 8;
	return *this;
}

bool WebSocket::Frame::hasMask() const noexcept
{
	return (this->firstBytes & 0b0000000010000000) != 0;
}

WebSocket::Frame& WebSocket::Frame::setMask(const unsigned int mask) noexcept
{
	this->mask.clear();
	this->mask.push_back((unsigned char)(mask & 0xFF000000) >> 24);
	this->mask.push_back((unsigned char)(mask & 0x00FF0000) >> 16);
	this->mask.push_back((unsigned char)(mask & 0x0000FF00) >> 8);
	this->mask.push_back((unsigned char)(mask & 0x000000FF));
	this->firstBytes |= 0b0000000010000000;
	return *this;
}

WebSocket::Frame& WebSocket::Frame::clearMask() noexcept
{
	this->mask.clear();
	this->firstBytes &= 0b1111111101111111;
	return *this;
}

void WebSocket::Frame::__construct(const thisClass& src) noexcept
{
	::memcpy(&this->firstBytes, &src.firstBytes, sizeof(unsigned short));
	if(src.hasMask())
		this->setMask(src.mask);
	else
		this->clearMask();
	this->payload = src.payload;
	this->__updatePayloadSize();
}

WebSocket::Frame::Frame(const thisClass& src) noexcept
{
	this->__construct(src);
}

WebSocket::Frame& WebSocket::Frame::operator =(const thisClass& src) noexcept
{
	this->__construct(src);
	return *this;
}

std::vector<unsigned char> WebSocket::Frame::getPayload() const noexcept
{
	return this->payload;
}

size_t WebSocket::Frame::getPayloadSize() const noexcept
{
	return (size_t)this->payload.size();
}

WebSocket::Frame& WebSocket::Frame::setPayload(const std::vector<unsigned char>& x) noexcept
{
	this->payload = x;
	this->__updatePayloadSize();
	return *this;
}

WebSocket::Frame& WebSocket::Frame::setPayload(const std::string& x) noexcept
{
	std::vector<unsigned char> xx((unsigned char*)x.data(), (unsigned char*)x.data() + x.length());
	this->swapPayload(xx);
	return *this;
}

WebSocket::Frame& WebSocket::Frame::swapPayload(std::vector<unsigned char>& x) noexcept
{
	this->payload.swap(x);
	this->__updatePayloadSize();
	return *this;
}

void WebSocket::Frame::__updatePayloadSize() noexcept
{
	const auto size = this->payload.size();
	if(size > 65535)
		this->firstBytes |= 127;
	else if(size > 125)
		this->firstBytes |= 126;
	else
		this->firstBytes |= (unsigned short)size;
}

size_t WebSocket::Frame::getFrameSize() const noexcept
{
	size_t extSize;
	switch(this->firstBytes & 0b0000000001111111)
	{
	case 126: extSize = 2; break;
	case 127: extSize = 8; break;
	default: extSize = 0;
	}
	if(this->hasMask())
		extSize += 4;

	return 2 + extSize + this->getPayloadSize();
}

std::vector<unsigned char> WebSocket::Frame::toBinary() const noexcept
{
	std::vector<unsigned char> y;
	unsigned char *p;
	p = ((unsigned char*)&this->firstBytes) + 1;
	y.push_back(*(p--));
	y.push_back(*(p--));
	switch(this->firstBytes & 0b0000000001111111)
	{
	case 126:
	{
		unsigned short size = (unsigned short)this->payload.size();
		p = ((unsigned char*)&size) + 1;
		y.push_back(*(p--));
		y.push_back(*(p--));
	}
	break;
	case 127:
	{
		unsigned long long size = (unsigned long long)this->payload.size();
		p = ((unsigned char*)&size) + 7;
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
		y.push_back(*(p--));
	}
	break;
	}

	if(this->hasMask())
		y.insert(y.end(), this->mask.begin(), this->mask.end());
	y.insert(y.end(), this->payload.begin(), this->payload.end());

	return y;
}

WebSocket::Frame& WebSocket::Frame::clearPayload() noexcept
{
	std::vector<unsigned char> ny;
	this->payload.swap(ny);
	this->__updatePayloadSize();
	return *this;
}

WebSocket::Frame& WebSocket::Frame::setMask(const std::vector<unsigned char>& mask) noexcept
{
	this->mask = mask;
	this->mask.resize(4);
	this->firstBytes |= 0b0000000010000000;
	return *this;
}

}
