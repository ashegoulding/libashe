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

namespace ashe
{

class DescendantProcess : public Icebank
{
/* DescendantProcess: Wrapper class for fork()
* NOTE: Do not use with multiple threads.
** POSIX doesn't support that and it will cause deadlock.
*/
public:
	typedef Icebank motherClass;
	typedef DescendantProcess thisClass;

	// Return type of wait(), which is join__() in this class.
	class JoinedProcess : public Icebank
	{
	public:
		typedef Icebank motherClass;
		typedef JoinedProcess thisClass;

		DescendantProcess *instance = NULL; // Can be NULL, if an instance that handles its process has been deleted
		pid_t pid = -1;
		int rawCode = 0;

		void __construct(const thisClass &src) noexcept;

	public:
		JoinedProcess(const int rawCode, const pid_t pid = -1, void *instance = NULL) noexcept;
		JoinedProcess(const thisClass &src) noexcept;
		virtual ~JoinedProcess() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual bool exitedNormally() const noexcept;
		virtual int exitCode() const noexcept;
		virtual bool signaled() const noexcept;
		virtual int signalNumber() const noexcept;
		virtual std::string signalString() const noexcept;
		virtual bool coreDumped() const throw(WeakRune);

		virtual std::string toString() const noexcept;
	};

	/* Waits any child process by wait()
	 * After this method, pid will be set to -1 which means the instance will return to original state.
	 *
	 * Return: A structure that describes the exited process
	 *
	 * Throws WeakRune:
	 ** No process to wait
	 ** Rare case, OS-defined error whilst wait()
	 */
	static JoinedProcess join__() throw(WeakRune);

protected:
	typedef std::map<pid_t, DescendantProcess*> __PIDMapType;

	// PID - instance map for join__() method.
	static __PIDMapType __pidMap__;

	pid_t pid = -1;
	bool detached = false;
	// If set, the thread will not wait for child process to exit on delete.
	bool __daemonic = true;

	/* main function of the forked process.
	 * Override this function to implement a process.
	 *
	 * Return: Exit code of the process (like usual main() return)
	 */
	virtual int main();

	void __construct(const thisClass &src) noexcept;
	virtual void __checkValidity() const throw(WeakRune);

public:
	DescendantProcess() noexcept;
	/* Constructs from detached or not running instance.
	 *
	 * src: An instance to construct from
	 *
	 * Throws WeakRune: When 'src' is running or not detached
	 */
	DescendantProcess(const thisClass &src) throw(WeakRune);
	virtual ~DescendantProcess() noexcept;

	/* Replaces the instance with detached or not running instance.
	 *
	 * src: An instance to replace with
	 *
	 * Throws WeakRune: When 'src' is running or not detached
	 */
	virtual thisClass &operator =(const thisClass &src) throw(WeakRune, StrongRune);
	/* Detaches the instance for another instance to use.
	 * This method can be invoked even if the process has started.
	 */
	virtual thisClass &detach() noexcept;

	/* Starts a process.
	 * NOTE: After join(), it can be re-ran with this method.
	 *
	 * Throws WeakRune:
	 ** When the process is already running
	 ** OS-defined error whilst fork(): Permission or system limit
	 */
	virtual thisClass &start() throw(WeakRune, StrongRune);
	/* Joins with the started process.
	 * After this method, pid will be set to -1 which means the instance will return to original state.
	 *
	 * wait: When 'false', calls waitpid() with WNOHANG which does not block the thread
	 *
	 * Return: The exit code of the process
	 *
	 * Throws WeakRune:
	 ** A process was not running
	 ** Rare case, OS-defined error whilst waitpid()
	 */
	virtual JoinedProcess join(const bool wait = true) throw(WeakRune, StrongRune);
	/* Sends a signal to the running process.
	 * By default, sends 'SIGKILL' signal
	 *
	 * num: signal to send. 'SIGKILL' by default
	 *
	 * Throws WeakRune: OS-defined error whilst kill()
	 ** A process was not running
	 ** Permission denied
	 */
	virtual thisClass &kill(const int num = SIGKILL) throw(WeakRune);

	/* Returns the pid of a running process.
	 * It returns even if a process is not running. -1 will be returned in this case.
	 *
	 * Return: PID that the instance is handling
	 */
	virtual pid_t getPID() const throw(WeakRune);
	virtual bool child() const throw(WeakRune);
	/* Tests daemonic state of the instance.
	 *
	 * Return: 'true' when the instance, and its process is daemonic
	 */
	virtual bool daemonic() const noexcept;
	/* Sets the instance daemonic. Invoke this method after a process starts, before join() method.
	 * If the instance is daemonic, the thread will not wait for the process on delete.
	 * After instance deletion, the only way to join with that process is using join__() method.
	 *
	 * daemonic: 'true' means daemonic
	 *
	 * Throws WeakRune: When the instance is not running or detached
	 */
	virtual thisClass &daemonic(const bool daemonic = true) throw(WeakRune);

	/* Returns a string describing the instance.
	 *
	 * Return: A string that describes the instance
	 */
	virtual std::string toString() const noexcept;
};

} /* namespace ashe */

#endif /* DESCENDANTPROCESS_H_ */
