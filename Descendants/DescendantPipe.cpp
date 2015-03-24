#include "DescendantPipe.h"

#include <cstring>
#include <sstream>

namespace ashe
{

DescendantPipe::DescendantPipe(const bool blocking) throw (WeakRune, StrongRune)
{
	this->className = "DescendantPipe";
	if(::pipe(this->pipe) < 0)
	{
		WeakRune e("Whilst ::pipe()");
		throw e.setError();
	}
	this->setBlocking(blocking);
	this->__setInstanceBufferSize(this->getBufferSize());
}

DescendantPipe::DescendantPipe(const thisClass& src) throw (WeakRune, StrongRune)
		: motherClass(src)
{
	this->pipe[0] = this->pipe[1] = -1;
	this->__construct(src);
}

DescendantPipe::~DescendantPipe() noexcept
{
	if(! this->detached)
		this->__close();
}

void DescendantPipe::__construct(const thisClass &src) throw(WeakRune, StrongRune)
{
	this->__close();
	if(src.detached)
	{
		::memcpy(this->pipe, src.pipe, sizeof(int) * 2);
		this->__instanceBuffer.resize(this->getBufferSize());
		this->__instanceBuffer.shrink_to_fit();
	}
	else
	{
		int i = ::dup(src.pipe[0]);
		if(i < 0)
		{
			WeakRune e("Whilst ::dup() input fd");
			e.setError();
			::close(i);
			throw e;
		}
		int o = ::dup(src.pipe[1]);
		if(o < 0)
		{
			WeakRune e("Whilst ::dup() output fd");
			e.setError();
			::close(i);
			::close(o);
			throw e;
		}

		this->pipe[0] = i;
		this->pipe[1] = o;
		this->setBlocking(src.isBlocking());
		this->setBufferSize(src.getBufferSize());
	}
	this->detached = false;
}

DescendantPipe::thisClass& DescendantPipe::operator =(const thisClass& src) throw (WeakRune, StrongRune)
{
	motherClass::__construct(src);
	this->__construct(src);
	return *this;
}

DescendantPipe::thisClass& DescendantPipe::detach() noexcept
{
	this->detached = true;
	return *this;
}

size_t DescendantPipe::write(const size_t size, const void* range) throw (WeakRune)
{
	this->__checkValidity();
	const ssize_t rdSize = ::write(this->pipe[1], range, size);
	if(rdSize < 0)
	{
		WeakRune e("Whilst write()");
		throw e.setError();
	}
	return (size_t)rdSize;
}

size_t DescendantPipe::write(const std::vector<unsigned char>& vector) throw (WeakRune)
{
	return thisClass::write((size_t)vector.size(), vector.data());
}

size_t DescendantPipe::read(const size_t size, void* range) throw (WeakRune)
{
	this->__checkValidity();
	const ssize_t rdSize = ::read(this->pipe[0], range, size);
	if(rdSize < 0)
	{
		WeakRune e("Whilst read()");
		throw e.setError();
	}
	return (size_t)rdSize;
}

size_t DescendantPipe::read(std::vector<unsigned char>& vector) throw (WeakRune)
{
	const size_t rdSize = thisClass::read(this->__instanceBuffer.size(), (unsigned char*)this->__instanceBuffer.data());
	if(rdSize)
	{
		std::vector<unsigned char>::iterator it = this->__instanceBuffer.begin();
		vector.insert(vector.end(), it, it + rdSize);
	}

	return (size_t)rdSize;
}

size_t DescendantPipe::getBufferSize() const throw(WeakRune)
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

void DescendantPipe::__close() noexcept
{
	if(this->__isClosed())
		return;
	::close(this->pipe[0]);
	::close(this->pipe[1]);
	this->pipe[0] = this->pipe[1] = -1;
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

void DescendantPipe::__setInstanceBufferSize(const size_t size) throw (StrongRune)
{
	try
	{
		this->__instanceBuffer.resize(size);
		this->__instanceBuffer.shrink_to_fit();
	}
	catch(std::exception &e)
	{
		std::string msg = "Whilst changing instance buffer size: ";
		msg += e.what();
		StrongRune toThrow(msg);
		throw toThrow;
	}
}

DescendantPipe::thisClass& DescendantPipe::setBufferSize(const size_t size) throw (WeakRune, StrongRune)
{
	if(::fcntl(this->pipe[0], F_SETPIPE_SZ, (int)size) < 0)
	{
		WeakRune e("Failed to change buffer size of the input pipe.");
		throw e.setError();
	}

	this->__setInstanceBufferSize(this->getBufferSize());
	return *this;
}

bool DescendantPipe::isBlocking() const throw(WeakRune)
{
	const int opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
	{
		WeakRune e("Whilst getting flag of the pipe.");
		throw e.setError();
	}

	return (opt & O_NONBLOCK) != 0;
}

bool DescendantPipe::hasDetached() const noexcept
{
	return this->detached;
}

DescendantPipe::thisClass& DescendantPipe::setBlocking(const bool blocking) throw(StrongRune)
{
	int opt;

	opt = ::fcntl(this->pipe[0], F_GETFL);
	if(opt < 0)
	{
		StrongRune e("Whilst getting flag of the input pipe.");
		throw e;
	}
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[0], F_SETFL, opt) < 0)
	{
		StrongRune e("Whilst setting flag of the pipe.");
		throw e;
	}

	opt = ::fcntl(this->pipe[1], F_GETFL);
	if(opt < 0)
	{
		StrongRune e("Whilst getting flag of the output pipe.");
		throw e;
	}
	else if(blocking)
		opt &= ~O_NONBLOCK;
	else
		opt |= O_NONBLOCK;
	if(::fcntl(this->pipe[1], F_SETFL, opt) < 0)
	{
		StrongRune e("Whilst setting flag of the pipe.");
		throw e;
	}

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
		sb << (this->isBlocking()? "Blocking" : "Non-blocking") << ", ";
	}
	catch(...){}
	try
	{
		sb << "Buffer size: " << this->getBufferSize() << 'b';
	}
	catch(...){}
	if(this->detached)
		sb << " (Detached)";

	return sb.str();
}

}
