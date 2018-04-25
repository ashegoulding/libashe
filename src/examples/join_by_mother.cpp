#include "Descendants/DescendantProcess.h"

class MyChild : public ashe::DescendantProcess
{
protected:
	virtual int main()
	{
		// TODO: Count to 2^32 and return
		volatile unsigned int i;
		for(i=0; i<0xFFFFFFFF; ++i);
		return 0;
	}
};

int main()
{
	MyChild child;
	child.start().join();
	return 0;
}
