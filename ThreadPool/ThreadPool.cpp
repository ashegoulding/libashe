/*
 * ThreadPool.cpp
 *
 *  Created on: 2015. 3. 18.
 *      Author: david
 */

#include "ThreadPool.h"

#include <sstream>
#include <assert.h>

namespace ashe
{

ThreadPool::ThreadPool(const unsigned short spawn/* = std::thread::hardware_concurrency()*/) noexcept
		: __initialSize(spawn)
{
	this->__spawnPoolThreads(spawn);
}

ThreadPool::ThreadPool(const thisClass &src) noexcept
		: __initialSize(0)
{
	this->__construct(src);
}

void ThreadPool::__construct(const thisClass& src) noexcept
{
	this->__recall();
	this->__initialSize = src.__initialSize;
	this->labourBlocking = src.labourBlocking;
	this->__spawnPoolThreads(src.__initialSize);
}

void ThreadPool::__recall() noexcept
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

ThreadPool::~ThreadPool() noexcept
{
	this->__recall();
}

ThreadPool::thisClass& ThreadPool::operator =(const thisClass& src) noexcept
{
	this->__construct(src);
	return *this;
}

unsigned short ThreadPool::size() const noexcept
{
	return this->__initialSize;
}

unsigned short ThreadPool::countResting() noexcept
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	return (unsigned short)this->__resting.size();
}

unsigned short ThreadPool::countWorking() noexcept
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	return (unsigned short)this->__working.size();
}

ThreadPool::thisClass& ThreadPool::count(unsigned short * resting, unsigned short * working) noexcept
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	if(resting)
		*resting = (unsigned short)this->__resting.size();
	if(working)
		*working = (unsigned short)this->__working.size();

	return *this;
}

void ThreadPool::__onTickEnd(PoolThread* th) noexcept
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	std::set<PoolThread*>::iterator pos = this->__working.find(th);

	if(pos != this->__working.end())
	{
		this->__working.erase(pos);
		this->__resting.push(th);
		this->__restingEvent.set_value((unsigned short)this->__resting.size());
		this->__restingEvent = std::promise<unsigned short>();
	}
}

void ThreadPool::__spawnPoolThreads(const unsigned short x) noexcept
{
	std::lock_guard<std::mutex> l(this->__methodMtx);
	unsigned short i;

	for(i=0; i<x; ++i)
		this->__resting.push(new PoolThread(this));
}

ThreadPool::thisClass& ThreadPool::setNonBlockingLabour(const bool nonBlocking) noexcept
{
	this->labourBlocking = !nonBlocking;
	return *this;
}

bool ThreadPool::blockingLabour() const noexcept
{
	return this->labourBlocking;
}

ThreadPool::thisClass& ThreadPool::labour(WorkUnit* wu) throw(Exception)
{
	if(! wu)
		throw Exception(Exception::C_ILLEGAL_ARGUMENT, "WorkUnit pointer cannot be NULL.");
	else if(this->labourBlocking)
	{
		this->__methodMtx.lock();
		if(this->__resting.empty())
		{
			unsigned short resting;
			std::future<unsigned short> restingFuture;
			while(true)
			{
				restingFuture = this->__restingEvent.get_future();
				this->__methodMtx.unlock();
				resting = restingFuture.get();
				this->__methodMtx.lock();
				if(resting)
					break;
			}
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
				throw Exception(Exception::C_EMPTY_POOL);
			else
				throw Exception(Exception::C_ALL_THREAD_BUSY);
		}
		this->__resting.front()->labour(wu);
		this->__working.insert(this->__resting.front());
		this->__resting.pop();
	}

	return *this;
}

} /* ThreadPool */

namespace ashe
{

ThreadPool::Exception::Exception(const Code code, const std::string msg/* = ""*/) noexcept
		: code(code)
{
	std::stringstream sb;
	sb << '[' << thisClass::codeToString__(code) << ']';
	if(! msg.empty())
		sb << ' ' << msg;
	this->msg = sb.str();
}

ThreadPool::Exception::~Exception() noexcept
{
}

ThreadPool::Exception::Code ThreadPool::Exception::getCode() const noexcept
{
	return this->code;
}

std::string ThreadPool::Exception::toString() const noexcept
{
	return this->msg;
}

std::string ThreadPool::Exception::codeToString__(const Code x) noexcept
{
	switch(x)
	{
	case C_EMPTY_POOL: return "C_EMPTY_POOL";
	case C_ALL_THREAD_BUSY: return "C_ALL_THREAD_BUSY";
	case C_ILLEGAL_ARGUMENT: return "C_ILLEGAL_ARGUMENT";
	}
	return "C_NONE";
}

void ThreadPool::Exception::__construct(const thisClass& src) noexcept
{
	this->code = src.code;
	this->msg = src.msg;
}

ThreadPool::Exception::Exception(const thisClass& src) noexcept
		: code(src.code)
{
	this->__construct(src);
}

ThreadPool::Exception::thisClass& ThreadPool::Exception::operator =(const thisClass& src) noexcept
{
	this->__construct(src);
	return *this;
}

const char* ThreadPool::Exception::what() const noexcept
{
	return this->msg.c_str();
}

} /* ThreadPool::Exception */

namespace ashe
{

ThreadPool::WorkUnit::~WorkUnit() noexcept{}
void ThreadPool::WorkUnit::onTick() noexcept{}

} /* ThreadPool::WorkUnit */

namespace ashe
{

void ThreadPool::PoolThread::__run() noexcept
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

ThreadPool::PoolThread::PoolThread(ThreadPool *mother) noexcept
		: mother(mother)
{
	*((std::thread*)this) = std::thread([this](){
		this->__run();
	});
}

ThreadPool::PoolThread::~PoolThread() noexcept
{
	this->die();
}

ThreadPool::PoolThread::thisClass& ThreadPool::PoolThread::labour(WorkUnit *wu) noexcept
{
	std::lock_guard<std::mutex> l(this->omenMtx);
	this->omen.set_value(wu);

	return *this;
}

ThreadPool::PoolThread::thisClass& ThreadPool::PoolThread::die() noexcept
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
