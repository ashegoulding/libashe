#ifndef ASHE_WEAKRUNE_H_
#define ASHE_WEAKRUNE_H_

#include "Fjord.h"

#include <string>
#include <sstream>
#include <exception>

#include <errno.h>
#include <cstring>
#include <cstdio>

namespace ashe
{

/*
 * WeakRune: Exception that is intended or recoverable
 */
class WeakRune : public Fjord
{
public:
	typedef Fjord motherClass;
	typedef WeakRune thisClass;

protected:
	int errorNumber = 0;
	std::string whatString;

	void __construct(const thisClass &src) noexcept;

public:
	WeakRune(std::string msg = "") noexcept;
	WeakRune(const thisClass &src) noexcept;
	virtual ~WeakRune() noexcept;

	thisClass &operator =(const thisClass &src) noexcept;

	virtual const char *what() const noexcept; //@Override, alias of getMsg
	virtual std::string getMsg() const noexcept;

	virtual bool hasError() const noexcept;
	virtual thisClass &setError() noexcept;
	virtual int getErrorNumber() const noexcept;
	virtual std::string getErrorString() const noexcept;
}; // class 'WeakRune'

} // namespace 'ashe'

#endif
