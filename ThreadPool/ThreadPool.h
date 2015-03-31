/*
* ThreadPool.h
*
* Ashe's thread pool with C++11
* On UNIX, link with lpthread
*
* @Maintained
*  2015 Q1
* @Author
*  Ashe David Sterkhus
*  Blame to: ashe.goulding+blame@gmail.com
* @COLOPHON
*  This file is part of libashe, Ashe's C++11/98 utility stuff
*/
#ifndef ASHE_THREADPOOL_H_
#define ASHE_THREADPOOL_H_

#include <thread>
#include <mutex>
#include <future>

#include "Frelish.h"
#include <string>

#include <queue>
#include <set>

namespace ashe
{

class ThreadPool : public Fjord
{
/* Implement WorkUnit first for PoolThread to run.
*
* @Usage flow:
*  1. ThreadPool instantiation
*  2. (Optional) set non-block labour
*  3. Put the PoolThreads to work with labour() method
*  4. Created PoolThreads shall be implicitly joined when the instance's deletion
*
* @Note
*  - This entire class is thread-safe except labour() method
*/
public:
	typedef Fjord motherClass;
	typedef ThreadPool thisClass;

	class Rune : public WeakRune
	{
	public:
		typedef WeakRune motherClass;
		typedef Rune thisClass;

		enum Code
		{
			C_NONE,
			C_EMPTY_POOL, // The instance has no thread for labour
			C_ALL_THREAD_BUSY, // All the PoolThreads are busy when non-block labour is set
			C_ILLEGAL_ARGUMENT // Method has been invoked with illeagal argument
		};

		// Converts Code enumeration to literal string
		static std::string codeToString__(const Code x) noexcept;

	protected:
		Code code;

		void __construct(const thisClass &src) noexcept;

	public:
		Rune(const Code code, const std::string msg = "") noexcept;
		Rune(const thisClass &src) noexcept;
		virtual ~Rune() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual Code getCode() const noexcept;
	};

	class WorkUnit : public Fjord
	{
	// WorkUnit that PoolThread runs when it's given by labour() method
	public:
		typedef Fjord motherClass;
		typedef WorkUnit thisClass;

		/* Constructors do nothing special
		 */
		WorkUnit() noexcept;
		WorkUnit(const thisClass &src) noexcept;
		virtual ~WorkUnit() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		/* A method that arbitrary PoolThread runs.
		 * Override this method to make the thread to do something.
		 *
		 * Do not throw anything within this method. No code catches anything from this method()
		 * Or the program shall be terminated.
		 * To catch anything from this method, implement your own PoolThread::__run() method,
		 * that handles exceptions and then spawns a thread with __spawnPoolThreads() method.
		 */
		virtual void onTick();
	};

protected:
	class PoolThread : protected std::thread
	{
	/* Thread class that would be nurtured by superior class.
	* I don't think you will ever get to derive this class.
	*
	* @Behaviour
	*  - construction: Unleashes a freshly born std::thread to __run() method.
	*  - destruction: Invokes die() method -- Calls the thread in and then joins it if it is running.
	* @Note
	*  - You're not gonna copy a thread, would you? nor this class
	*/
	public:
		typedef std::thread motherClass;
		typedef PoolThread thisClass;

	protected:
		ThreadPool *mother = NULL;
		std::promise<WorkUnit*> omen;
		std::mutex omenMtx;

		/* Override and implement your own __run() method,
		 * if you wish to catch anything pops up from WorkUnit::onTick() method.
		 *
		 * @Behaviour
		 *  - Waits to be awaken by ThreadPool::labour() method.
		 *  - Returns if its mother(ThreadPool) calls itself in, or runs given WorkUnit
		 */
		virtual void __run() noexcept;

	public:
		// Assertion fails if 'mother' is NULL
		PoolThread(ThreadPool *mother) noexcept;
		// Do not invoke this
		PoolThread(const thisClass &src) throw(StrongRune);
		virtual ~PoolThread() noexcept;

		// Do not invoke this
		virtual thisClass &operator =(const thisClass &src) throw(StrongRune);

		/* Gives a WorkUnit to work on
		 *
		 * @Behaviour
		 *  - If the given WorkUnit is NULL, __run() method shall return
		 */
		virtual thisClass &labour(WorkUnit *wu) noexcept;
		virtual thisClass &die() noexcept;
	};

	unsigned short __initialSize; // The constructed number of threads
	bool labourBlocking = true; // Does not blocks current thread if true

	std::mutex __methodMtx; // A mutex for __resting, __working, __restingEvent
	std::queue<PoolThread*> __resting; // Threads that are currently not working and ready to be deployed
	std::set<PoolThread*> __working; // Threads that are currently working on the given WorkUnits
	/* An event object that's used when labour() method blocked due to empty __resting.
	 * A value is set to __resting's new size every time it changes.
	 */
	std::promise<unsigned short> __restingEvent;

	void __construct(const thisClass &src) noexcept;
	virtual void __spawnPoolThreads(const unsigned short x) noexcept;

	// PoolThreads reports to this method when their work is done.
	virtual void __onTickEnd(PoolThread *th) noexcept;

public:
	/* Default value of 'spawn' is the number of the system's threads
	 *
	 * @NOTE
	 *  - Empty ThreadPool is valid instance, but only that is just a baloney
	 *  - Be sure to instantiate with non-zero number of spawn threads
	 *  - Empty ThreadPool's labour() method shall only throw Rune
	 */
	ThreadPool(const unsigned short spawn = std::thread::hardware_concurrency()) noexcept;
	// Refer to operator =()
	ThreadPool(const thisClass &src) noexcept;
	virtual ~ThreadPool() noexcept;

	/* This profound copy-constructor assigns given instance's attribute -- __initialSize, labourBlocking
	 *
	 * @Behaviour
	 *  - Invokes __recall()
	 *  - Assigns its attributes the given instance
	 *  - Respawns the number of threads that of the given instance
	 */
	virtual thisClass &operator =(const thisClass &src) noexcept;

	/* Returns the __initialSize
	 *
	 * @Note
	 *  - The return value is not the number of the PoolThreads currently active
	 */
	virtual unsigned short size() const noexcept;
	/* Counts currently sleeping, resting PoolThreads
	 *
	 * @Note
	 *  - Uses mutex. Calling this often may cause bottleneck
	 */
	virtual unsigned short countResting() noexcept;
	/* Counts currently working, deployed PoolThreads
	 *
	 * @Note
	 *  - Uses mutex internally. Calling this often may cause bottleneck
	 */
	virtual unsigned short countWorking() noexcept;
	/* Combination of countResting(), countWorking() methods.
	 * With previous methods, there was no way to know both resting and working PoolThread
	 * at a certain time point, calling those two methods separately couldn't get exact readings due to data race.
	 *
	 * @Note
	 *  - Uses mutex internally. Calling this often may cause bottleneck
	 */
	virtual thisClass &count(unsigned short *resting, unsigned short *working) noexcept;

	/* Sets whether labour() method should block calling thread when all the PoolThreads are busy
	 *
	 * @NOTE
	 *  - This is not thread-safe
	 *  - Invoke this method before being accessed by multiple threads
	 */
	virtual thisClass &setNonBlockingLabour(const bool nonBlocking = true) noexcept;
	// Tests if labour() blocks. Returns opposite value of labourBlocking
	virtual bool blockingLabour() const noexcept;
	/* Assigns a PoolThread given WorkUnit
	 *
	 * @Throws - Rune instance
	 *  - C_ILLEGAL_ARGUMENT: 'wu' is NULL
	 *  - C_EMPTY_POOL: Unable to assign a PoolThread for WorkUnit due to empty pool.
	 *  This would never happen unless the instance was constructed with 0 initial PoolThreads
	 *  (or buggy std::thread implementation)
	 *  Be sure to not construct empty ThreadPool
	 *  - C_ALL_THREAD_BUSY: All the PoolThread are working and deployed and calling thread should not be blocked
	 */
	virtual thisClass &labour(WorkUnit *wu) throw(Rune);
	/* Calls in all the PoolThreads that the instance is conceiving.
	* The destructor and copy-constructor invokes this method.
	*
	* @NOTE
	*  - Call this method before fork()
	*/
	virtual void recall() noexcept;
};

} /* namespace ashe */

#endif /* THREADPOOL_H_ */
