/*
 * DescendantProcess.cpp
 *
 *  Created on: 2014. 6. 12.
 *      Author: virtue
 */

#include "DescendantProcess.h"

#include <sstream>

namespace ashe
{

DescendantProcess::__PIDMapType DescendantProcess::__pidMap__;

int DescendantProcess::main()
{
	return 0;
}

DescendantProcess::DescendantProcess() noexcept
{
	this->className = "ashe::DescendantProcess";
}

DescendantProcess::DescendantProcess(const thisClass& src) throw (WeakRune)
		: motherClass(src)
{
	if(src.detached || src.pid < 0)
		this->__construct(src);
	else
	{
		WeakRune e("It is not nice to copy a forked process!");
		throw e;
	}
	this->className = "ashe::DescendantProcess";
}

DescendantProcess::~DescendantProcess() noexcept
{

	try
	{
		if(! (this->daemonic || this->detached))
			this->join();
		this->setDaemonic(false);
	}
	catch(...){}
}

int DescendantProcess::join(const bool wait) throw (WeakRune)
{
	int y;
	pid_t fRet = ::waitpid(this->pid, &y, (wait? 0 : WNOHANG));
	if(fRet < 0)
	{
		WeakRune e("Whilst ::waitpid()");
		throw e.setError();
	}
	else if(! fRet)
	{
		WeakRune e("Process has not returned yet.");
		throw e;
	}

	this->pid = -1;
	return y;
}

DescendantProcess::thisClass& DescendantProcess::kill(const int num) throw (WeakRune)
{
	if(::kill(this->pid, num) < 0)
	{
		WeakRune e("Whilst ::kill()");
		throw e.setError();
	}
	else
		this->pid = -1;

	return *this;
}

DescendantProcess::thisClass& DescendantProcess::operator =(const thisClass& src) throw (WeakRune)
{
	if(src.detached || src.pid < 0)
		this->__construct(src);
	else
	{
		WeakRune e("It is not nice to copy a forked process!");
		throw e;
	}
	this->className = "ashe::DescendantProcess";
	return *this;
}

void DescendantProcess::__construct(const thisClass& src) noexcept
{
	this->pid = src.pid;
	this->daemonic = src.daemonic;
	this->detached = false;
}

DescendantProcess::thisClass& DescendantProcess::start() throw (WeakRune)
{
	if(this->pid >= 0 && (!this->detached))
	{
		WeakRune e("Invalid action. The process is already ran.");
		throw e;
	}

	this->pid = ::fork();
	if(this->pid < 0)
	{
		WeakRune e("Whilst ::fork()");
		throw e.setError();
	}
	else if(this->pid)
		this->setDaemonic(this->daemonic);
	else
		::exit(this->main());

	return *this;
}

pid_t DescendantProcess::getPID() const noexcept
{
	return this->pid;
}

DescendantProcess::thisClass& DescendantProcess::detach() noexcept
{
	this->detached = true;
	return *this;
}

bool DescendantProcess::isDaemonic() const noexcept
{
	return this->daemonic;
}

DescendantProcess::JoinedProcess DescendantProcess::join__() throw(WeakRune)
{
	JoinedProcess y;
	if((y.pid = ::wait(&y.status)) < 0)
	{
		WeakRune e("Whilst ::wait()");
		throw e.setError();
	}

	thisClass::__PIDMapType::iterator it = thisClass::__pidMap__.find(y.pid);
	if(it == thisClass::__pidMap__.end())
		y.instance = NULL;
	else
	{
		y.instance = it->second;
		y.instance->pid = -1;
	}

	return y;
}

DescendantProcess::thisClass& DescendantProcess::setDaemonic(const bool daemonic)  throw(WeakRune)
{
	if(this->detached || this->pid <= 0)
	{
		WeakRune e("Invalid action: Instance is detached or not running.");
		throw e;
	}

	if((this->daemonic = daemonic))
		thisClass::__pidMap__[this->pid] = this;
	else
		thisClass::__pidMap__.erase(this->pid);

	return *this;
}

std::string DescendantProcess::toString() const noexcept
{
	std::stringstream sb;
	sb << motherClass::toString();
	if(this->pid > 0)
		sb << " pid: " << this->pid;
	if(this->detached)
		sb << " (Detached)";

	return sb.str();
}

} /* namespace ashe */
