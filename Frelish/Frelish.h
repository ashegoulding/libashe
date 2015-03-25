/*
 *  Ancestral class for objects in 'ashe' namespace.
 *  Icebank: Classes that are less important or being in development
 *  Fjord: Classes that are mostly firmly well-implemented and changes least.
 *
 *  StrongRune: Exceptions cannot be ignored and recovered.
 *  WeakRunes: Exceptions that can be ignored or can be fixed in runtime.
 *
 *  This distinction is for personality, status and positions of classes.
 *  Kind of debugging manner, more like coding artistry.
 */

#include "StrongRune.h"
#include "WeakRune.h"
#include "Icebank.h"
#include "Fjord.h"

#include <vector>
#include <set>

namespace ashe
{

extern std::string trim__(const std::string &x) noexcept;
extern std::vector<std::string> explodeSpace__(const std::string &x) noexcept;
extern std::vector<std::string> explode__(const std::string &x, const std::set<std::string> &delimiters) noexcept;
extern std::string toLower__(const std::string &x) noexcept;
extern std::string toUpper__(const std::string &x) noexcept;
extern std::string::size_type findStarting__(const std::string &x) noexcept;

}
