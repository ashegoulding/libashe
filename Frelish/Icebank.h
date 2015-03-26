#ifndef ASHE_ICEBANK_H_
#define ASHE_ICEBANK_H_

#include "Fjord.h"

namespace ashe
{

class Icebank : public Fjord
{
/* Class that is under developing.
* Derive from this class if a class is being made or buggy.
*/
public:
	typedef Fjord motherClass;
	typedef Icebank thisClass;

	Icebank() noexcept;
	Icebank(const Icebank &src) noexcept;
	virtual ~Icebank() noexcept;
};

} /* namespace ashe */

#endif /* ASHE_ICEBANK_H_ */
