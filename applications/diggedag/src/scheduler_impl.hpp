
#ifndef DIGGEDAG_SCHEDULER_IMPL_HPP
#define DIGGEDAG_SCHEDULER_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "dag.hpp"


namespace diggedag
{
  class dag;
  namespace impl
  {
    class scheduler 
    {
      private:
        // diggedag::dag dag_;
        // bool          have_dag_;

      public:
         scheduler (void);
        ~scheduler (void);

        void hook_dag_create    (diggedag::dag & d);
        void hook_dag_destroy   (diggedag::dag & d);
        void hook_dag_schedule  (diggedag::dag & d);
        void hook_dag_run_pre   (diggedag::dag & d);
        void hook_dag_run_post  (diggedag::dag & d);
        void hook_dag_run_done  (diggedag::dag & d);
        void hook_dag_run_fail  (diggedag::dag & d);
        void hook_dag_wait      (diggedag::dag & d);

        void hook_node_add      (diggedag::dag & d, diggedag::node & n);
        void hook_node_remove   (diggedag::dag & d, diggedag::node & n);
        void hook_node_run_pre  (diggedag::dag & d, diggedag::node & n);
        void hook_node_run_done (diggedag::dag & d, diggedag::node & n);
        void hook_node_run_fail (diggedag::dag & d, diggedag::node & n);

        void hook_edge_add      (diggedag::dag & d, diggedag::edge & e);
        void hook_node_remove   (diggedag::dag & d, diggedag::edge & e);
        void hook_edge_run_pre  (diggedag::dag & d, diggedag::edge & e);
        void hook_edge_run_done (diggedag::dag & d, diggedag::edge & e);
        void hook_edge_run_fail (diggedag::dag & d, diggedag::edge & e);
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_SCHEDULER_IMPL_HPP

