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

#include <atomic>
#include <condition_variable>

namespace ashe
{

template<class T>
class Semaphore : public Icebank
{
public:
	typedef Icebank motherClass;
	typedef Semaphore thisClass;

protected:
	std::atomic<T> atom;
	std::condition_variable __cv;

	void __construct(const thisClass &src) noexcept;

public:
	Semaphore() noexcept;
	Semaphore(const T x) noexcept;
	virtual ~Semaphore() noexcept;

	virtual thisClass &operator =(const thisClass & src) noexcept;
	virtual thisClass &operator =(const T & src) noexcept;
	virtual T operator T () noexcept;

	virtual bool operator ==(const T &x) noexcept;
	virtual bool operator !=(const T &x) noexcept;
	virtual bool operator <=(const T &x) noexcept;
	virtual bool operator >=(const T &x) noexcept;
	virtual bool operator <(const T &x) noexcept;
	virtual bool operator >(const T &x) noexcept;

	virtual thisClass &watch() noexcept;
	virtual T get() noexcept;
};

} /* namespace ashe */

namespace ashe
{

template<class T>
Semaphore<T>::Semaphore() noexcept
{
}

template<class T>
Semaphore<T>::Semaphore(const T x) noexcept
{
}

template<class T>
Semaphore<T>::~Semaphore() noexcept
{
}

template<class T>
void Semaphore<T>::__construct(const thisClass& src) noexcept
{
}

template<class T>
T Semaphore<T>::get() noexcept
{
}

template<class T>
Semaphore<T>& Semaphore<T>::watch() noexcept
{
}

template<class T>
Semaphore<T>& Semaphore<T>::operator =(const thisClass& src) noexcept
{
}

template<class T>
Semaphore<T>& Semaphore<T>::operator =(const T& src) noexcept
{
}

template<class T>
T Semaphore<T>::operator T() noexcept
{
}

template<class T>
bool Semaphore<T>::operator ==(const T& x) noexcept
{
}

template<class T>
bool Semaphore<T>::operator !=(const T& x) noexcept
{
}

template<class T>
bool Semaphore<T>::operator <=(const T& x) noexcept
{
}

template<class T>
bool Semaphore<T>::operator >=(const T& x) noexcept
{
}

template<class T>
bool Semaphore<T>::operator <(const T& x) noexcept
{
}

template<class T>
bool Semaphore<T>::operator >(const T& x) noexcept
{
}

}

#endif /* SEMAPHORE_H_ */
