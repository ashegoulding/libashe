/*
* DescendantProcess.h
*
* @Maintained
*  Under development since: 2015 Q1
* @Author
*  Ashe David Sterkhus
*  Blame to: ashe.goulding+blame@gmail.com
* @COLOPHON
*  This file is part of libashe, Ashe's C++11/98 utility stuff
*
* @DESCRIPTION
*  - Wrapping class for posix fork() and wait()
*/
#ifndef ASHE_DESCENDANTPROCESS_H_
#define ASHE_DESCENDANTPROCESS_H_

#include "Frelish.h"

#include <unistd.h>
#include <wait.h>

#include <map>
#include <mutex>

namespace ashe
{

class DescendantProcess : public Icebank
{
/* C++11 wrapping class for posix fork() and wait()
* This one instance is one child process.
* Derive this class and implement main() method for a child process to do something.
*
* @NOTE
*  - Term 'join' is actually wait() function in posix.
*  - This class is reusable after join() or detach() or
*  - Not thread-safe. One thread for creation and one thread for join__(). Behaviour on more threads than those is undefined.
* @Start and join (wait)
*  - By signal(recommended in a single threaded program): include <signal.h> and set handler for SIGCHLD. In the handler, join a process by join__().
*   + The processes should be detached or daemonic.
*   + Handling signals in multi-threaded program is a bit strange (Googld for signal handling and pthread signal masks and handler)
*  - By handling thread(recommended in a multi-threaded program): Start a process and detach or unset daemonic. loop join__() in a thread that handles for process join.
*  - By mother thread: Start a process and join it in a single thread.
*/
public:
	typedef Icebank motherClass;
	typedef DescendantProcess thisClass;

	class JoinedProcess : public Icebank
	{
	/* Class that holds information about joined process.
	* Return type of join() and join__() method.
	*/
	public:
		typedef Icebank motherClass;
		typedef JoinedProcess thisClass;

		// Can be NULL, if an instance that handles its process has been deleted
		DescendantProcess *instance = NULL;
		pid_t pid = -1;
		int rawCode = 0;

		void __construct(const thisClass &src) noexcept;

	public:
		JoinedProcess(const int rawCode, const pid_t pid = -1, void *instance = NULL) noexcept;
		JoinedProcess(const thisClass &src) noexcept;
		virtual ~JoinedProcess() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		// Tests if the process returned by calling exit() or returning main() function.
		// Returns false if the process has been terminated by a signal.
		virtual bool exitedNormally() const noexcept;
		// Returns the exit code of the process.
		// Return value is not valid if the return value of exitedNormally() is false.
		virtual int exitCode() const noexcept;
		// Tests if the process has been terminated by a signal.
		// Returns false if the process has been normally returned by calling exit() or returning main() function.
		virtual bool signaled() const noexcept;
		// Returns a signal number that caused termination of the process.
		// Return value is not valid if the return value of signaled() is false.
		virtual int signalNumber() const noexcept;
		// Returns a string describing the signal that the process has received for termination.
		// Returned string is not valid if the return value of signaled() is false.
		virtual std::string signalString() const noexcept;
		// Tests if the core of process has been dumped.
		// Throws if the system doesn't support WCOREDUMP feature. Almost all the system supports this. Don't mind not catching it explicitly.
		virtual bool coreDumped() const throw(WeakRune);

		// Returns a string describing the whole information of instance.
		// The string is fairly human-readable. You can use this string right to Standard output.
		virtual std::string toString() const noexcept;
	};

	/* Waits any child process by wait()
	* After this method, pid member will set to -1 which means the instance will return to original state.
	* This method is also used to join detached or non daemonic process.
	* Does not block when 'wait' given false.
	*
	* @Throws
	*  No process was there to join(), with 'wait' is given false.
	*  Rare case, OS-defined error whilst waitpid()
	*/
	static JoinedProcess join__(const bool wait = true) throw(WeakRune);

protected:
	typedef std::map<pid_t, DescendantProcess*> __PIDMapType;

	// PID - instance map for join__() method.
	static __PIDMapType __pidMap__;
	static std::mutex __pidMapMtx__;

	pid_t pid = -1;
	bool detached = false;
	// If set, the thread will not wait for child process to exit on delete.
	bool __daemonic = true;

	/* main function of the forked process.
	* Override this function to implement a process.
	*
	* The return value can be inspected by parent process.
	* Uncaught exception, SEGFAULT, assert() within this method does not affect the parent process.
	*/
	virtual int main();

	void __construct(const thisClass &src) noexcept;
	virtual void __checkValidity() const throw(WeakRune);

public:
	DescendantProcess() noexcept;
	/* Constructs from detached or not running instance.
	*
	* src: An instance to construct from
	* Throws: When 'src' is running or not detached
	*/
	DescendantProcess(const thisClass &src) throw(WeakRune);
	virtual ~DescendantProcess() noexcept;

	/* Replaces the instance with detached or not running instance.
	*
	* src: An instance to replace with
	*
	* Throws: When 'src' is running or not detached
	*/
	virtual thisClass &operator =(const thisClass &src) throw(WeakRune, StrongRune);
	// Detaches the instance for another instance to use.
	virtual thisClass &detach() noexcept;

	/* Starts a process.
	*
	* Throws:
	*  - When the process is already running.
	*  - OS-defined error whilst fork(): Permission or system limit.
	*/
	virtual thisClass &start() throw(WeakRune, StrongRune);
	/* Joins with the started process.
	* After this method, pid will be set to -1 which means the instance will return to original state.
	*
	* wait: When 'false', calls waitpid() with WNOHANG which does not block the thread
	*
	* Return: The exit code of the process
	*
	* Throws:
	*  - A process was not running
	*  - Rare case, OS-defined error whilst waitpid()
	*/
	virtual JoinedProcess join(const bool wait = true) throw(WeakRune, StrongRune);
	/* Sends a signal to the running process.
	* By default, sends 'SIGKILL' signal
	*
	* num: signal to send. 'SIGKILL' by default
	*
	* @NOTE
	*  - This method is not only just for SIGKILL. It can be used to send other signals like SIGUSR1
	*  - Therefore invoking this method does not guarantee termination of the process.
	*
	* Throws WeakRune: OS-defined error whilst kill()
	*  - A process was not running
	*  - Permission denied
	*/
	virtual thisClass &kill(const int num = SIGKILL) throw(WeakRune);

	/* Returns the pid of running process.
	*
	* Return: PID that the instance is handling
	* @Throws
	*  - A process was not running.
	*  - The instance has detached.
	*/
	virtual pid_t getPID() const throw(WeakRune);
	/* Check if the calling process is the child process itself.
	* Calling this from main() method will always return true.
	*
	* @Throws
	*  - A process was not running.
	*  - The instance has detached.
	*/
	virtual bool child() const throw(WeakRune);
	// Tests the validity of instance. Returns false if a process was not running or the instnace has detached.
	virtual bool valid() const noexcept;
	/* Tests daemonic state of the instance.
	*
	* Return: 'true' when the instance, and its process is daemonic
	* @Throws
	*  - A process was not running.
	*  - The instance has detached.
	*/
	virtual bool daemonic() const throw(WeakRune);
	/* Sets the instance daemonic. Invoke this method after a process starts, before join() method.
	* If the instance is non-daemonic, the thread will not wait for the process on delete.
	* After instance deletion, the only way to join with that process is using join__() method.
	*
	* daemonic: 'true' means daemonic
	*
	* @Throws
	*  - When the instance is not running or detached
	*/
	virtual thisClass &daemonic(const bool daemonic = true) throw(WeakRune);

	virtual std::string toString() const noexcept;
	};

} /* namespace ashe */

#endif /* DESCENDANTPROCESS_H_ */
