#ifndef ASHE_WEAKRUNE_H_
#define ASHE_WEAKRUNE_H_

#include <src/libashe/Fjord.h>
#include <string>
#include <exception>

namespace ashe
{

class ASHE_DECL_EXT WeakRune : public Fjord, public std::exception
{
/* WeakRune: Exception that is likely expected and recoverable
*/
public:
	typedef Fjord motherClass;
	typedef WeakRune thisClass;

protected:
	int errorNumber = 0;
	std::string whatString;

	void __construct(const thisClass &src) ASHE_NOEXCEPT;

public:
	WeakRune(std::string msg = "") ASHE_NOEXCEPT;
	WeakRune(const thisClass &src) ASHE_NOEXCEPT;
	virtual ~WeakRune() ASHE_NOEXCEPT;

	thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

	virtual std::string toString() const ASHE_NOEXCEPT;
	virtual const char *what() const ASHE_NOEXCEPT; //@Override, alias of getMsg

	virtual bool hasError() const ASHE_NOEXCEPT;
	virtual thisClass &setError() ASHE_NOEXCEPT;
	virtual int getErrorNumber() const ASHE_NOEXCEPT;
	virtual std::string getErrorString() const ASHE_NOEXCEPT;
}; // class 'WeakRune'

} // namespace 'ashe'

#endif
