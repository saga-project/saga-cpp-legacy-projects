
#ifndef DIGGEDAG_ENUM_HPP
#define DIGGEDAG_ENUM_HPP

#include <string>

namespace diggedag
{
  enum state
  {
    Incomplete = 0,
    Pending    = 1,
    Running    = 2, 
    Ready      = 3
  };

  std::string state_to_string (state s);

} // namespace diggedag

#endif // DIGGEDAG_ENUM_HPP

