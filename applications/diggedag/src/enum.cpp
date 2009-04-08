
#include "enum.hpp"

namespace diggedag
{
  std::string state_to_string (state s)
  {
    if ( s == Incomplete )
      return "Incomplete";
    if ( s == Pending )
      return "Pending";
    else if ( s == Running )
      return "Running";
    else if ( s == Ready )
      return "Ready";
    else if ( s == Failed )
      return "Failed";
    else
      return "Unknown";
  }

} // namespace diggedag

