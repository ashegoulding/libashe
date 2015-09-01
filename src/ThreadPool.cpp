#include <sstream>
#include <assert.h>
#include "libashe/ThreadPool.h"

namespace ashe
{

ThreadPool::ThreadPool(const uint16_t spawn/* = std::thread::hardware_concurrency()*/) ASHE_NOEXCEPT
		: __initialSize(spawn)
{
	this->className = "ashe::ThreadPool";
	this->__spawnPoolThreads(spawn);
}

ThreadPool::ThreadPool(const thisClass &src) ASHE_NOEXCEPT
		: motherClass(src)
		, __initialSize(0)
{
	this->className = "ashe::ThreadPool";
	this->__construct(src);
}

ThreadPool::~ThreadPool() ASHE_NOEXCEPT
{
	this->recall();
}

ThreadPool::thisClass& ThreadPool::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->__construct(src);
	this->className = "ashe::ThreadPool";
	return *this;
}

void ThreadPool::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->recall();
	this->__initialSize = src.__initialSize;
	this->labourBlocking = src.labourBlocking;
	this->__spawnPoolThreads(src.__initialSize);
}

void ThreadPool::recall() ASHE_NOEXCEPT
{
	PoolThread *th;

	while(true)
	{
		this->__methodMtx.lock();
		if(this->__resting.empty())
		{
			this->__methodMtx.unlock();
			break;
		}
		th = this->__resting.front();
		this->__resting.pop();
		this->__methodMtx.unlock();
		delete th;
	}
	while(true)
	{
		this->__methodMtx.lock();
		if(this->__working.empty())
		{
			this->__methodMtx.unlock();
			break;
		}
		th = *this->__working.begin();
		this->__working.erase(this->__working.begin());
		this->__methodMtx.unlock();
		delete th;
	}
}

void ThreadPool::__spawnPoolThreads(const uint16_t x) ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	uint16_t i;

	for(i=0; i<x; ++i)
		this->__resting.push(new PoolThread(this));
}

void ThreadPool::__onTickEnd(PoolThread* th) ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	std::set<PoolThread*>::iterator pos = this->__working.find(th);

	if(pos != this->__working.end())
	{
		this->__working.erase(pos);
		this->__resting.push(th);
		this->__restingEvent.set_value((uint16_t)this->__resting.size());
		this->__restingEvent = std::promise<uint16_t>();
	}
}

ThreadPool::thisClass& ThreadPool::labour(WorkUnit* wu) throw(Rune)
{
	if(! wu)
		throw Rune(Rune::C_ILLEGAL_ARGUMENT, "WorkUnit pointer cannot be NULL."); // Or a random PoolThread's gonna die
	else if(this->labourBlocking)
	{
		this->__methodMtx.lock();
		if(this->__resting.empty())
		{
			if(this->__working.empty())
			{
				this->__methodMtx.unlock();
				throw Rune(Rune::C_EMPTY_POOL, "Was trying to block");
			}

			std::future<uint16_t> restingFuture;
			do
			{
				restingFuture = this->__restingEvent.get_future();
				this->__methodMtx.unlock();
				restingFuture.get();
				this->__methodMtx.lock();
			}
			while(this->__resting.empty());
		}

		this->__resting.front()->labour(wu);
		this->__working.insert(this->__resting.front());
		this->__resting.pop();

		this->__methodMtx.unlock();
	}
	else
	{
		std::lock_guard<std::mutex> l(this->__methodMtx);

		if(this->__resting.empty())
		{
			if(this->__working.empty())
				throw Rune(Rune::C_EMPTY_POOL, "Was non-blocking");
			else
				throw Rune(Rune::C_ALL_THREAD_BUSY);
		}
		this->__resting.front()->labour(wu);
		this->__working.insert(this->__resting.front());
		this->__resting.pop();
	}

	return *this;
}

uint16_t ThreadPool::size() const ASHE_NOEXCEPT
{
	return this->__initialSize;
}

uint16_t ThreadPool::countResting() ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	return (uint16_t)this->__resting.size();
}

uint16_t ThreadPool::countWorking() ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	return (uint16_t)this->__working.size();
}

ThreadPool::thisClass& ThreadPool::count(uint16_t * resting, uint16_t * working) ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	if(resting)
		*resting = (uint16_t)this->__resting.size();
	if(working)
		*working = (uint16_t)this->__working.size();

	return *this;
}

ThreadPool::thisClass& ThreadPool::setNonBlockingLabour(const bool nonBlocking) ASHE_NOEXCEPT
{
	this->labourBlocking = !nonBlocking;
	return *this;
}

bool ThreadPool::blockingLabour() const ASHE_NOEXCEPT
{
	return this->labourBlocking;
}

} /* ThreadPool */

namespace ashe
{

ThreadPool::Rune::Rune(const Code code, const std::string msg/* = ""*/) ASHE_NOEXCEPT
		: code(code)
{
	this->className = "ashe::ThreadPool::Rune";
	std::stringstream sb;
	sb << '[' << thisClass::codeToString__(code) << ']';
	if(! msg.empty())
		sb << ' ' << msg;
	this->whatString = sb.str();
}

ThreadPool::Rune::Rune(const thisClass& src) ASHE_NOEXCEPT
		: motherClass(src)
		, code(src.code)
{
	this->__construct(src);
	this->className = "ashe::ThreadPool::Rune";
}

ThreadPool::Rune::~Rune() ASHE_NOEXCEPT
{
}

ThreadPool::Rune::thisClass& ThreadPool::Rune::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	motherClass::__construct(src);
	this->__construct(src);
	this->className = "ashe::ThreadPool::Rune";
	return *this;
}

ThreadPool::Rune::Code ThreadPool::Rune::getCode() const ASHE_NOEXCEPT
{
	return this->code;
}

std::string ThreadPool::Rune::codeToString__(const Code x) ASHE_NOEXCEPT
{
	switch(x)
	{
	case C_EMPTY_POOL: return "C_EMPTY_POOL";
	case C_ALL_THREAD_BUSY: return "C_ALL_THREAD_BUSY";
	case C_ILLEGAL_ARGUMENT: return "C_ILLEGAL_ARGUMENT";
	}
	return "C_NONE";
}

void ThreadPool::Rune::__construct(const thisClass& src) ASHE_NOEXCEPT
{
	this->code = src.code;
}

} /* ThreadPool::Exception */

namespace ashe
{

ThreadPool::WorkUnit::WorkUnit() ASHE_NOEXCEPT
{
	this->className = "ashe::ThreadPool::WorkUnit";
}

ThreadPool::WorkUnit::WorkUnit(const thisClass& src) ASHE_NOEXCEPT
		: motherClass(src)
{
	this->className = "ashe::ThreadPool::WorkUnit";
}

ThreadPool::WorkUnit::~WorkUnit() ASHE_NOEXCEPT {}

ThreadPool::WorkUnit::thisClass& ThreadPool::WorkUnit::operator =(const thisClass& src) ASHE_NOEXCEPT
{
	return *this;
}

void ThreadPool::WorkUnit::onTick(){}

} /* ThreadPool::WorkUnit */

namespace ashe
{

ThreadPool::PoolThread::PoolThread(ThreadPool *mother) ASHE_NOEXCEPT
		: mother(mother)
{
	assert(mother);
	*((std::thread*)this) = std::thread([this](){
		this->__run();
	});
}

ThreadPool::PoolThread::PoolThread(const thisClass& src) throw (StrongRune)
{
	throw StrongRune("Nothing can copy a thread!");
}

ThreadPool::PoolThread::~PoolThread() ASHE_NOEXCEPT
{
	this->die();
}

ThreadPool::PoolThread::thisClass& ThreadPool::PoolThread::operator =(const thisClass& src) throw (StrongRune)
{
	throw StrongRune("Nothing can copy a thread!");
	return *this;
}

void ThreadPool::PoolThread::__run() ASHE_NOEXCEPT
{
	std::future<WorkUnit*> fulfillment = this->omen.get_future();
	WorkUnit *wu;
PROC_START:
	if(! (wu = fulfillment.get()))
		return;

	this->omenMtx.lock();
	this->omen = std::promise<WorkUnit*>();
	fulfillment = this->omen.get_future();
	this->omenMtx.unlock();

	wu->onTick();
	this->mother->__onTickEnd(this);

	goto PROC_START;
}

ThreadPool::PoolThread::thisClass& ThreadPool::PoolThread::labour(WorkUnit *wu) ASHE_NOEXCEPT
{
	std::lock_guard<std::mutex> l(this->omenMtx);
	this->omen.set_value(wu);

	return *this;
}

ThreadPool::PoolThread::thisClass& ThreadPool::PoolThread::die() ASHE_NOEXCEPT
{
	if(this->joinable())
	{
		this->omenMtx.lock();
		this->omen.set_value(NULL);
		this->omenMtx.unlock();
		this->join();
	}
	return *this;
}
} /* ThreadPool::PoolThread */

