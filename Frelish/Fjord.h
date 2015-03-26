#ifndef ASHE_FJORD_H_
#define ASHE_FJORD_H_

#include <string>

namespace ashe
{

class Fjord
{
/* Class that is in concrete and stable development state.
* Derive from this class if a class is no longer buggy.
*/
public:
	typedef Fjord thisClass;

protected:
	// Class name for toString() method support
	std::string className;

	void __construct(const thisClass &src) noexcept;

public:
	Fjord() noexcept;
	Fjord(const Fjord &src) noexcept;
	virtual ~Fjord() noexcept;

	virtual std::string toString() const noexcept;
	virtual std::string getClassName() const noexcept;
};

} /* namespace ashe */

#endif /* FJORD_H_ */
