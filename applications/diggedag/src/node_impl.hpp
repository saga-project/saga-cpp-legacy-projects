
#ifndef DIGGEDAG_NODE_IMPL_HPP
#define DIGGEDAG_NODE_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "node_description.hpp"
#include "util/thread.hpp"


namespace diggedag
{
  class edge;
  namespace impl
  {
    class node : public diggedag::util::thread
    {
      private:
        diggedag::node_description   nd_;       // node application to run

        std::vector <diggedag::edge> edge_in_;  // input  data
        std::vector <diggedag::edge> edge_out_; // output data

        std::string                  name_;     // instance name
        diggedag::state              state_;    // instance state


      public:
         node (const diggedag::node_description & nd);
         node (const std::string                  cmd);
        ~node (void);

        void            set_name        (const std::string    & name);
        void            add_edge_in     (const diggedag::edge & e);
        void            add_edge_out    (const diggedag::edge & e);

        void            fire            (void);
        void            thread_work     (void);
        std::string     get_name        (void) const;
        diggedag::node_description 
                        get_description (void) const;
        diggedag::state get_state       (void);
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_NODE_IMPL_HPP

