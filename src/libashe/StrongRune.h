#ifndef ASHE_STRONGRUNE_H_
#define ASHE_STRONGRUNE_H_

#include <src/libashe/MFCLinkage.h>
#include <string>
#include <exception>

namespace ashe
{

class ASHE_DECL_EXT StrongRune : public std::exception
{
/*
* StrongRune: Exception that no one, nothing should catch.
* For Unrecoverable exception throwing.
*
* Alternative of assert()
*/
public:
	typedef std::exception motherClass;
	typedef StrongRune thisClass;

protected:
	std::string whatString;

	void __construct(const thisClass &src) ASHE_NOEXCEPT;

public:
	StrongRune(std::string msg = "") ASHE_NOEXCEPT;
	StrongRune(const StrongRune &src) ASHE_NOEXCEPT;
	virtual ~StrongRune() ASHE_NOEXCEPT;

	thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

	virtual const char *what() const ASHE_NOEXCEPT; //@Override, alias of getMsg
	virtual std::string getMsg() const ASHE_NOEXCEPT;
}; //class 'StrongRune'

} // namespace 'ashe'

#endif // STRONGRUNE_H_
