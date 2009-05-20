
#ifndef DIGEDAG_ENUM_HPP
#define DIGEDAG_ENUM_HPP

#include <string>
#include <vector>

namespace digedag
{
  class edge;
  class node;

  typedef std::string                          node_id_t;
  typedef std::pair <std::string, std::string> edge_id_t;
  typedef std::vector <digedag::edge *>       edge_map_t;
  typedef              digedag::node *        node_map_t;

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

