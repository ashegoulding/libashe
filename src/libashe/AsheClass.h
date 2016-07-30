/* AsheClass.h
*/
#ifndef LASHE_ASHECLASS_H_
#define LASHE_ASHECLASS_H_

#include "pub.h"

namespace ashe
{

#pragma pack(push, LASHE_PUB_ALIGN)

/* AsheClass
* The top-level class.
*/
class LASHE_DECL_EXT AsheClass
{
public:
	typedef AsheClass thisClass;

public:
	virtual ~AsheClass() LASHE_NOEXCEPT;

	virtual const char *className() const LASHE_NOEXCEPT;
};

#pragma pack(pop)

}

#endif
