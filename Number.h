/*
 * Number.h
 *
 *  Created on: 2014. 10. 20.
 *      Author: david
 */

#ifndef ASHE_NUMBER_H_
#define ASHE_NUMBER_H_

#include <string>
#include <sstream>
#include <cstring>

namespace ashe
{

template <class T>
class Number
{
public:
	typedef Number<T> thisClass;

protected:
	void __construct(const thisClass *src) throw();

public:
	T number;

	static T fromString(std::string &x) throw();
	static std::string toString(T x) throw();
	static void swap(T &a, T &b) throw();

	Number() throw();
	Number(const T x) throw();
	Number(std::string x) throw();
	Number(const thisClass &src) throw();
	Number(const thisClass *src) throw();
	virtual ~Number() throw();

	virtual thisClass &operator =(const thisClass &src) throw();
	virtual thisClass *operator =(const thisClass *src) throw();
	virtual thisClass &operator =(const T x) throw();
	virtual inline T &operator *() throw();

	virtual std::string toString() throw();

	virtual void swap(T &with) throw();
	virtual void swap(thisClass &with) throw();
};

template<class T>
Number<T>::Number() throw ()
{
	::memset(&this->number, 0, sizeof(T));
}

template<class T>
Number<T>::Number(const T x) throw ()
	: number(x)
{
}

template<class T>
Number<T>::Number(std::string x) throw ()
	: number(thisClass::fromString(x))
{
}

template<class T>
Number<T>::Number(const thisClass& src) throw ()
{
	this->__construct(&src);
}

template<class T>
Number<T>::Number(const thisClass* src) throw ()
{
	this->__construct(src);
}

template<class T>
void Number<T>::__construct(const thisClass* src) throw ()
{
	this->number = src->number;
}

template<class T>
Number<T>::~Number() throw ()
{
}

template<class T>
Number<T>& ashe::Number<T>::operator =(const thisClass& src) throw ()
{
	this->__construct(&src);
	return *this;
}

template<class T>
Number<T>* ashe::Number<T>::operator =(const thisClass* src) throw ()
{
	this->__construct(src);
	return this;
}

template<class T>
Number<T> &ashe::Number<T>::operator =(const T x) throw ()
{
	this->number = x;
	return *this;
}

template<class T>
T& Number<T>::operator *() throw ()
{
	return this->number;
}

template<class T>
T Number<T>::fromString(std::string& x) throw ()
{
	std::stringstream sb;
	T y;
	memset(&y, 0, sizeof(T));
	sb << x;
	sb >> y;
	return y;
}

template<class T>
std::string Number<T>::toString(T x) throw ()
{
	std::stringstream sb;
	sb << x;
	return sb.str();
}

template<class T>
inline void Number<T>::swap(T& a, T& b) throw ()
{
	T tmp = b;
	b = a;
	a = tmp;
}

template<class T>
inline void Number<T>::swap(T& with) throw ()
{
	T tmp = with;
	with = this->number;
	this->number = tmp;
}

template<class T>
inline void Number<T>::swap(thisClass& with) throw ()
{
	T tmp = with.number;
	with = this->number;
	this->number = tmp;
}


template<class T>
std::string Number<T>::toString() throw ()
{
	return thisClass::toString(this->number);
}

} /* namespace 'ashe' */
#endif /* ASHE_NUMBER_H_ */
