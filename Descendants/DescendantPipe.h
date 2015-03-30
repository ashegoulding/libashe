/*
* DescendantProcess.h
*
* Ashe's UUID utility with C++11
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
/* DescendantMessageQueue: Wrapper class of posix pipe
 */
public:
	typedef Icebank motherClass;
	typedef TransitiveInterface fatherClass;
	typedef DescendantPipe thisClass;

protected:
	int pipe[2];
	std::vector<unsigned char> __stash;

	void __construct(const thisClass &src) throw(WeakRune);
	virtual void __close() noexcept;
	virtual bool __isClosed() const noexcept;
	virtual void __checkValidity() const throw(WeakRune);

public:
	/*
	* blocking: Sets 'O_NONBLOCK' when false
	*
	* Throws WeakRune:
	** When pipe creation failure
	** When fails to change blocking mode
	*/
	DescendantPipe(const bool blocking = true) throw(WeakRune);
	/*
	* src: Instance to copy
	*
	* Throws WeakRune:
	** when fd duplication failure
	** when blocking mode setting failure
	** when buffer setting failure
	*/
	DescendantPipe(const thisClass &src) throw(WeakRune);
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
	*/
	thisClass &operator =(const thisClass &src) throw(WeakRune);
	/* Sets instance's status to 'detached'.
	* If the instance has detached, it does nothing on delete so another instance can continue to use.
	* For Handing fd over to another instance.
	* NOTE: Once detached, no way to recover it. Call wisely.
	*/
	virtual thisClass &detach() throw(WeakRune); //@Implement

	/* Feeds data into the pipe.
	* When blocking mode, it can block the thread until the write() completes.
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
	virtual thisClass &post(const void *data, const size_t len) throw(WeakRune); //@Implement
	virtual thisClass &post(const std::vector<unsigned char> &data) throw(WeakRune); //@Implement
	virtual thisClass &post(const std::vector<unsigned char> &data, const size_t len) throw(WeakRune); //@Implement
	virtual thisClass &post() throw(WeakRune); //@Implement
	/* Reads the pipe
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
	virtual thisClass &receive(void *data, const size_t len) throw(WeakRune); //@Implement
	virtual thisClass &receive(std::vector<unsigned char> &data) throw(WeakRune); //@Implement
	virtual thisClass &receive(std::vector<unsigned char> &data, const size_t len) throw(WeakRune); //@Implement

	virtual size_t bufferSize() const throw(WeakRune); //@Implement
	virtual thisClass &bufferSize(const size_t size) throw(WeakRune); //@Implement
	virtual bool blocking() const throw(WeakRune); //@Implement
	virtual thisClass &blocking(const bool blocking) throw(WeakRune); //@Implement

	virtual thisClass &descriptors(std::set<int> &y) throw(WeakRune); //@Implement

	virtual bool valid() const noexcept; //@Implement

	/* Returns a string describing the instance.
	*
	* Return: A string that describes the instance
	*/
	virtual std::string toString() const noexcept; //@Override
};

} /* namespace ashe */

#endif /* DESCENDANTMESSAGEQUEUE_H_ */
