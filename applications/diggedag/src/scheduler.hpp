
#ifndef DIGGEDAG_SCHEDULER_HPP
#define DIGGEDAG_SCHEDULER_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/mutex.hpp"


namespace diggedag
{
  class dag;
  class node;
  class edge;
  class scheduler 
  {
    private:
      util::mutex mtx_;

    public:
      scheduler (void);
      ~scheduler (void);

      void hook_dag_create    (diggedag::dag * d);
      void hook_dag_destroy   (diggedag::dag * d);
      void hook_dag_schedule  (diggedag::dag * d);
      void hook_dag_run_pre   (diggedag::dag * d);
      void hook_dag_run_post  (diggedag::dag * d);
      void hook_dag_run_done  (diggedag::dag * d);
      void hook_dag_run_fail  (diggedag::dag * d);
      void hook_dag_wait      (diggedag::dag * d);

      void hook_node_add      (diggedag::dag * d, diggedag::node * n);
      void hook_node_remove   (diggedag::dag * d, diggedag::node * n);
      void hook_node_run_pre  (diggedag::dag * d, diggedag::node * n);
      void hook_node_run_done (diggedag::dag * d, diggedag::node * n);
      void hook_node_run_fail (diggedag::dag * d, diggedag::node * n);

      void hook_edge_add      (diggedag::dag * d, diggedag::edge * e);
      void hook_node_remove   (diggedag::dag * d, diggedag::edge * e);
      void hook_edge_run_pre  (diggedag::dag * d, diggedag::edge * e);
      void hook_edge_run_done (diggedag::dag * d, diggedag::edge * e);
      void hook_edge_run_fail (diggedag::dag * d, diggedag::edge * e);
  };

} // namespace diggedag

#endif // DIGGEDAG_SCHEDULER_HPP

