
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/thread.hpp"

#include "node.hpp"
#include "node_description.hpp"


namespace diggedag
{
  enum state
  {
    Incomplete = 0,
    Pending    = 1,
    Running    = 2, 
    Ready      = 3,
    Failed     = 4
  };

  std::string state_to_string (state s)
  {
    if      ( s == Incomplete ) return "Incomplete";
    else if ( s == Pending    ) return "Pending"   ;
    else if ( s == Running    ) return "Running"   ;
    else if ( s == Ready      ) return "Ready"     ;
    else if ( s == Failed     ) return "Failed"    ;
    else                        return "Unknown"   ;
  }
  class node : public diggedag::util::thread
  {
    private:
      diggedag::node_description     nd_;       // node application to run

      std::vector <diggedag::edge *> edge_in_;  // input  data
      std::vector <diggedag::edge *> edge_out_; // output data

      std::string                    name_;     // instance name
      diggedag::state                state_;    // instance state

      diggedag::dag                * dag_;
      diggedag::scheduler          * scheduler_;


    public:
      node  (diggedag::node_description & nd);
      node  (std::string                  cmd);
      ~node (void);

      void            set_name        (std::string      name);
      void            add_edge_in     (diggedag::edge * e);
      void            add_edge_out    (diggedag::edge * e);

      void            fire            (void);
      void            thread_work     (void);
      std::string     get_name        (void) const;
      diggedag::node_description 
                      get_description (void) const;
      diggedag::state get_state       (void);

      void            set_dag         (diggedag::dag  * d);
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

