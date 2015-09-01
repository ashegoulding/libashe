#ifndef ASHE_ICEBANK_H_
#define ASHE_ICEBANK_H_

#include "Fjord.h"

namespace ashe
{

class ASHE_DECL_EXT Icebank : public Fjord
{
/* Class that is under developing.
* Derive from this class if a class is being made or buggy.
*/
public:
	typedef Fjord motherClass;
	typedef Icebank thisClass;

	Icebank() ASHE_NOEXCEPT;
	Icebank(const Icebank &src) ASHE_NOEXCEPT;
	virtual ~Icebank() ASHE_NOEXCEPT;
};

} /* namespace ashe */

#endif /* ASHE_ICEBANK_H_ */
