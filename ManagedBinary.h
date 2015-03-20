/*
 * ManagedBinary.h
 *
 * Ashe's ManagedBinary with C++98
 *
 * @Maintained
 *  2015 Q1
 * @Author
 *  Ashe David Sterkhus
 *  Blame to: ashe.goulding+blame@gmail.com
 * @COLOPHON
 *  This file is part of libashe, Ashe's C++11/98 utility stuff
 */
#ifndef ASHE_MANAGEDBINARY_H_
#define ASHE_MANAGEDBINARY_H_

#include <cstring>
#include <string>
#include <sstream>
#include <typeinfo>

#include "Frelish.h"

namespace ashe
{
/* ManagedBinary, dynamic memory range manager
 * This implementation is for C++98 use.
 * For C++11, use std::vector<T>::data()
 */
template <class T>
class ManagedBinary : public Fjord
{
public:
	typedef ManagedBinary<T> thisClass;

	T *data; // Pointer to range
	size_t size; // In element quantity, not byte: Content, using size
	size_t realSize; // In element quantity, not byte: Real size allcated to thisClass::data pointer. Used by unforced thisClass::resize()
	bool clearUpOnDelete; // True: deleting thisClass::data on instance destruction. For when handing over the pointer to outside of the instance

protected:
	void __construct(const thisClass *src) throw();

public:
	ManagedBinary(const size_t size = 0) throw();
	// If 'arr' is set and size is 0: 'arr' will be treated as null terminated c-style string.
	// If both 'arr' and 'size' are set: 'arr' will be treated as byte range and the instance shall be constructed based on 'arr'
	ManagedBinary(const T *arr, const size_t size) throw();
	// Constructs the instance based on the given instance
	// Except that it will not clone 'data' pointer address but allocates new data range and copies the content to new instance
	ManagedBinary(const thisClass &src) throw();
	// Checks thisClass::clearUpOnDelete to see if the instance should delete its 'arr' on destruction
	virtual ~ManagedBinary() throw();

	// Copies the whole content of the given instance to the instance it self
	// Except that it will not clone 'data' pointer address but allocates new data range and copies the content to new instance
	virtual thisClass &operator =(const thisClass &src) throw();
	virtual thisClass *operator =(const thisClass *src) throw();

	// Sets the size. This will drop the content in thisClass::data
	// Newly allocated data range shall be zero-filled after this method invoked
	virtual thisClass &setSize(const size_t size) throw();
	// Resizes the size. Whatever the 'newSize' is, the method will try to maintain the content.
	// Shorter range: cut, Larger: original content with zero-filled range of newly grown.
	virtual thisClass &resize(const size_t newSize, const bool forced = true) throw();
	// Zero-fills the data much as the size, not the realSize. Invokes ::memset()
	inline virtual thisClass &fillZeros() throw();
	// Fills the data range with the given element
	// Note that this method is slower than method: fillBytes()
	// And if you just want to zero-fill the range, invoke fillZeros() method which is fastest one
	inline virtual thisClass &fill(const T x) throw();
	// Fills the data range with the given byte
	// Note that this method is faster than method: fill(const T)
	// This method is equivalent to: memset(thisClass::data, x, thisClass::size);
	inline virtual thisClass &fillBytes(const unsigned char x) throw();
	// Clears up the instance
	// The data will be deleted and the noth 'size' and 'realSize' members will be set to 0
	inline virtual thisClass &empty() throw();
	// Swaps the instance with given instance
	// Purpose was on the inside of the method but you can use it from outside too
	// (Like swap methods in C++ Standard Library)
	inline virtual thisClass &swap(thisClass &with) throw();

	// Returns instance representative string which contains address of 'this' pointer, 'size', 'realSize' and 'clearUpOnDelete'
	// If the argument 'y' is set, it will also returns the string with 'y' in c-style string
	virtual std::string toString() const throw();
};

template<class T>
ManagedBinary<T>::ManagedBinary(const size_t size) throw ()
	: data(NULL)
	, size(0)
	, realSize(0)
	, clearUpOnDelete(true)
{
	this->className = "ManagedBinary<" + std::string(typeid(T).name()) + ">";
	if(size)
		this->setSize(size);
}

template<class T>
ManagedBinary<T>::ManagedBinary(const T* arr, const size_t size) throw ()
	: data(NULL)
	, size(0)
	, realSize(0)
	, clearUpOnDelete(true)
{
	this->className = "ManagedBinary<" + std::string(typeid(T).name()) + ">";
	if(arr)
	{
		size_t copySize; // In bytes
		if(size)
			copySize = size*sizeof(T);
		else
			copySize = strlen(arr); // Possible of access violation on wrong argument
		this->setSize(copySize);
		memcpy(this->data, arr, copySize);
	}
	// Does nothing even if 'size' is set anyway
}

template<class T>
ManagedBinary<T>::ManagedBinary(const thisClass& src) throw ()
	: data(NULL)
	, size(0)
	, realSize(0)
	, clearUpOnDelete(true)
{
	this->className = "ManagedBinary<" + std::string(typeid(T).name()) + ">";
	this->__construct(&src);
}

template<class T>
ManagedBinary<T>::~ManagedBinary() throw ()
{
	// thisClass::data leaks if thisClass::clearUpOnDelete is false!
	// Use this feature wisely, or consider using copy-constructing feature of this class.
	// This instance is clonable but do not use memcpy() like it is a structure instance.
	if(this->clearUpOnDelete)
		this->empty();
}

template<class T>
ManagedBinary<T>& ManagedBinary<T>::operator =(const thisClass& src) throw ()
{
	this->__construct(&src);
	return *this;
}

template<class T>
ManagedBinary<T>* ManagedBinary<T>::operator =(const thisClass* src) throw ()
{
	this->__construct(src);
	return this;
}

template<class T>
void ManagedBinary<T>::__construct(const thisClass* src) throw ()
{
	this->setSize(src->size);
	if(src->data)
		memcpy(this->data, src->data, sizeof(T)*this->size);
	this->clearUpOnDelete = src->clearUpOnDelete;
}

template<class T>
ManagedBinary<T> &ManagedBinary<T>::setSize(const size_t size) throw ()
{
	if(this->realSize == size)
	{
		// Case: No change
		this->size = size;
		this->fillZeros();
		return *this;
	}
	this->empty();
	if(size)
	{
		this->realSize = this->size = size;
		this->data = new T[this->realSize];
		this->fillZeros();
	}

	return *this;
}

template<class T>
ManagedBinary<T> &ManagedBinary<T>::resize(const size_t newSize, const bool forced) throw ()
{
	if(this->realSize >= newSize && (! forced))
	{
		// Case: Not necessary to grow
		this->size = newSize;
		return *this;
	}
	// Case: Need to grow
	// TODO: Grow as requested.
	thisClass newOne(newSize);
	if(this->size && newOne.size)
		memcpy(newOne.data, this->data, sizeof(T)*(newOne.size < this->size? newOne.size : this->size));
	this->clearUpOnDelete = true;
	this->swap(newOne);

	return *this;
}

template<class T>
ManagedBinary<T> &ManagedBinary<T>::fillZeros() throw ()
{
	memset(this->data, 0, sizeof(T)*this->size);
	return *this;
}

template<class T>
inline ManagedBinary<T> &ManagedBinary<T>::empty() throw ()
{
	if(this->data || this->realSize)
	{
		delete[] this->data;
		this->data = NULL;
		this->realSize = this->size = 0;
	}

	return *this;
}

template<class T>
inline ManagedBinary<T> &ManagedBinary<T>::swap(thisClass& with) throw ()
{
	thisClass dummyOne;
	dummyOne.size = with.size;
	dummyOne.realSize = with.realSize;
	dummyOne.data = with.data;
	with.size = this->size;
	with.realSize = this->realSize;
	with.data = this->data;
	this->size = dummyOne.size;
	this->realSize = dummyOne.realSize;
	this->data = dummyOne.data;
	dummyOne.clearUpOnDelete = false;

	return *this;
}

template<class T>
inline ManagedBinary<T> &ManagedBinary<T>::fill(const T x) throw ()
{
	T *p = this->data;
	const T *end = p + this->size;
	while(p < end)
		*(p++) = x;

	return *this;
}

template<class T>
inline ManagedBinary<T> &ManagedBinary<T>::fillBytes(const unsigned char x) throw ()
{
	memset(this->data, x, sizeof(T)*this->size);
	return *this;
}

template<class T>
std::string ManagedBinary<T>::toString() const throw ()
{
	std::stringstream sb;
	sb << "[ManagedBinary<" << typeid(T).name() << "> @ " << std::hex << this << std::dec << "]\n";
	sb << "size: " << this->size << ", " << this->size*sizeof(T) << (this->size > 1? "bytes" : "byte");
	if(this->size == this->realSize)
		sb << "( == realSize)";
	else
		sb << "\nrealSize: " << this->realSize << ", " << this->realSize*sizeof(T) << (this->realSize > 1? "bytes" : "byte");
	if(! this->clearUpOnDelete)
		sb << "\nWill not clear up on delete.";

	return sb.str();
}

} // namespace 'ashe'

#endif // ASHE_MANAGEDBINARY_H_
