/*
 * ThreadPool.h
 *
 *  Created on: 2015. 3. 18.
 *      Author: david
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <thread>
#include <mutex>
#include <future>

#include <exception>
#include <string>

#include <queue>
#include <set>

namespace ashe
{

class ThreadPool
{
public:
	typedef ThreadPool thisClass;

	class Exception : public std::exception
	{
	public:
		typedef Exception thisClass;

		enum Code
		{
			C_NONE,
			C_EMPTY_POOL,
			C_ALL_THREAD_BUSY,
			C_ILLEGAL_ARGUMENT
		};

		static std::string codeToString__(const Code x) noexcept;

	protected:
		Code code;
		std::string msg;

		void __construct(const thisClass &src) noexcept;

	public:
		Exception(const Code code, const std::string msg = "") noexcept;
		Exception(const thisClass &src) noexcept;
		virtual ~Exception() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual Code getCode() const noexcept;
		virtual std::string toString() const noexcept;
		virtual const char *what() const noexcept;
	};

	class WorkUnit
	{
	public:
		typedef WorkUnit thisClass;

		virtual ~WorkUnit() noexcept;
		virtual void onTick() noexcept;
	};

protected:
	class PoolThread : protected std::thread
	{
	public:
		typedef std::thread motherClass;
		typedef PoolThread thisClass;

	protected:
		ThreadPool *mother;
		std::promise<WorkUnit*> omen;
		std::mutex omenMtx;

		virtual void __run() noexcept;

	public:
		PoolThread(ThreadPool *mother) noexcept;
		virtual ~PoolThread() noexcept;

		virtual thisClass &labour(WorkUnit *wu) noexcept;
		virtual thisClass &die() noexcept;
	};

	unsigned short __initialSize;
	bool labourBlocking = true;
	std::mutex __methodMtx;

	std::queue<PoolThread*> __resting;
	std::set<PoolThread*> __working;

	std::promise<unsigned short> __restingEvent;

	void __construct(const thisClass &src) noexcept;
	virtual void __spawnPoolThreads(const unsigned short x) noexcept;
	virtual void __recall() noexcept;

	virtual void __onTickEnd(PoolThread *th) noexcept;

public:
	ThreadPool(const unsigned short spawn = std::thread::hardware_concurrency()) noexcept;
	ThreadPool(const thisClass &src) noexcept;
	virtual ~ThreadPool() noexcept;

	virtual thisClass &operator =(const thisClass &src) noexcept;

	virtual unsigned short size() const noexcept;
	virtual unsigned short countResting() noexcept;
	virtual unsigned short countWorking() noexcept;
	virtual thisClass &count(unsigned short *resting, unsigned short *working) noexcept;

	virtual thisClass &setNonBlockingLabour(const bool nonBlocking = true) noexcept;
	virtual bool blockingLabour() const noexcept;
	virtual thisClass &labour(WorkUnit *wu) throw(Exception);
};

} /* namespace ashe */

#endif /* THREADPOOL_H_ */
