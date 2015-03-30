#include "DescendantPipe.h"

#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

namespace ashe
{

DescendantPipe::DescendantPipe(const bool blocking) throw (WeakRune)
{
	this->className = "ashe::DescendantPipe";
	if(::pipe(this->pipe) < 0)
	{
		WeakRune e("Whilst ::pipe()");
		throw e.setError();
	}
	this->blocking(blocking);
}

DescendantPipe::DescendantPipe(const thisClass& src) throw (WeakRune)
		: motherClass(src)
{
	this->pipe[0] = this->pipe[1] = -1;
	this->__construct(src);
	this->className = "ashe::DescendantPipe";
}

DescendantPipe::~DescendantPipe() noexcept
{
	if(! this->detached)
		this->__close();
}

DescendantPipe::thisClass& DescendantPipe::operator =(const thisClass& src) throw (WeakRune)
{
	motherClass::__construct(src);
	this->__construct(src);
	this->className = "ashe::DescendantPipe";
	return *this;
}

void DescendantPipe::__construct(const thisClass &src) throw(WeakRune)
{
	this->__close();
	if(src.detached)
		::memcpy(this->pipe, src.pipe, sizeof(int) * 2);
	else
	{
		int i = ::dup(src.pipe[0]);
		if(i < 0)
		{
			WeakRune e("Whilst ::dup() input fd");
			e.setError();
			throw e;
		}
		int o = ::dup(src.pipe[1]);
		if(o < 0)
		{
			WeakRune e("Whilst ::dup() output fd");
			e.setError();
			::close(i);
			throw e;
		}

		this->pipe[0] = i;
		this->pipe[1] = o;
		this->blocking(src.blocking());
		this->bufferSize(src.bufferSize());
		this->__stash = src.__stash;
	}
	this->stateBits = src.stateBits;
}

void DescendantPipe::__close() noexcept
{
	if(this->__isClosed())
		return;
	::close(this->pipe[0]);
	::close(this->pipe[1]);
	this->pipe[0] = this->pipe[1] = -1;
	this->__setStateBits(fatherClass::SB_CLOSED, true);
	this->__setStateBits(fatherClass::SB_ENDED, true);
	this->detached = false;
	this->__stash.clear();
	this->lastRetrievedSize = this->lastSentSize = 0;
}

bool DescendantPipe::__isClosed() const noexcept
{
	return this->pipe[0] < 0 && this->pipe[1] < 0;
}

void DescendantPipe::__checkValidity() const throw (WeakRune)
{
	if(this->__isClosed())
	{
		WeakRune e("Instance no longer valid: pipes are closed");
		throw e;
	}
	else if(this->hasDetached())
	{
		WeakRune e("Instance no longer valid: Instance has detached");
		throw e;
	}
}

bool DescendantPipe::blocking() const throw(WeakRune)
{
	const int opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
	{
		WeakRune e("Whilst getting flag of the pipe.");
		throw e.setError();
	}

	return (opt & O_NONBLOCK) != 0;
}

DescendantPipe::thisClass& DescendantPipe::blocking(const bool blocking) throw(WeakRune)
{
	int opt;

	opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
		throw WeakRune("Whilst getting flag of the input pipe.");
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[0], F_SETFL, opt) < 0)
		throw WeakRune("Whilst setting flag of the pipe.");

	opt = ::fcntl(this->pipe[1], F_GETFL);
	if(opt < 0)
		throw WeakRune("Whilst getting flag of the output pipe.");
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[1], F_SETFL, opt) < 0)
		throw WeakRune("Whilst setting flag of the pipe.");

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::post(const void* data, const size_t len) throw (WeakRune)
{
	const std::vector<unsigned char> x((unsigned char*)data, (unsigned char*)data + len);
	return this->post(x);
}

DescendantPipe::thisClass& DescendantPipe::post(const std::vector<unsigned char>& data) throw (WeakRune)
{
	return this->post(data, data.size());
}

DescendantPipe::thisClass& DescendantPipe::post(const std::vector<unsigned char>& data, const size_t len) throw (WeakRune)
{
	if(data.empty())
		throw WeakRune("No data given for post()");
	else if(data.size() < len)
		throw WeakRune("'len' is out of the range of 'data'");
	auto it = data.begin();
	this->__stash.insert(this->__stash.end(), it, it + len);

	return this->post();
}

DescendantPipe::thisClass& DescendantPipe::post() throw (WeakRune)
{
	this->__checkValidity();
	if(! this->__stash.empty())
	{
		const auto rwSize = ::write(this->pipe[1], this->__stash.data(), this->__stash.size());
		if(rwSize <= 0)
		{
			std::string msg;
			switch(errno)
			{
			case EAGAIN:
				msg = "Data transfer delayed.";
				this->__setStateBits(fatherClass::SB_DELAYED, true);
				break;
			case EBADF:
			case EBADFD:
			case EPIPE:
			case EINVAL:
			case EFAULT:
				this->__close();
				msg = "The pipe is no longer valid.";
				break;
			default:
				msg = "Could not transfer the data.";
			}
			this->__setStateBits(fatherClass::SB_FAILED, true);

			WeakRune e(msg);
			throw e.setError();
		}

		auto it = this->__stash.begin();
		this->__stash.erase(it, it + rwSize);
		this->__accumilateSentSize((size_t)rwSize);
		if((size_t)rwSize < this->__stash.size())
		{
			WeakRune e("Data transfer delayed.");
			this->__setStateBits(fatherClass::SB_DELAYED, true);
			throw e.setError();
		}
		this->__setStateBits(fatherClass::SB_GOOD, true);
	}

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::receive(void* data, const size_t len) throw (WeakRune)
{
	this->__checkValidity();
	if(! len)
		throw WeakRune("Given data range is empty.");
	const auto rwSize = ::read(this->pipe[0], data, len);

	if(rwSize < 0)
	{
		std::string msg;
		switch(errno)
		{
		case EAGAIN:
			msg = "No data to read in the pipe.";
			break;
		case EBADF:
		case EINVAL:
			this->__close();
			msg = "The pipe is no longer valid.";
			break;
		default:
			msg = "Could not read data from the pipe.";
		}
		this->__setStateBits(fatherClass::SB_FAILED, true);

		WeakRune e(msg);
		throw e.setError();
	}
	this->__accumilateRetrievedSize((size_t)rwSize);
	this->__setStateBits(fatherClass::SB_GOOD, true);

	return *this;
}

DescendantPipe::thisClass& DescendantPipe::receive(std::vector<unsigned char>& data) throw (WeakRune)
{
	return this->receive(data.data(), data.size());
}

DescendantPipe::thisClass& DescendantPipe::receive(std::vector<unsigned char>& data, const size_t len) throw (WeakRune)
{
	if(! len)
		throw WeakRune("No data range givne for receive()");
	else if(data.size() < len)
		throw WeakRune("'len' is out of the range of 'data'");
	return this->receive(data.data(), len);
}

DescendantPipe::thisClass& DescendantPipe::bufferSize(const size_t size) throw (WeakRune)
{
	if(::fcntl(this->pipe[0], F_SETPIPE_SZ, (int)size) < 0)
	{
		WeakRune e("Failed to change buffer size of the input pipe.");
		throw e.setError();
	}
	return *this;
}

size_t DescendantPipe::bufferSize() const throw(WeakRune)
{
	// buffer size of pipe[0] and pipe[1] are the same (Representative fd)
	const int y = ::fcntl(this->pipe[0], F_GETPIPE_SZ);
	if(y < 0)
	{
		WeakRune e("Could not get buffer size of the pipe.");
		throw e.setError();
	}
	return (size_t)y;
}

bool DescendantPipe::valid() const noexcept
{
	return this->pipe[0] >= 0 && this->pipe[1] >= 0;
}

DescendantPipe::thisClass& DescendantPipe::detach() throw (WeakRune)
{
	if(! this->valid())
		throw WeakRune("The instance is not valid.");
	else if(this->detached)
		throw WeakRune("The instance is aleady detached.");

	this->detached = true;
	return *this;
}

std::string DescendantPipe::toString() const noexcept
{
	std::stringstream sb;
	sb << motherClass::toString() << ' ';
	if(this->pipe[0] >= 0 && this->pipe[1] >= 0)
		sb << "(i, o): " << this->pipe[0] << ", " << this->pipe[1] << ", ";

	try
	{
		sb << (this->blocking()? "Blocking" : "Non-blocking") << ", ";
	}
	catch(...){}
	try
	{
		sb << "Buffer size: " << this->bufferSize() << 'b';
	}
	catch(...){}
	if(this->detached)
		sb << " (Detached)";

	return sb.str();
}

DescendantPipe::thisClass& DescendantPipe::descriptors(std::set<int>& y) throw (WeakRune)
{
	y.clear();
	y.insert(this->pipe[0]);
	y.insert(this->pipe[1]);

	return *this;
}

}
