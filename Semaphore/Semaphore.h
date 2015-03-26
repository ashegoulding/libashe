/*
 * Semaphore.h
 *
 * C++11 Semaphore by Ashe
 * On UNIX, link with lpthread
 *
 * @Maintained
 *  Under development since: 2015 Q1
 * @Author
 *  Ashe David Sterkhus
 *  Blame to: ashe.goulding+blame@gmail.com
 * @COLOPHON
 *  This file is part of libashe, Ashe's C++11/98 utility stuff
 */
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "Frelish.h"

#include <typeinfo>
#include <mutex>
#include <condition_variable>
#include <sstream>

#include <cstring>

namespace ashe
{

template<class ArithType>
class Semaphore : public Icebank
{
/* Semaphore implementation, using C++11 features
*
* @NOTE
*  - 'ArithType' could be any type that has following arithmetic operations: +=, -=, =, <=
*  - None of the feature of this class is thread-safe.
*/
public:
	typedef Icebank motherClass;
	typedef Semaphore<ArithType> thisClass;

	class Ticket : public Icebank
	{
	/* C++11 RAII compliant
	*
	* @NOTE
	*  - Enters the section on construction, leaves it on destruction
	*  - One can explicitly, manually leave the section by 'dispose()' method
	* @REFER_TO
	*  - Usage tradition of std::unique_lock, std::lock_guard
	*/
	public:
		typedef Icebank motherClass;
		typedef Ticket thisClass;

	protected:
		// NULL is valid, that is when the instance has been disposed
		Semaphore<ArithType> *mother = NULL;
		ArithType crement; // Decrementing value on the destruction

	public:
		/* @Behaviour
		 *  - Enters the section with 'mother' and 'crement'
		 */
		Ticket(Semaphore<ArithType> &mother, const ArithType crement = (ArithType)1) noexcept;
		// Unintended code, makes the program killed when invoked, reminds the programmer of his or her fault.
		Ticket(const thisClass &src) throw(StrongRune);
		/* @Behaviour
		 *  - Invokes 'dispose()' methods, making the instance's 'mother' to leave the seciton.
		 */
		virtual ~Ticket() noexcept;

		// Unintended code, makes the program killed when invoked, reminds the programmer of his or her fault.
		virtual thisClass &operator =(const thisClass &src) throw(StrongRune);
		/* Alias of the constructor
		 *
		 * @Behaviour
		 *  - Disposes the instance itself ('dispose()' method)
		 *  - Then reconstructs itself with given Semaphore instance.
		 */
		virtual thisClass &operator =(Semaphore<ArithType> &src) noexcept;

		/* Leaves the section ('leave()' method of 'mother')
		 *
		 * @Behaviour
		 *  - Invokes 'leave()'
		 *  - Sets 'mother' member to NULL, hence the instance shall have no linkage with no 'mother'
		 *  - This method does nothing when it is already disposed.
		 */
		virtual thisClass &dispose() noexcept;
	};

protected:
	std::mutex __concurrencyMtx; // Handles the other member's data race
	std::condition_variable __cv;
	ArithType traffic, // Increasing value
		concurrency; // Limit value

	void __construct(const thisClass &src) noexcept;

public:
	/* Constructor
	 *
	 * @NOTE
	 *  - Using this class with default values (concurrency, increment, decrement ...)
	 *  - works as plane mutex which this class wasn't meant to do.
	 *  - Don't be a piss-taker. Give concurrency larger than 1
	 */
	Semaphore(const ArithType concurrency = (ArithType)1) noexcept;
	/* Copy-constructor
	 *
	 * @NOTE
	 *  - Copies only attributes.
	 *  - This constructor does not copy anything other than its attribute
	 *  - (Holding threads, the state of mutex, 'traffic' member)
	 */
	Semaphore(const thisClass &src) noexcept;
	/* Destructor
	 *
	 * @NOTE
	 *  - Deleting the instance with other threads running with it causes segfault.
	 *  - Join them before the destruction.
	 */
	virtual ~Semaphore() noexcept;

	// Reconstructor. Refer to the copy-constructor
	virtual thisClass &operator =(const thisClass &src) noexcept;

	virtual thisClass &enter(const ArithType increment = (ArithType)1) noexcept;
	virtual thisClass &leave(const ArithType decrement = (ArithType)1) noexcept;
	// Updates the concurrency run-time, making more blocked thread to be released.
	virtual thisClass &setConcurrency(const ArithType x) noexcept;
	virtual ArithType getConcurrency() const noexcept; // Not thread-safe
	virtual ArithType getTraffic() const noexcept; // Not thread-safe
};

} /* namespace ashe */

namespace ashe
{

template<class ArithType>
Semaphore<ArithType>::Semaphore(const ArithType concurrency/* = (ArithType)1*/) noexcept
	: concurrency(concurrency)
{
	std::stringstream sb;
	sb << "ashe::Semaphore<" << typeid(ArithType).name() << ">";
	this->className = sb.str();

	::memset(&this->traffic, 0, sizeof(ArithType));
}

template<class ArithType>
Semaphore<ArithType>::Semaphore(const thisClass &src) noexcept
	: motherClass(src)
{
	this->__construct(src);
	::memset(&this->traffic, 0, sizeof(ArithType));
	std::stringstream sb;
	sb << "ashe::Semaphore<" << typeid(ArithType).name() << ">";
	this->className = sb.str();
}

template<class ArithType>
Semaphore<ArithType>::~Semaphore() noexcept
{
}

template<class ArithType>
Semaphore<ArithType> &Semaphore<ArithType>::operator =(const thisClass &src) noexcept
{
	motherClass::__construct(src);
	this->__construct(src);
	::memset(&this->traffic, 0, sizeof(ArithType));

	std::stringstream sb;
	sb << "ashe::Semaphore<" << typeid(ArithType).name() << ">";
	this->className = sb.str();

	return *this;
}

template<class ArithType>
void Semaphore<ArithType>::__construct(const thisClass& src) noexcept
{
	this->concurrency = src.concurrency;
}

template<class ArithType>
Semaphore<ArithType> &Semaphore<ArithType>::enter(const ArithType increment/* = (ArithType)1*/) noexcept
{
	std::unique_lock<std::mutex> l(this->__concurrencyMtx);
	while(this->concurrency <= this->traffic)
		this->__cv.wait(l);
	this->traffic += increment;

	return *this;
}

template<class ArithType>
Semaphore<ArithType>& Semaphore<ArithType>::leave(const ArithType decrement/* = (ArithType)1*/) noexcept
{
	std::unique_lock<std::mutex> l(this->__concurrencyMtx);
	this->traffic -= decrement;
	this->__cv.notify_all();

	return *this;
}

template<class ArithType>
Semaphore<ArithType> &Semaphore<ArithType>::setConcurrency(const ArithType x) noexcept
{
	std::unique_lock<std::mutex> l(this->__concurrencyMtx);
	this->concurrency = x;
	this->__cv.notify_all();

	return *this;
}

template<class ArithType>
ArithType Semaphore<ArithType>::getConcurrency() const noexcept
{
	return this->concurrency;
}

template<class ArithType>
ArithType Semaphore<ArithType>::getTraffic() const noexcept
{
	return this->traffic;
}

}

namespace ashe
{

template<class ArithType>
Semaphore<ArithType>::Ticket::Ticket(Semaphore<ArithType>& mother, const ArithType crement/* = (ArithType)1*/) noexcept
	: mother(&mother)
	, crement(crement)
{
	std::stringstream sb;
	sb << "ashe::Semaphore<" << typeid(ArithType).name() << ">::Ticket";
	this->className = sb.str();

	mother.enter(crement);
}

template<class ArithType>
Semaphore<ArithType>::Ticket::Ticket(const thisClass& src) throw(StrongRune)
{
	throw StrongRune("Not intended usage! Ticket cannot be copy-constructed");
}

template<class ArithType>
Semaphore<ArithType>::Ticket::~Ticket() noexcept
{
	this->dispose();
}

template<class ArithType>
typename Semaphore<ArithType>::Ticket &Semaphore<ArithType>::Ticket::operator =(const thisClass& src) throw(StrongRune)
{
	throw StrongRune("Not intended usage! Ticket cannot be copied");
	return *this;
}

template<class ArithType>
typename Semaphore<ArithType>::Ticket &Semaphore<ArithType>::Ticket::operator =(Semaphore<ArithType>& src) noexcept
{
	motherClass::__construct(src);
	this->dispose().mother = &src;
	this->mother->enter();
	this->crement = (ArithType)1;

	std::stringstream sb;
	sb << "ashe::Semaphore<" << typeid(ArithType).name() << ">::Ticket";
	this->className = sb.str();

	return *this;
}

template<class ArithType>
typename Semaphore<ArithType>::Ticket &Semaphore<ArithType>::Ticket::dispose() noexcept
{
	if(this->mother)
	{
		this->mother->leave(this->crement);
		this->mother = NULL;
	}
	return *this;
}

}

#endif /* SEMAPHORE_H_ */
