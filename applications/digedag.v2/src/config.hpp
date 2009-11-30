
#ifndef DIGEDAG_CONFIG_HPP
#define DIGEDAG_CONFIG_HPP

#include <vector>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace digedag 
{
  class edge;
  class node;

  // C++ has no template typedefs :-(
  #define sp_t boost::shared_ptr

  typedef std::string                             node_id_t;
  typedef std::pair   <std::string, std::string>  edge_id_t;
  typedef std::vector <sp_t <edge> >              edge_map_t;
  typedef              sp_t <node>                node_map_t;

}

#endif // DIGEDAG_CONFIG_HPP

