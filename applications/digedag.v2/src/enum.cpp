
#include <sstream>

#include "config.hpp"
#include "enum.hpp"

namespace digedag
{
  std::string state_to_string (state s)
  {
    if      ( s == Incomplete ) return "Incomplete";
    else if ( s == Pending    ) return "Pending"   ;
    else if ( s == Running    ) return "Running"   ;
    else if ( s == Done       ) return "Done"      ;
    else if ( s == Failed     ) return "Failed"    ;
    else                        return "Unknown"   ;
  }

  std::string edge_id_to_string (edge_id_t id)
  {
    std::stringstream ss;

    ss << id;

    return ss.str ();
  }
} // namespace digedag

