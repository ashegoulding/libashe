/*
* DescendantPipe.h
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
*  - Wrapping class for posix anonymous pipe.
*/
#ifndef ASHE_DESCENDANTPIPE_H_
#define ASHE_DESCENDANTPIPE_H_

#include "Frelish.h"
#include "TransitiveInterface/TransitiveInterface.h"

namespace ashe
{

class DescendantPipe : public Icebank, public TransitiveInterface
{
/* Anonymous posix pipe supports non-blocking mode.
* @Usage:
*  - Construction
*  - post()
*  - retrieve()
*
* @NOTE
*  - On fork(), this class has no special behaviour. Just fork() and use.
*  - For IPC, two of this instance are needed for bi-directional communication, as it was.
*  - This class is merely a wrapping class for pipe()
*/
public:
	typedef Icebank motherClass;
	typedef TransitiveInterface fatherClass;
	typedef DescendantPipe thisClass;

protected:
	int pipe[2];
	/* stash memory: the instance buffer for non-block operation support.
	*
	* @NOTE
	*  - The instance appends data remaining to send next tick to this vector.
	*  - Before send(), the instance appends given data to send to this vector.
	*  - The sent data will be truncated within this vector.
	*/
	std::vector<unsigned char> __stash;

	/* Destructs the instance and copy constructs the instance.
	*
	* @Behaviour
	*  - When 'src' is detached instance, it merely copies from the given instance.
	*  - Otherwise, on the other hand, copies pipe descriptors from 'src' with dup()
	* @NOTE
	*  - Note that pipes are destroyed when all the descriptors representing the pipes are all closed.
	*/
	void __construct(const thisClass &src) throw(TransitiveRune);
	virtual void __close() noexcept;
	virtual bool __isClosed() const noexcept;
	virtual void __checkValidity() const throw(TransitiveRune);

public:
	/* Default constructor.
	*
	* @Behaviour
	*  - Constructs the instance in non-blocking mode when 'blocking' given false
	*/
	DescendantPipe(const bool blocking = true) throw(TransitiveRune);
	// Refer to __construct()
	DescendantPipe(const thisClass &src) throw(TransitiveRune);
	virtual ~DescendantPipe() noexcept;

	// Refer to __construct()
	thisClass &operator =(const thisClass &src) throw(TransitiveRune);
	/* Sets the instance to detached state.
	*
	* @NOTE
	*  - It is intended to save system resource.
	*  - Copy constructors do not duplicates descriptor of detached instance.
	*  - Use it when you wish to contain the instance inside of std containers (like std::list)
	* @Throws
	*  - When the instance is not valid: closed internally.
	*  - When the instance is already detached state.
	*/
	virtual thisClass &detach() throw(TransitiveRune); //@Implement

	/* Posts given data to the other side of the pipe.
	*
	* @Behaviour
	*  - Appends given data to __stash before actual sending.
	*  - Sends pending data first before the given data.
	*  - If the data has been completely sent, the instance shall become 'good' state (State bits cleared)
	* @NOTE
	*  - When non-blocking, be sure to empty the __stash, pending data first before further data to send with post()
	*   -- that is without any argument
	*  - When non-blocking, invoking this just with data given can cause excessive memory usage by __stash vector in worst case.
	* @Throws
	*  - When EAGAIN, the pipe buffer has no space and would block the calling thread (errno shall be set)
	*  - Internal error from platform (errno shall be set)
	*  - The instance is not valid so that the data cannot be sent (detached state or bug)
	*  - Given vector or memory range has no length.
	*  - The data was written but incompletely, delay has occurred.
	*/
	virtual thisClass &post(const void *data, const size_t len) throw(TransitiveRune); //@Implement
	virtual thisClass &post(const std::vector<unsigned char> &data) throw(TransitiveRune); //@Implement
	virtual thisClass &post(const std::vector<unsigned char> &data, const size_t len) throw(TransitiveRune); //@Implement
	/* Posts pending remaining data in __stash. It's for non-blocking mode support.
	* For more detail, refer to post() method with data arguments.
	*
	* @NOTE
	*  - This method has no affect if there's no data pending. It doesn't even throw anything.
	*  - When delay occurred even in this method, the system might be not in good shape.
	*   Assume data transfer rate and sleep for specific amount of time for the data in the pipe buffer to be sent,
	*   Which will save some system resource from going in chaos.
	* @Throws
	*  - Delay occurred again, leaves pending data remaining in __stash.
	*/
	virtual thisClass &post() throw(TransitiveRune); //@Implement
	/* Reads the pipe with given memory range.
	 *
	 * @Throws
	 *  - When EAGAIN, there's no data to read in the pipe's buffer and the calling thread would block (errno shall be set)
	 *  - Internal error from platform (errno shall be set)
	 *  - The instance is not valid so no data can be read (detached state or bug)
	 *  - Given vector or memory range has no length
	*/
	virtual thisClass &receive(void *data, const size_t len) throw(TransitiveRune); //@Implement
	/* Reads the pipe with given vector.
	* Refer to receive(void *data, const size_t len) for more detail.
	*
	* @NOTE
	*  - Reads amount of data as 'data' vector's size.
	*  - If it's empty, it will throw.
	*/
	virtual thisClass &receive(std::vector<unsigned char> &data) throw(TransitiveRune); //@Implement
	/* Reads the pipe with given vector, for 'len'
	* Refer to receive(void *data, const size_t len) for more detail.
	*
	* @NOTE
	*  - Reads amount of data as 'len'
	*  - It will throw if 'len' is zero or 'data' vector is shorter than 'len' that would have caused SEGFAULT.
	*/
	virtual thisClass &receive(std::vector<unsigned char> &data, const size_t len) throw(TransitiveRune); //@Implement

	/* Returns actual buffer size of the pipe.
	*
	* @NOTE
	*  - This method returns the actual buffer size of the pipe by calling ::fcntl() function.
	*  - Therefore if you have set buffer size to 0, it had likely set the actual size to 4096, which is minimum possible size for system.
	* @Throws
	*  - If internal error occurred (errno shall be set)
	*/
	virtual size_t bufferSize() const throw(TransitiveRune); //@Implement
	/* Sets buffer size of the pipe.
	*
	* @NOTE
	*  - This method sets the pipe's buffer size by calling ::fcntl() funciton.
	*  - The actual size that has been set could differ from intended size.
	*  - Therefore if you set buffer size to 0, it will likely set the actual size to 4096, which is minimum possible size for system.
	* @Throws
	*  - If internal error occurred (errno shall be set)
	*/
	virtual thisClass &bufferSize(const size_t size) throw(TransitiveRune); //@Implement
	// Tests if it's blocking mode.
	virtual bool blocking() const throw(TransitiveRune); //@Implement
	// Sets the blocking mode either blocking or non-blocking.
	virtual thisClass &blocking(const bool blocking) throw(TransitiveRune); //@Implement

	/* Intended for poll() or select() wrapping class.
	* Will clear and insert 2 descriptors of the pipe into given argument 'y'
	*
	* @NOTE
	*  - It returns the actual descriptors of the pipe that the instance has.
	*  - Manipulating them outside can cause the instance to fail.
	*/
	virtual thisClass &descriptors(std::set<int> &y) throw(TransitiveRune); //@Implement

	// Returns false: detached or closed
	virtual bool valid() const noexcept; //@Implement

	/* Returns a string describing the instance:
	* descriptors, blocking mode, buffer size, transfered size, detached state.
	*/
	virtual std::string toString() const noexcept; //@Override
};

} /* namespace ashe */

#endif /* DESCENDANTMESSAGEQUEUE_H_ */
