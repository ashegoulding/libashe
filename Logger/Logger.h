/*
*/
#ifndef ASHE_LOGGER_H_
#define ASHE_LOGGER_H_

#include <string>
#include <list>
#include <ostream>

namespace ashe
{

class Logger
{
public:
	typedef Logger thisClass;
	struct Output
	{
		std::ostream* stream;
		bool deleteWith;
		bool closeOnDelete;
	};
	typedef std::list<Output> OutputListType;

protected:
	bool timeInUTC;
	std::string timeFormat;
	OutputListType handleList;

public:
	
};

}

#endif
