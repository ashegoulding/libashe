#include "Frelish.h"

#include <algorithm>
#include <locale>

namespace ashe
{

std::string trim__(const std::string& x) ASHE_NOEXCEPT
{
	std::string y = x;
	std::locale loc;

	while(y.begin() != y.end())
	{
		if(std::isspace(*y.begin(), loc))
			y.erase(y.begin());
		else
			break;
	}
	while(y.begin() != y.end() && y.end() - 1 != y.end())
	{
		if(std::isspace(*(y.end()-1), loc))
			y.erase(y.end()-1);
		else
			break;
	}

	return y;
}

std::vector<std::string> explodeSpace__(const std::string& x) ASHE_NOEXCEPT
{
	std::string arg = x;
	std::vector<std::string> y;
	std::string::size_type i, cnt;
	std::locale loc;
	while(true)
	{
		arg = trim__(arg);
		cnt = arg.length();
		for(i=0; i<cnt; ++i)
		{
			if(std::isspace(arg[i], loc))
			{
				y.push_back(arg.substr(0, i));
				arg.erase(arg.begin(), arg.begin()+i);
				break;
			}
		}
		if(i >= cnt)
		{
			if(! arg.empty())
				y.push_back(arg);
			break;
		}
	}
	return y;
}

std::vector<std::string> explode__(const std::string& x, const std::set<std::string>& delimiters) ASHE_NOEXCEPT
{
	std::string arg = x;
	std::vector<std::string> y;
	std::string::size_type i, cnt;

	auto trimDelimiters = [&arg, &delimiters]()
	{
		while(arg.begin() != arg.end())
		{
			if(delimiters.find(std::string(1, *arg.begin())) != delimiters.end())
				arg.erase(arg.begin());
			else
				break;
		}
		while(arg.begin() != arg.end() && arg.end() - 1 != arg.end())
		{
			if(delimiters.find(std::string(1, *(arg.end() - 1))) != delimiters.end())
				arg.erase(arg.end()-1);
			else
				break;
		}
	};

	while(true)
	{
		trimDelimiters();
		cnt = arg.length();
		for(i=0; i<cnt; ++i)
		{
			if(delimiters.find(std::string(1, arg[i])) != delimiters.end())
			{
				y.push_back(arg.substr(0, i));
				arg.erase(arg.begin(), arg.begin()+i);
				break;
			}
		}
		if(i >= cnt)
		{
			if(! arg.empty())
				y.push_back(arg);
			break;
		}
	}
	return y;
}

std::string toLower__(const std::string& x) ASHE_NOEXCEPT
{
	std::string y = x;
	std::transform(y.begin(), y.end(), y.begin(), ::tolower);

	return y;
}

std::string toUpper__(const std::string& x) ASHE_NOEXCEPT
{
	std::string y = x;
	std::transform(y.begin(), y.end(), y.begin(), ::toupper);

	return y;
}

extern std::string::size_type findStarting__(const std::string& x) ASHE_NOEXCEPT
{
	std::string::size_type y = 0;
	std::locale loc;
	for(const auto &v : x)
	{
		if(! std::isspace(v, loc))
			return y;
		++y;
	}
	return y;
}

}
