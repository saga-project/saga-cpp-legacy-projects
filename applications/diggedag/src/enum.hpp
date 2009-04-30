
#ifndef DIGGEDAG_ENUM_HPP
#define DIGGEDAG_ENUM_HPP

#include <string>

namespace diggedag
{
  enum state
  {
    Incomplete = 0,
    Stopped    = 1,
    Pending    = 2,
    Running    = 3, 
    Ready      = 4,
    Failed     = 5
  };

  std::string state_to_string (state s);

} // namespace diggedag

#endif // DIGGEDAG_ENUM_HPP

