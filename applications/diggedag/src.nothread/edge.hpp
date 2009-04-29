
#ifndef DIGGEDAG_EDGE_HPP
#define DIGGEDAG_EDGE_HPP

#include <vector>

#include <saga/saga.hpp>

#include "enum.hpp"
#include "dag.hpp"
#include "node.hpp"
#include "scheduler.hpp"


namespace diggedag
{
  class edge
  {
    private:
      saga::url             src_;   // src location of data
      saga::url             tgt_;   // tgt location of data
      diggedag::state       state_; // state of instance

      diggedag::node      * src_node_;
      diggedag::node      * tgt_node_;

      diggedag::dag       * dag_;
      diggedag::scheduler * scheduler_;

      saga::task            task_;  // workload


    public:
      edge  (const saga::url & src, 
             const saga::url & tgt = "");
      ~edge (void);

      bool             cb           (saga::monitorable o, 
                                     saga::metric      m, 
                                     saga::context     c);
      void             fire         (void);
      void             erase_src    (void);
      void             erase_tgt    (void);
      void             add_src_node (diggedag::node * src);
      void             add_tgt_node (diggedag::node * tgt);
      diggedag::state  get_state    (void) const;

      saga::url        get_src      (void) const { return src_; }
      saga::url        get_tgt      (void) const { return tgt_; }

      diggedag::node * get_src_node (void) const { return src_node_; }
      diggedag::node * get_tgt_node (void) const { return tgt_node_; }

      void             set_dag      (diggedag::dag  * d);
  };

} // namespace diggedag

#endif // DIGGEDAG_EDGE_HPP

