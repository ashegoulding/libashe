#include "__internal.hpp"

#include <algorithm>
#include <cstdlib>


namespace __lashe {

Global *global = nullptr;

[[noreturn]] void die_critical() LASHE_NOEXCEPT
{
    ::abort();
    // in case `abort()` is ignored by debugger.
    ::exit(128 + 6);
}

void ensure_init()
{
    if (global == nullptr) {
        throw ashe::Exception("LibAshe not initialised.");
    }
}

void ensure_ability(const ashe::LAsheAbility x)
{
    ensure_init();

    if (global->initAbilitySet.find(x) == global->initAbilitySet.end()) {
        std::stringstream ss;

        ss << "LibAshe not initialised with ability: " << ashe::tostr(x);
        throw ashe::Exception(ss.str().c_str());
    }
}

void drop_unimpl() { throw ashe::Exception("not implemented."); }

bool has_ability(const ashe::LAsheAbility x) LASHE_NOEXCEPT
{
    return global->initAbilitySet.end() != global->initAbilitySet.find(x);
}

ModuleType load_module(const char *name)
{
    ModuleType ret;

#if ASHE_ISOS_POSIX()
    ret = ::dlopen(name, RTLD_LAZY);
    if (nullptr == ret) {
        std::stringstream ss;

        ss << "Failed to load module(" << name << "): " << ::dlerror();
        throw ashe::ModuleException(ss.str().c_str());
    }
#elif ASHE_ISOS_WIN()
// TODO
#error "FIXME"
#endif

    return ret;
}

void unload_module(ModuleType mod)
{
#if ASHE_ISOS_POSIX()
    if (::dlclose(mod) != 0) {
        std::stringstream ss;

        ss << "Failed to unload module: " << ::dlerror();
        throw ashe::ModuleException(ss.str().c_str());
    }
#elif ASHE_ISOS_WIN()
// TODO
#error "FIXME"
#endif
}

} // namespace __lashe
