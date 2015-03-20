#ifndef ASHE_DESCENDANTPIPE_H_
#define ASHE_DESCENDANTPIPE_H_

#include "Frelish.h"

#include <unistd.h>
#include <fcntl.h>

#include <vector>

namespace ashe
{

/* DescendantMessageQueue: Wrapper class of posix pipe
 */
class DescendantPipe : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef DescendantPipe thisClass;

	int pipe[2];

protected:
	// Instance buffer. For vector read() support.
	std::vector<unsigned char> __instanceBuffer;
	bool detached = false;

	void __construct(const thisClass &src) throw(WeakRune, StrongRune);
	virtual void __close() noexcept;
	virtual bool __isClosed() const noexcept;
	virtual void __checkValidity() const throw(WeakRune);
	virtual void __setInstanceBufferSize(const size_t size) throw(StrongRune);

public:
	/*
	 * blocking: Sets 'O_NONBLOCK' when false
	 *
	 * Throws WeakRune:
	 ** When pipe creation failure
	 ** When fails to change blocking mode
	 *
	 * Throws StrongRune: Rare case, instance buffer initialisation failure, blocking mode setting failure
	 */
	DescendantPipe(const bool blocking = true) throw(WeakRune, StrongRune);
	/*
	 * src: Instance to copy
	 *
	 * Throws WeakRune:
	 ** when fd duplication failure
	 ** when blocking mode setting failure
	 ** when buffer setting failure
	 *
	 * Throws StrongRune: Rare case, instance buffer initialisation failure, blocking mode setting failure
	 */
	DescendantPipe(const thisClass &src) throw(WeakRune, StrongRune);
	virtual ~DescendantPipe() noexcept;

	/* replaces instance with 'src'.
	 * The original instance shall be deleted.
	 *
	 * src: Instance to copy
	 *
	 * Throws WeakRune:
	 ** when fd duplication failure
	 ** when blocking mode setting failure
	 ** when buffer setting failure
	 *
	 * Throws StrongRune: Instance buffer initialisation failure
	 */
	thisClass &operator =(const thisClass &src) throw(WeakRune, StrongRune);
	/* Sets instance's status to 'detached'.
	 * If the instance has detached, it does nothing on delete so another instance can continue to use.
	 * For Handing fd over to another instance.
	 * NOTE: Once detached, no way to recover it. Call wisely.
	 */
	virtual thisClass &detach() noexcept;
	/* Tests if the instance has detached.
	 *
	 * Return: 'true' if the instance has detached.
	 */
	virtual bool hasDetached() const noexcept;

	/* Feeds memory range into the pipe.
	 * When blocking mode, it can block the thread until the write() completes.
	 *
	 * size: Size of the range
	 * range: A pointer to the range
	 *
	 * Return: Successfully written bytes.
	 ** It can be less than the argument 'size' when non-blocking mode.
	 ** In this case, wait and re-invoke this method to completely write remaining data.
	 ** (select() or poll() ...)
	 *
	 * Throws WeakRune:
	 ** when the instance is not valid
	 ** when OS-defined error: EAGAIN, EWOULDBLOCK, EPIPE ...
	 */
	virtual size_t write(const size_t size, const void *range) throw(WeakRune);
	/* Feeds a vector into the pipe (vector support)
	 * When blocking mode, it can block the thread until the write() completes.
	 *
	 * vector: A vector to feed
	 *
	 * Return: Successfully written bytes.
	 ** It can be less than the argument 'size' when non-blocking mode.
	 ** In this case, wait and re-invoke this method to completely write remaining data.
	 ** (select() or poll() ...)
	 *
	 * Throws WeakRune:
	 ** when the instance is not valid
	 ** when OS-defined error: EAGAIN, EWOULDBLOCK, EPIPE ...
	 */
	virtual size_t write(const std::vector<unsigned char> &vector) throw(WeakRune);
	/* Reads pipe with a memory range.
	 * When blocking mode, it can block the thread until the read() completes.
	 *
	 * size: Size of the range
	 * range: A pointer to the range
	 *
	 * Return: Successfully read bytes.
	 *
	 * Throws WeakRune:
	 ** when the instance is not valid
	 ** when OS-defined error: EAGAIN, EWOULDBLOCK, EPIPE ...
	 */
	virtual size_t read(const size_t size, void *range) throw(WeakRune);
	/* Reads pipe with a vector.
	 * When blocking mode, it can block the thread until the read() completes.
	 *
	 * vector: A vector to read with
	 *
	 * Return: Successfully read bytes.
	 *
	 * Throws WeakRune:
	 ** when the instance is not valid
	 ** when OS-defined error: EAGAIN, EWOULDBLOCK, EPIPE ...
	 */
	virtual size_t read(std::vector<unsigned char> &vector) throw(WeakRune);

	/* Returns buffer size assigned to the pipe.
	 *
	 * Return: In bytes, Buffer size of the pipe
	 *
	 * Throws WeakRune: Rare case, OS-defined error whilst querying the buffer size
	 */
	virtual size_t getBufferSize() const throw(WeakRune);
	// Actual buffer size is different. Setting 0 will result in setting real buffer size to 4096 in linux.
	/* Sets the buffer size of the pipe.
	 * The actual buffer size will not be exactly set as requested due to OS implementation.
	 * handing 'size' as 0 will not cause an exception, but only sets the buffer to minimum size.
	 *
	 * size: Requesting size to set
	 *
	 * Throws StrongRune: Rare case, instance buffer initialisation failure
	 * Throws WeakRune: Rare case, OS-defined error
	 */
	virtual thisClass &setBufferSize(const size_t size) throw(WeakRune, StrongRune);
	/* Tests blocking mode flag.
	 *
	 * Return: 'true' when the pipe is blocking mode
	 *
	 * Throws WeakRune: OS-defined error whilst querying the mode.
	 */
	virtual bool isBlocking() const throw(WeakRune);
	/* Sets blocking mode.
	 *
	 * blocking: Mode type. 'true' means blocking, 'false' means non-blocking
	 *
	 * Throws StrongRune: Rare case, OS-defined error whilst setting the mode
	 */
	virtual thisClass &setBlocking(const bool blocking = true) throw(StrongRune);

	/* Returns a string describing the instance.
	 *
	 * Return: A string that describes the instance
	 */
	virtual std::string toString() const noexcept;
};

} /* namespace ashe */

#endif /* DESCENDANTMESSAGEQUEUE_H_ */
