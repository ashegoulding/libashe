#ifndef LASHE_STRINGUTILS_HPP_
#define LASHE_STRINGUTILS_HPP_
#include "pub.hpp"

#include <algorithm>
#include <locale>
#include <string>


namespace ashe {

template <class CharT>
std::basic_string<CharT> trim(const CharT *str,
                              const std::locale &lc) LASHE_NOEXCEPT
{
    size_t i;
    const size_t len = std::char_traits<CharT>::length(str);
    const CharT *start, *end;

    // Find start.
    for (i = 0; i < len && std::isspace(str[i], lc); i += 1)
        ;
    start = str + i;

    // Find end.
    for (i = 0; i < len && std::isspace(str[len - i - 1], lc); i += 1)
        ;
    end = std::max(start, str + (len - i));

    return std::basic_string<CharT>(start, end);
}
template <class CharT>
std::basic_string<CharT> trim(const std::basic_string<CharT> &str,
                              const std::locale &lc) LASHE_NOEXCEPT
{
    return trim(str.c_str(), lc);
}
template <class CharT>
std::basic_string<CharT> trim(const CharT *str) LASHE_NOEXCEPT
{
    std::locale lc;
    return trim(str, lc);
}
template <class CharT>
std::basic_string<CharT>
trim(const std::basic_string<CharT> &str) LASHE_NOEXCEPT
{
    std::locale lc;
    return trim(str.c_str(), lc);
}

template <class CharT>
std::basic_string<CharT> lower(const CharT *str,
                               const std::locale &lc) LASHE_NOEXCEPT
{
    size_t i = 0;
    std::basic_string<CharT> ret(std::char_traits<CharT>::length(str), 0);

    std::for_each(ret.begin(), ret.end(), [&i, &lc, &str](CharT &c) {
        c = std::tolower(str[i], lc);
        i += 1;
    });

    return ret;
}
template <class CharT>
std::basic_string<CharT> lower(const std::basic_string<CharT> &str,
                               const std::locale &lc) LASHE_NOEXCEPT
{
    return lower(str.c_str(), lc);
}
template <class CharT>
std::basic_string<CharT> lower(const CharT *str) LASHE_NOEXCEPT
{
    std::locale lc;
    return lower(str, lc);
}
template <class CharT>
std::basic_string<CharT>
lower(const std::basic_string<CharT> &str) LASHE_NOEXCEPT
{
    std::locale lc;
    return lower(str.c_str(), lc);
}

template <class CharT>
std::basic_string<CharT> upper(const CharT *str,
                               const std::locale &lc) LASHE_NOEXCEPT
{
    size_t i = 0;
    std::basic_string<CharT> ret(std::char_traits<CharT>::length(str), 0);

    std::for_each(ret.begin(), ret.end(), [&i, &lc, &str](CharT &c) {
        c = std::toupper(str[i], lc);
        i += 1;
    });

    return ret;
}
template <class CharT>
std::basic_string<CharT> upper(const std::basic_string<CharT> &str,
                               const std::locale &lc) LASHE_NOEXCEPT
{
    return upper(str.c_str(), lc);
}
template <class CharT>
std::basic_string<CharT> upper(const CharT *str) LASHE_NOEXCEPT
{
    std::locale lc;
    return upper(str, lc);
}
template <class CharT>
std::basic_string<CharT>
upper(const std::basic_string<CharT> &str) LASHE_NOEXCEPT
{
    std::locale lc;
    return upper(str.c_str(), lc);
}

template <class CharT, class IteratorType>
std::basic_string<CharT> join_str(IteratorType first, IteratorType last,
                                  const CharT *str)
{
    const auto len = std::distance(first, last);
    std::basic_stringstream<CharT> ss;

    if (len > 0) {
        const auto end = last - 1;

        while (first != end) {
            ss << *first << str;
            first++;
        }
        ss << *first;
    }

    return ss.str();
}

template <class CharT, class IteratorType>
std::basic_string<CharT> join_str(IteratorType first, IteratorType last,
                                  const std::basic_string<CharT> &str)
{
    return join_str(first, last, str.c_str());
}

} // namespace ashe

#endif
