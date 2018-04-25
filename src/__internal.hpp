#ifndef __LASHE_INTERNAL_HPP_
#define __LASHE_INTERNAL_HPP_
#include "libashe/ModuleException.hpp"
#include "libashe/StringUtils.hpp"
#include "libashe/helper.hpp"
#include "libashe/pub.hpp"

#include <cstdarg>

#include <mutex>
#include <regex>
#include <set>
#include <string>
#include <vector>

#if ASHE_ISOS_WIN()
#include <Windows.h>
#elif ASHE_ISOS_POSIX()
#include <dlfcn.h>
#include <unistd.h>
#endif


namespace __lashe {

#if ASHE_ISOS_POSIX()
typedef void *ModuleType;
typedef pid_t PIDType;
#elif ASHE_ISOS_WIN()
typedef HMODULE ModuleType;
typedef DWORD PIDType;
#endif

struct Global {
    std::set<ashe::LAsheAbility> initAbilitySet;
    std::vector<ashe::LAsheAbility> initAbilityVec;
};

extern Global *global;

[[noreturn]] void die_critical() LASHE_NOEXCEPT;

// TODO: test
void ensure_init();

// TODO: test
void ensure_ability(const ashe::LAsheAbility x);

// TODO: test
void drop_unimpl();

// TODO: test
bool has_ability(const ashe::LAsheAbility x) LASHE_NOEXCEPT;

// TODO: test
ModuleType load_module(const char *name);

// TODO: test
void unload_module(ModuleType mod);

// TODO: test
template <class T> T load_func(const ModuleType mod, const size_t cnt, ...)
{
    T ret = nullptr;
    std::vector<std::string> tried(cnt);
    va_list vl;
    size_t i;
    std::string arg;

    if (0 == cnt) {
        die_critical();
    }

    va_start(vl, cnt);
    for (i = 0; i < cnt; i += 1) {
#if ASHE_ISOS_POSIX()
        arg = va_arg(vl, const char *);
        ret = (T)::dlsym(mod, arg.c_str());
        if (nullptr != ret) {
            break;
        }
#elif ASHE_ISOS_WIN()
// TODO
#error "FIXME"
#endif
        tried.push_back(arg);
    }
    va_end(vl);

    if (nullptr == ret) {
        std::stringstream ss;

        ss << "Failed to load function: "
           << ashe::join_str(tried.begin(), tried.end(), " | ");
        throw ashe::ModuleException(ss.str().c_str());
    }

    return ret;
}

inline PIDType get_pid() LASHE_NOEXCEPT
{
#if ASHE_ISOS_WIN()
    return ::GetCurrentProcessId();
#elif ASHE_ISOS_POSIX()
    return ::getpid();
#else
#error "FIXME"
#endif
}

}

#endif
