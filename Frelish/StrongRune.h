#ifndef ASHE_STRONGRUNE_H_
#define ASHE_STRONGRUNE_H_

#include <string>
#include <exception>

namespace ashe
{

class StrongRune : public std::exception
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

	void __construct(const thisClass &src) noexcept;

public:
	StrongRune(std::string msg = "") noexcept;
	StrongRune(const StrongRune &src) noexcept;
	virtual ~StrongRune() noexcept;

	thisClass &operator =(const thisClass &src) noexcept;

	virtual const char *what() const noexcept; //@Override, alias of getMsg
	virtual std::string getMsg() const noexcept;
}; //class 'StrongRune'

} // namespace 'ashe'

#endif // STRONGRUNE_H_
