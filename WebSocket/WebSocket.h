/*
 * WebSocket.h
 *
 * C++11, POSIX WebSocket implementation by Ashe
 *
 * @Refer
 *  - https://tools.ietf.org/html/rfc6455
 *  - https://developer.mozilla.org/en-US/docs/WebSockets/Writing_WebSocket_servers
 *  - http://www.websocket.org/echo.html
 * @Maintained
 *  Under development since: 2015 Q1
 * @Author
 *  Ashe David Sterkhus
 *  Blame to: ashe.goulding+blame@gmail.com
 * @COLOPHON
 *  This file is part of libashe, Ashe's C++11/98 utility stuff
 */
#ifndef ASHE_WEBSOCKET_H_
#define ASHE_WEBSOCKET_H_

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <regex>
#include <map>
#include <set>

#include "Frelish.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace ashe
{

class WebSocket : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef WebSocket thisClass;
	const static int implementedVersion = 13;

	class Frame : public Icebank
	{
	public:
		typedef Icebank motherClass;
		typedef Frame thisClass;
		enum OPCode
		{
			OP_CONTINUATION = 0x00,
			OP_TEXT = 0x01,
			OP_BINARY = 0x02,
			OP_CONNECTION_CLOSE = 0x08,
			OP_PING = 0x09,
			OP_PONG = 0x0A
		};

	protected:
		unsigned short firstBytes = 0;
		std::vector<unsigned char> mask;
		std::vector<unsigned char> payload;

		void __construct(const thisClass &src) noexcept;
		void __updatePayloadSize() noexcept;
		virtual std::vector<unsigned char> __maskPayload() const noexcept;

	public:
		Frame() noexcept;
		Frame(const thisClass &src) noexcept;
		Frame(std::vector<unsigned char> &v, const size_t frameSizeLimit) throw(ssize_t, std::string);
		virtual ~Frame() noexcept;

		Frame &operator =(const thisClass &src) noexcept;

		virtual bool fin() const noexcept;
		virtual Frame &setFin(const bool fin) noexcept;
		virtual bool rsv(const unsigned char idx) const noexcept;
		virtual OPCode opcode() const noexcept;
		virtual Frame &setOpcode(const OPCode opcode) noexcept;
		virtual bool hasMask() const noexcept;
		virtual Frame &setMask(const unsigned int mask) noexcept;
		virtual Frame &setMask(const std::vector<unsigned char> &mask) noexcept;
		virtual Frame &clearMask() noexcept;
		virtual std::vector<unsigned char> getPayload() const noexcept;
		virtual size_t getPayloadSize() const noexcept;
		virtual Frame &setPayload(const std::vector<unsigned char> &x) noexcept;
		virtual Frame &setPayload(const std::string &x) noexcept;
		virtual Frame &swapPayload(std::vector<unsigned char> &x) noexcept;
		virtual Frame &clearPayload() noexcept;
		virtual size_t getFrameSize() const noexcept;

		virtual std::vector<unsigned char> toBinary() const noexcept;
	};

protected:
	std::vector<unsigned char> stash[2];
	size_t remainingRead = 0;
	std::string handshakeStash;
	bool wasHandshaking = false, handshook = false;

	std::string requestedUrl;
	std::map<std::string, std::string> header;
	std::map<std::string, std::string> values;

	size_t bufferSize = 4096;
	std::vector<unsigned char> buffer;

	size_t headerLimit = 1024; // Chars of header
	size_t headerLineLimit = 256; // Chars of a header line

	size_t frameSizeLimit = 1048576;/*10MiB*/

	bool reuseAddr = false, closeOnDelete = true;

	void __construct(const thisClass &src) noexcept;
	void __clearStash() noexcept;
	void __clearRequest() noexcept;

public:
	int fd = -1;

	WebSocket() noexcept;
	WebSocket(const WebSocket &src) noexcept;
	virtual ~WebSocket() noexcept;

	virtual WebSocket &operator =(const WebSocket &src) noexcept;

	virtual WebSocket &bind(const unsigned short port, const std::string address = "0.0.0.0") throw(int);
	virtual WebSocket &listen(const int backlog = 10) throw(int);
	virtual WebSocket accept() throw(int);
	virtual WebSocket &handshake() throw(std::string, int, ssize_t);
	virtual WebSocket &close(const bool urgently = false) noexcept;
	virtual WebSocket &shutdown(const bool input, const bool output) throw(int);
	virtual WebSocket &shutdownBoth() throw(int);

	virtual std::map<std::string, std::string> getHeader() throw(std::string);
	virtual std::map<std::string, std::string> getRequestedValues() noexcept;
	virtual std::string getRequestedUrl() noexcept;

	virtual Frame receive() throw(ssize_t, int, std::string);
	virtual WebSocket &post(const Frame &frame) throw(int, ssize_t);
	virtual WebSocket &post() throw(int, ssize_t);

	virtual WebSocket &setNonBlock(const bool nonBlock = true) throw(int);
	virtual bool isNonBlock() const throw(int);
	virtual WebSocket &setReuseAddress(const bool reuse) throw(int);
	virtual bool isReuseAddress() throw(int);
	virtual std::string getPeerAddressString() const noexcept;
	virtual std::vector<unsigned char> getPeerAddress() const noexcept;
	virtual unsigned short getBindPort() const noexcept;
	virtual unsigned short getPeerPort() const noexcept;

	virtual size_t getHeaderLimit() const noexcept;
	virtual WebSocket &setHeaderLimit(const size_t limit) noexcept;
	virtual size_t getHeaderLineLimit() const noexcept;
	virtual WebSocket &setHeaderLineLimit(const size_t limit) noexcept;
	virtual size_t getBufferSize() const noexcept;
	virtual WebSocket &setBufferSize(const size_t size) noexcept;
	virtual WebSocket &setFrameSizeLimit(const size_t size) noexcept;
	virtual size_t getFrameSizeLimit() const noexcept;

	static std::string trim__(const std::string &x) noexcept;
	static std::vector<std::string> explodeSpace__(const std::string &x) noexcept;
	static std::vector<std::string> explode__(const std::string &x, const std::set<std::string> &delimiters) noexcept;
	static std::string toLower__(const std::string &x) noexcept;
	static std::string toUpper__(const std::string &x) noexcept;
};

} /* namespace ashe */

#endif /* WEBSOCKET_H_ */
