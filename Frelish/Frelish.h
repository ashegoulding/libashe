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

// Returns a white spaces trimmed string in both start and end of the given string
extern std::string trim__(const std::string &x) noexcept;
// Returns a vector contains strings exploded by white space delimiters
extern std::vector<std::string> explodeSpace__(const std::string &x) noexcept;
// Returns a vector contains strings exploded by the given delimiter set
extern std::vector<std::string> explode__(const std::string &x, const std::set<std::string> &delimiters) noexcept;
// Returns a string converted into upper case letters
extern std::string toLower__(const std::string &x) noexcept;
// Returns a string converted into lower case letters
extern std::string toUpper__(const std::string &x) noexcept;
/* Returns "first_not_of" white space's position.
 * The return value will be always zero or positive.
 *
 * @RETURN
 *  0: The string starts with a non-white space character right away OR the string is empty
 *  (above 0): The start of the string after white space character(s)
 */
extern std::string::size_type findStarting__(const std::string &x) noexcept;

}
