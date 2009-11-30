
#ifndef DIGEDAG_ENUM_HPP
#define DIGEDAG_ENUM_HPP


#include <string>
#include <vector>

#include "config.hpp"

namespace digedag
{
  enum state
  {
    Incomplete = 0,
    Stopped    = 1,
    Pending    = 2,
    Running    = 3, 
    Done       = 4,
    Failed     = 5
  };

  std::string state_to_string (state s);

} // namespace digedag

#endif // DIGEDAG_ENUM_HPP

