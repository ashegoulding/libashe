#ifndef ASHE_SECTIONBLOCK2D_H_
#define ASHE_SECTIONBLOCK2D_H_

#include "ManagedBinary.h"

namespace ashe
{

template <class T>
class SectionBlock2D : public ManagedBinary<T>
{
public:
	typedef ManagedBinary<T> motherClass;
	typedef SectionBlock2D<T> thisClass;

	size_t cntHorizental, cntVertical;

protected:
	void __construct(thisClass *src) throw()
	{
		motherClass::__construct(src);
		this->cntHorizental = src->cntHorizental;
		this->cntVertical = src->cntVertical;
	}

public:
	SectionBlock2D(const T h = 0, const T v = 0) throw()
		: motherClass((size_t)(h*v))
	{
		this->cntHorizental = h;
		this->cntVertical = v;
	}
	SectionBlock2D(thisClass *src) throw()
		: motherClass()
		, cntHorizental(0)
		, cntVertical(0)
	{
		this->__construct(src);
	}
	SectionBlock2D(thisClass &src) throw()
		: motherClass()
		, cntHorizental(0)
		, cntVertical(0)
	{
		this->__construct(&src);
	}
	virtual ~SectionBlock2D() throw()
	{
	}

	virtual thisClass &operator =(thisClass &src) throw()
	{
		this->__construct(&src);
		return *this;
	}
	virtual thisClass *operator =(thisClass *src) throw()
	{
		this->__construct(src);
		return this;
	}
	virtual inline T *operator [](const size_t y) throw()
	{
		return this->data + (y*this->cntHorizental);
	}

	virtual inline void setDimension(const size_t h, const size_t v) throw()
	{
		motherClass::setSize((size_t)(h*v));
		this->cntHorizental = h;
		this->cntVertical = v;
	}
	virtual void setSize(const size_t size) throw() //@Override. Regular square: [size] x [size]
	{
		this->setDimension(size, size);
	}
	virtual void resize(const size_t newSize, const bool forced/* = true */) throw()
	{
		motherClass::resize(newSize*newSize, forced);
		this->cntHorizental = newSize;
		this->cntVertical = newSize;
	}
	virtual void empty() throw()
	{
		motherClass::empty();
		this->cntVertical = this->cntHorizental = 0;
	}

	virtual void swap(thisClass &with) throw() //@Override
	{
		motherClass::swap(with);
		const size_t cntHorizental = this->cntHorizental, cntVertical = this->cntVertical;
		this->cntHorizental = with.cntHorizental;
		this->cntVertical = with.cntVertical;
		with.cntHorizental = cntHorizental;
		with.cntVertical = cntVertical;
	}
};

} // namespace 'ashe'

#endif // ASHE_SECTIONBLOCK2D_H_
