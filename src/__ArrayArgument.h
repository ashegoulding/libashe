#ifndef LASHE_ARRAYARGUMENT_H_
#define LASHE_ARRAYARGUMENT_H_

#include "libashe/pub.h"
#include <map>
#include <vector>


namespace ashe
{
namespace __aa
{

template<class T>
struct ParseParam
{
	std::map<T, size_t> param;
};

template<class T>
struct ParseResultValue
{
	bool appeared = false;
	std::vector<T> list;
};

template<class T>
std::map<T, ParseResultValue<T>> parseArrayArgument__(T const *arr, const ParseParam<T> &param) LASHE_NOEXCEPT
{
	auto &p = param.param;
	std::map<T, ParseResultValue<T>> ret;
	T v;
	size_t i;

	if(nullptr == arr)
		return ret;
	while(*arr)
	{
		auto pIt = p.find(v);

		v = *(arr++);
		if(pIt != p.end())
		{
			auto &rv = ret[v];
			rv.appeared = true;
			for(i=0; i<pIt->second; ++i)
				rv.list.push_back(*(arr++));
		}
	}

	return ret;
}

}
}

#endif
