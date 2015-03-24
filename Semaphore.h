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

namespace ashe
{

template<class IntType>
class Semaphore : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef Semaphore<IntType> thisClass;

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
		Semaphore<IntType> *mother = NULL;

		void __construct(const thisClass &src) noexcept;

	public:
		Ticket(Semaphore<IntType> &mother) noexcept;
		Ticket(const thisClass &src) noexcept;
		virtual ~Ticket() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;
		virtual thisClass &operator =(Semaphore<IntType> &src) noexcept;

		virtual thisClass &dispose() noexcept;
	};

protected:
	std::mutex __concurrencyMtx, __ticketMtx;
	IntType traffic, concurrent;

	void __construct(const thisClass &src) noexcept;

public:
	Semaphore(const IntType concurrency = 1) noexcept;
	Semaphore(const thisClass &src) noexcept;
	virtual ~Semaphore() noexcept;

	virtual thisClass &operator =(const thisClass &src) noexcept;

	virtual thisClass &enter() noexcept;
	virtual thisClass &leave() noexcept;
};

} /* namespace ashe */

namespace ashe
{

template<class IntType>
Semaphore<IntType>::Semaphore(const IntType concurrency) noexcept
{
}

template<class IntType>
Semaphore<IntType>::Semaphore(const thisClass &src) noexcept
{
}

template<class IntType>
Semaphore<IntType>::~Semaphore() noexcept
{
}

template<class IntType>
Semaphore<IntType> &Semaphore<IntType>::operator =(const thisClass &src) noexcept
{
}

template<class IntType>
void Semaphore<IntType>::__construct(const thisClass& src) noexcept
{
}

template<class IntType>
Semaphore<IntType>& Semaphore<IntType>::enter() noexcept
{
}

template<class IntType>
Semaphore<IntType>& Semaphore<IntType>::leave() noexcept
{
}

}

namespace ashe
{

template<class IntType>
Semaphore<IntType>::Rune::Rune(const Code code/* = C_NONE*/, const std::string msg/* = ""*/) noexcept
{
	std::stringstream sb;
	sb << "Semaphore<" << typeid(IntType).name() << ">::Rune";
	this->className = sb.str();

	sb.str("");
	sb.clear();
	sb << "[" << thisClass::codeToString__(code) << "] " << msg;
	this->whatString = sb.str();
}

template<class IntType>
Semaphore<IntType>::Rune::Rune(const thisClass& src) noexcept
	: motherClass(src)
{
	std::stringstream sb;
	sb << "Semaphore<" << typeid(IntType).name() << ">::Rune";
	this->className = sb.str();

	this->__construct(src);
}

template<class IntType>
Semaphore<IntType>::Rune::~Rune() noexcept
{
}

template<class IntType>
Semaphore<IntType>::Rune &Semaphore<IntType>::Rune::operator =(const thisClass& src) noexcept
{
	motherClass::__construct(src);
	this->__construct(src);
}

template<class IntType>
void Semaphore<IntType>::Rune::__construct(const thisClass& src) noexcept
{
	this->code = src.code;
}

template<class IntType>
Semaphore<IntType>::Rune::Code Semaphore<IntType>::Rune::getCode() const noexcept
{
	return this->code;
}

template<class IntType>
std::string Semaphore<IntType>::Rune::codeToString__(const Code code) noexcept
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

template<class IntType>
Semaphore<IntType>::Ticket::Ticket(Semaphore<IntType>& mother) noexcept
{
}

template<class IntType>
Semaphore<IntType>::Ticket::Ticket(const thisClass& src) noexcept
{
}

template<class IntType>
Semaphore<IntType>::Ticket::~Ticket() noexcept
{
}

template<class IntType>
Semaphore<IntType>::Ticket &Semaphore<IntType>::Ticket::operator =(const thisClass& src) noexcept
{
}

template<class IntType>
Semaphore<IntType>::Ticket &Semaphore<IntType>::Ticket::operator =(Semaphore<IntType>& src) noexcept
{
}

template<class IntType>
void Semaphore<IntType>::Ticket::__construct(const thisClass& src) noexcept
{
}

template<class IntType>
Semaphore<IntType>::Ticket &Semaphore<IntType>::Ticket::dispose() noexcept
{
}

}

#endif /* SEMAPHORE_H_ */
