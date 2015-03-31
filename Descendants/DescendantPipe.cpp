#include "DescendantPipe.h"

#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

namespace ashe
{

DescendantPipe::DescendantPipe(const bool blocking) throw (TransitiveRune)
{
	this->className = "ashe::DescendantPipe";
	if(::pipe(this->pipe) < 0)
	{
		TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Whilst ::pipe()");
		throw e.setError();
	}
	this->blocking(blocking);
}

DescendantPipe::DescendantPipe(const thisClass& src) throw (TransitiveRune)
		: motherClass(src)
{
	this->pipe[0] = this->pipe[1] = -1;
	this->__construct(src);
	this->className = "ashe::DescendantPipe";
}

DescendantPipe::~DescendantPipe() noexcept
{
	if(! this->__detached)
		this->__close();
}

DescendantPipe::thisClass& DescendantPipe::operator =(const thisClass& src) throw (TransitiveRune)
{
	motherClass::__construct(src);
	this->__construct(src);
	this->className = "ashe::DescendantPipe";
	return *this;
}

void DescendantPipe::__construct(const thisClass &src) throw(TransitiveRune)
{
	if(src.__detached)
	{
		this->__close();
		::memcpy(this->pipe, src.pipe, sizeof(int) * 2);
	}
	else
	{
		int i = ::dup(src.pipe[0]);
		if(i < 0)
		{
			TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Whilst ::dup() input fd");
			e.setError();
			throw e;
		}
		int o = ::dup(src.pipe[1]);
		if(o < 0)
		{
			TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Whilst ::dup() output fd");
			e.setError();
			::close(i);
			throw e;
		}

		this->__close();

		this->pipe[0] = i;
		this->pipe[1] = o;
		this->blocking(src.blocking());
		this->bufferSize(src.bufferSize());
	}
	this->__stash = src.__stash;
	this->stateBits = src.stateBits;
	this->__detached = false;
}

void DescendantPipe::__close() noexcept
{
	if(this->__detached || this->__isClosed())
		return;

	::close(this->pipe[0]);
	::close(this->pipe[1]);
	this->pipe[0] = this->pipe[1] = -1;
	this->__setStateBits(fatherClass::SB_CLOSED, true);
	this->__setStateBits(fatherClass::SB_ENDED, true);
	this->__detached = false;
	this->__stash.clear();
	this->retrievedSize = this->sentSize = this->lastRetrievedSize = this->lastSentSize = 0;
}

bool DescendantPipe::__isClosed() const noexcept
{
	return this->pipe[0] < 0 && this->pipe[1] < 0;
}

void DescendantPipe::__checkValidity() const throw (TransitiveRune)
{
	if(this->__isClosed())
	{
		TransitiveRune e(TransitiveRune::C_ILLEGAL_STATE, "Instance no longer valid: pipes are closed");
		throw e;
	}
	else if(this->detached())
	{
		TransitiveRune e(TransitiveRune::C_ILLEGAL_STATE, "Instance no longer valid: Instance has detached");
		throw e;
	}
}

bool DescendantPipe::blocking() const throw(TransitiveRune)
{
	const int opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
	{
		TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Whilst getting flag of the pipe.");
		throw e.setError();
	}

	return (opt & O_NONBLOCK) == 0;
}

DescendantPipe::thisClass& DescendantPipe::blocking(const bool blocking) throw(TransitiveRune)
{
	int opt;

	opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
		throw TransitiveRune(TransitiveRune::C_INTERNAL_ERROR, "Whilst getting flag of the input pipe.");
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[0], F_SETFL, opt) < 0)
		throw TransitiveRune(TransitiveRune::C_INTERNAL_ERROR, "Whilst setting flag of the pipe.");

	opt = ::fcntl(this->pipe[1], F_GETFL);
	if(opt < 0)
		throw TransitiveRune(TransitiveRune::C_INTERNAL_ERROR, "Whilst getting flag of the output pipe.");
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[1], F_SETFL, opt) < 0)
		throw TransitiveRune(TransitiveRune::C_INTERNAL_ERROR, "Whilst setting flag of the pipe.");

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::post(const void* data, const size_t len) throw (TransitiveRune)
{
	const std::vector<unsigned char> x((unsigned char*)data, (unsigned char*)data + len);
	return this->post(x);
}

DescendantPipe::thisClass& DescendantPipe::post(const std::vector<unsigned char>& data) throw (TransitiveRune)
{
	return this->post(data, data.size());
}

DescendantPipe::thisClass& DescendantPipe::post(const std::vector<unsigned char>& data, const size_t len) throw (TransitiveRune)
{
	if(data.empty())
		throw TransitiveRune(TransitiveRune::C_NO_DATA_GIVEN, "No data given for post()");
	else if(data.size() < len)
		throw TransitiveRune(TransitiveRune::C_SHORT_LENGTH, "'len' is out of the range of 'data'");
	const auto it = data.begin();
	this->__stash.insert(this->__stash.end(), it, it + len);

	return this->post();
}

DescendantPipe::thisClass& DescendantPipe::post() throw (TransitiveRune)
{
	this->__checkValidity();
	if(! this->__stash.empty())
	{
		const auto rwSize = ::write(this->pipe[1], this->__stash.data(), this->__stash.size());
		if(rwSize <= 0)
		{
			std::string msg;
			TransitiveRune::Code code;
			switch(errno)
			{
			case EAGAIN:
				msg = "Data transfer delayed.";
				code = TransitiveRune::C_DELAYED;
				this->__setStateBits(fatherClass::SB_DELAYED, true);
				break;
			case EBADF:
			case EBADFD:
			case EPIPE:
			case EINVAL:
			case EFAULT:
				this->__close();
				msg = "The pipe is no longer valid.";
				code = TransitiveRune::C_INTERNAL_ERROR;
				break;
			default:
				msg = "Could not transfer the data.";
				code = TransitiveRune::C_INTERNAL_ERROR;
			}
			this->__setStateBits(fatherClass::SB_FAILED, true);

			TransitiveRune e(code, msg);
			throw e.setError();
		}

		auto it = this->__stash.begin();
		this->__stash.erase(it, it + rwSize);
		this->__accumilateSentSize((size_t)rwSize);
		if((size_t)rwSize < this->__stash.size())
		{
			TransitiveRune e(TransitiveRune::C_DELAYED, "Data transfer delayed.");
			this->__setStateBits(fatherClass::SB_DELAYED, true);
			throw e;
		}
		this->__setStateBits(fatherClass::SB_GOOD, true);
	}

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::receive(void* data, const size_t len) throw (TransitiveRune)
{
	this->__checkValidity();
	if(! len)
		throw TransitiveRune(TransitiveRune::C_NO_DATA_GIVEN, "Given data range is empty.");
	const auto rwSize = ::read(this->pipe[0], data, len);

	if(rwSize < 0)
	{
		std::string msg;
		TransitiveRune::Code code;
		switch(errno)
		{
		case EAGAIN:
			msg = "No data to read in the pipe.";
			code = TransitiveRune::C_EMPTY_BUFFER;
			break;
		case EBADF:
		case EINVAL:
			this->__close();
			msg = "The pipe is no longer valid.";
			code = TransitiveRune::C_INTERNAL_ERROR;
			break;
		default:
			msg = "Could not read data from the pipe.";
			code = TransitiveRune::C_INTERNAL_ERROR;
		}
		this->__setStateBits(fatherClass::SB_FAILED, true);

		TransitiveRune e(code, msg);
		throw e.setError();
	}
	this->__accumilateRetrievedSize((size_t)rwSize);
	this->__setStateBits(fatherClass::SB_GOOD, true);

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::receive(std::vector<unsigned char>& data) throw (TransitiveRune)
{
	return this->receive(data.data(), data.size());
}

DescendantPipe::thisClass& DescendantPipe::receive(std::vector<unsigned char>& data, const size_t len) throw (TransitiveRune)
{
	if(! len)
		throw TransitiveRune(TransitiveRune::C_NO_DATA_GIVEN, "No data range given for receive()");
	else if(data.size() < len)
		throw TransitiveRune(TransitiveRune::C_SHORT_LENGTH, "'len' is out of the range of 'data'");
	return this->receive(data.data(), len);
}

DescendantPipe::thisClass& DescendantPipe::bufferSize(const size_t size) throw (TransitiveRune)
{
	if(::fcntl(this->pipe[0], F_SETPIPE_SZ, (int)size) < 0)
	{
		TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Failed to change buffer size of the input pipe.");
		throw e.setError();
	}
	return *this;
}

size_t DescendantPipe::bufferSize() const throw(TransitiveRune)
{
	// buffer size of pipe[0] and pipe[1] are the same (Representative fd)
	const int y = ::fcntl(this->pipe[0], F_GETPIPE_SZ);
	if(y < 0)
	{
		TransitiveRune e(TransitiveRune::C_INTERNAL_ERROR, "Could not get buffer size of the pipe.");
		throw e.setError();
	}
	return (size_t)y;
}

bool DescendantPipe::valid() const noexcept
{
	return this->pipe[0] >= 0 && this->pipe[1] >= 0 && (! this->__detached);
}

DescendantPipe::thisClass& DescendantPipe::detach() throw (TransitiveRune)
{
	if(! this->valid())
		throw TransitiveRune(TransitiveRune::C_ILLEGAL_STATE, "The instance is not valid.");
	else if(this->__detached)
		throw TransitiveRune(TransitiveRune::C_ILLEGAL_STATE, "The instance is already detached.");

	this->__detached = true;
	return *this;
}

std::string DescendantPipe::toString() const noexcept
{
	try
	{
		this->__checkValidity();
	}
	catch(...)
	{
		return motherClass::toString();
	}

	std::stringstream sb;
	sb << motherClass::toString() << ' ';
	if(this->pipe[0] >= 0 && this->pipe[1] >= 0)
		sb << "(i, o): " << this->pipe[0] << ", " << this->pipe[1] << ", ";
	try
	{
		const auto blocking = this->blocking();
		sb << (blocking? "Blocking" : "Non-blocking");
		const auto bufferSize = this->bufferSize();
		sb << ", Buffer size: " << bufferSize << 'b';
	}
	catch(...){}
	sb << ", overall sent/retrieved: " << this->sent(true) << '/' << this->retrieved(true);
	sb << ", last sent/retrieved: " << this->sent() << '/' << this->retrieved();
	if(this->__detached)
		sb << " (Detached)";

	return sb.str();
}

DescendantPipe::thisClass &DescendantPipe::descriptors(std::set<int>& y) throw (TransitiveRune)
{
	this->__checkValidity();
	y.clear();
	y.insert(this->pipe[0]);
	y.insert(this->pipe[1]);

	return *this;
}

}
