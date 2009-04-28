
#ifndef DIGGEDAG_NODE_IMPL_HPP
#define DIGGEDAG_NODE_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/thread.hpp"

#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"
#include "node_description.hpp"


namespace diggedag
{
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

         diggedag::dag                dag_;
         diggedag::node               node_;
         diggedag::scheduler          scheduler_;


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

        void            set_node        (diggedag::node & n);
        void            set_dag         (diggedag::dag  & d);
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_NODE_IMPL_HPP

