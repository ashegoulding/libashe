#ifndef ASHE_FJORD_H_
#define ASHE_FJORD_H_

#include <src/libashe/MFCLinkage.h>
#include <string>

namespace ashe
{

class ASHE_DECL_EXT Fjord
{
/* Class that is in concrete and stable development state.
* Derive from this class if a class is no longer buggy.
*/
public:
	typedef Fjord thisClass;

protected:
	// Class name for toString() method support
	std::string className;

	void __construct(const thisClass &src) ASHE_NOEXCEPT;

public:
	Fjord() ASHE_NOEXCEPT;
	Fjord(const Fjord &src) ASHE_NOEXCEPT;
	virtual ~Fjord() ASHE_NOEXCEPT;

	virtual std::string toString() const ASHE_NOEXCEPT;
	virtual std::string getClassName() const ASHE_NOEXCEPT;
};

} /* namespace ashe */

#endif /* FJORD_H_ */
