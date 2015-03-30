#include "DescendantProcess.h"

#include <sstream>
#include <cstring>

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
		if(! (this->__daemonic || this->detached))
			this->join();
		this->daemonic(false);
	}
	catch(...){}
}

DescendantProcess::JoinedProcess DescendantProcess::join(const bool wait) throw (WeakRune, StrongRune)
{
	if(this->child())
		throw WeakRune("The instance is child process. Check your code!");
	int rawCode;
	const pid_t fRet = ::waitpid(this->pid, &rawCode, (wait? 0 : WNOHANG));

	if(fRet < 0)
	{
		WeakRune e("Whilst ::waitpid()");
		throw e.setError();
	}
	else if(! fRet)
	{
		WeakRune e("The process has not returned yet.");
		throw e;
	}

	JoinedProcess y(rawCode, this->pid, this);
	thisClass::__pidMap__.erase(this->pid);
	this->pid = -1;

	return y;
}

DescendantProcess::thisClass& DescendantProcess::kill(const int num) throw (WeakRune)
{
	if(! this->pid)
	{
		// Case: Called by child process (within the main() method)
		if(::kill(::getpid(), num) < 0)
		{
			WeakRune e("Whilst ::kill()");
			throw e.setError();
		}
	}
	// Case: Called by parent
	else if(::kill(this->pid, num) < 0)
	{
		WeakRune e("Whilst ::kill()");
		throw e.setError();
	}

	return *this;
}

DescendantProcess::thisClass& DescendantProcess::operator =(const thisClass& src) throw (WeakRune, StrongRune)
{
	if(! this->detached)
	{
		if(this->pid > 0)
			throw WeakRune("The operand process is running. Please join it first.");
		else if(this->pid == 0)
			throw StrongRune("The operand process is child process. Check your code!");
	}

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
	this->__daemonic = src.__daemonic;
	this->detached = false;
}

void DescendantProcess::__checkValidity() const throw (WeakRune)
{
	if(this->detached)
		throw WeakRune("The instance has detached.");
	else if(this->pid < 0)
		throw WeakRune("The instance is empty.");
}

DescendantProcess::thisClass& DescendantProcess::start() throw (WeakRune, StrongRune)
{
	if(! this->detached)
	{
		if(this->pid == 0)
		{
			StrongRune e("The instance is child process. Check your code!");
			throw e;
		}
		else if(this->pid > 0)
		{
			WeakRune e("The process is already running.");
			throw e;
		}
	}

	this->pid = ::fork();
	if(this->pid < 0)
	{
		WeakRune e("Whilst ::fork()");
		throw e.setError();
	}
	else if(this->pid)
		this->daemonic(this->__daemonic);
	else
		::exit(this->main());

	return *this;
}

pid_t DescendantProcess::getPID() const throw(WeakRune)
{
	this->__checkValidity();
	return this->pid? this->pid : ::getpid();
}

bool DescendantProcess::child() const throw(WeakRune)
{
	this->__checkValidity();
	return this->pid == 0;
}

DescendantProcess::thisClass& DescendantProcess::detach() noexcept
{
	this->detached = true;
	return *this;
}

bool DescendantProcess::daemonic() const noexcept
{
	return this->__daemonic;
}

DescendantProcess::thisClass& DescendantProcess::daemonic(const bool daemonic)  throw(WeakRune)
{
	if(this->detached || this->pid <= 0)
	{
		WeakRune e("Invalid action: Instance is detached or not running.");
		throw e;
	}

	if((this->__daemonic = daemonic))
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
	else if(this->pid == 0)
		sb << " [Child instance]";

	if(this->detached)
		sb << " (Detached)";

	return sb.str();
}

DescendantProcess::JoinedProcess DescendantProcess::join__() throw(WeakRune)
{
	int rawCode;
	const auto pid = ::wait(&rawCode);

	if(pid < 0)
	{
		WeakRune e("Whilst ::wait()");
		throw e.setError();
	}

	const auto it = thisClass::__pidMap__.find(pid);
	DescendantProcess *instance = NULL;
	if(it != thisClass::__pidMap__.end())
	{
		instance = it->second;
		instance->pid = -1;
	}

	return JoinedProcess(rawCode, pid, instance);
}

} /* namespace ashe */

namespace ashe
{

DescendantProcess::JoinedProcess::JoinedProcess(const int rawCode, const pid_t pid/* = -1*/, void *instance/* = NULL*/) noexcept
	: instance((DescendantProcess*)instance)
	, pid(pid)
	, rawCode(rawCode)
{
	this->className = "ashe::DescendantProcess::JoinedProcess";
}

DescendantProcess::JoinedProcess::JoinedProcess(const thisClass& src) noexcept
		: motherClass()
{
	this->__construct(src);
	this->className = "ashe::DescendantProcess::JoinedProcess";
}

DescendantProcess::JoinedProcess::~JoinedProcess()noexcept{}

DescendantProcess::JoinedProcess::thisClass& DescendantProcess::JoinedProcess::operator =(const thisClass& src) noexcept
{
	this->__construct(src);
	return *this;
}

void DescendantProcess::JoinedProcess::__construct(const thisClass& src) noexcept
{
	this->instance = src.instance;
	this->pid = src.pid;
	this->rawCode = src.rawCode;
}

bool DescendantProcess::JoinedProcess::exitedNormally() const noexcept
{
	return WIFEXITED(this->rawCode);
}

int DescendantProcess::JoinedProcess::exitCode() const noexcept
{
	return WEXITSTATUS(this->rawCode);
}

bool DescendantProcess::JoinedProcess::signaled() const noexcept
{
	return WTERMSIG(this->rawCode);
}

int DescendantProcess::JoinedProcess::signalNumber() const noexcept
{
	return WTERMSIG(this->rawCode);
}

std::string DescendantProcess::JoinedProcess::signalString() const noexcept
{
	std::stringstream sb;
	const auto theNumber = this->signalNumber();
	const auto str = ::strsignal(theNumber);

	sb << '(' << theNumber << ')';
	if(str)
		sb << ' ' << str;
	else
		sb << " Unknown";

	return sb.str();
}

bool DescendantProcess::JoinedProcess::coreDumped() const throw (WeakRune)
{
#ifdef WCOREDUMP
	return WCOREDUMP(this->rawCode);
#else
	throw WeakRune("Not supported operating system.");
	return false;
#endif
}

}

