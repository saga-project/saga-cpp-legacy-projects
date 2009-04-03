
#ifndef DIGGEDAG_ENUM_HPP
#define DIGGEDAG_ENUM_HPP

#include <string>

namespace diggedag
{
  enum state
  {
    Pending = 0,
    Running = 1, 
    Ready   = 2
  };

  std::string state_to_string (state s);

} // namespace diggedag

#endif // DIGGEDAG_ENUM_HPP

