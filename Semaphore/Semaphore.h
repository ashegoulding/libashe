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
public:
	typedef Icebank motherClass;
	typedef Semaphore<ArithType> thisClass;

	class Rune : public WeakRune
	{
	public:
		typedef WeakRune motherClass;
		typedef Rune thisClass;

		enum Code
		{
			C_NONE,
			C_ZERO_CONCURRENCY
		};

		static std::string codeToString__(const Code code) noexcept;

	protected:
		Code code = C_NONE;

		void __construct(const thisClass &src) noexcept;

	public:
		Rune(const Code code = C_NONE, const std::string msg = "") noexcept;
		Rune(const thisClass &src) noexcept;
		virtual ~Rune() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual Code getCode() const noexcept;
	};

	class Ticket : public Icebank
	{
	public:
		typedef Icebank motherClass;
		typedef Ticket thisClass;

	protected:
		Semaphore<ArithType> *mother = NULL;
		ArithType crement;

	public:
		Ticket(Semaphore<ArithType> &mother, const ArithType crement = (ArithType)1) throw(Rune);
		Ticket(const thisClass &src) throw(StrongRune);
		virtual ~Ticket() noexcept;

		virtual thisClass &operator =(const thisClass &src) throw(StrongRune);
		virtual thisClass &operator =(Semaphore<ArithType> &src) noexcept;

		virtual thisClass &dispose() noexcept;
	};

protected:
	std::mutex __concurrencyMtx, __ticketMtx;
	std::condition_variable __cv;
	ArithType traffic, concurrency;

	void __construct(const thisClass &src) noexcept;

public:
	Semaphore(const ArithType concurrency = (ArithType)1) noexcept;
	Semaphore(const thisClass &src) noexcept;
	virtual ~Semaphore() noexcept;

	virtual thisClass &operator =(const thisClass &src) noexcept;

	virtual thisClass &enter(const ArithType increment = (ArithType)1) throw(Rune);
	virtual thisClass &leave(const ArithType decrement = (ArithType)1) noexcept;
	virtual thisClass &setConcurrency(const ArithType x) noexcept;
	virtual ArithType getConcurrency() const noexcept;
	virtual ArithType getTraffic() const noexcept;
};

} /* namespace ashe */

namespace ashe
{

template<class ArithType>
Semaphore<ArithType>::Semaphore(const ArithType concurrency/* = (ArithType)1*/) noexcept
	: concurrency(concurrency)
{
	std::stringstream sb;
	sb << "Semaphore<" << typeid(ArithType).name() << ">";
	this->className = sb.str();

	::memset(&this->traffic, 0, sizeof(ArithType));
}

template<class ArithType>
Semaphore<ArithType>::Semaphore(const thisClass &src) noexcept
	: motherClass(src)
{
	this->__construct(src);
	::memset(&this->traffic, 0, sizeof(ArithType));
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

	return *this;
}

template<class ArithType>
void Semaphore<ArithType>::__construct(const thisClass& src) noexcept
{
	this->concurrency = src.concurrency;
}

template<class ArithType>
Semaphore<ArithType> &Semaphore<ArithType>::enter(const ArithType increment/* = (ArithType)1*/) throw(Rune)
{
	std::unique_lock<std::mutex> l(this->__concurrencyMtx);
	if(! this->concurrency)
		throw Rune(Rune::C_ZERO_CONCURRENCY, "Whilst entering");

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
Semaphore<ArithType>::Rune::Rune(const Code code/* = C_NONE*/, const std::string msg/* = ""*/) noexcept
{
	std::stringstream sb;
	sb << "Semaphore<" << typeid(ArithType).name() << ">::Rune";
	this->className = sb.str();

	sb.str("");
	sb.clear();
	sb << "[" << thisClass::codeToString__(code) << "] " << msg;
	this->whatString = sb.str();
}

template<class ArithType>
Semaphore<ArithType>::Rune::Rune(const thisClass& src) noexcept
	: motherClass(src)
{
	std::stringstream sb;
	sb << "Semaphore<" << typeid(ArithType).name() << ">::Rune";
	this->className = sb.str();

	this->__construct(src);
}

template<class ArithType>
Semaphore<ArithType>::Rune::~Rune() noexcept
{
}

template<class ArithType>
typename Semaphore<ArithType>::Rune &Semaphore<ArithType>::Rune::operator =(const thisClass& src) noexcept
{
	motherClass::__construct(src);
	this->__construct(src);

	return *this;
}

template<class ArithType>
void Semaphore<ArithType>::Rune::__construct(const thisClass& src) noexcept
{
	this->code = src.code;
}

template<class ArithType>
typename Semaphore<ArithType>::Rune::Code Semaphore<ArithType>::Rune::getCode() const noexcept
{
	return this->code;
}

template<class ArithType>
std::string Semaphore<ArithType>::Rune::codeToString__(const Code code) noexcept
{
	switch(code)
	{
	case C_ZERO_CONCURRENCY: return "Instance has 0 concurrency limit";
	}
	return "Undefined";
}

}

namespace ashe
{

template<class ArithType>
Semaphore<ArithType>::Ticket::Ticket(Semaphore<ArithType>& mother, const ArithType crement/* = (ArithType)1*/) throw(Rune)
	: mother(&mother)
	, crement(crement)
{
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
