
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
      std::string policy_; // scheduling policy

      std::string data_src_pwd_;
      std::string data_tgt_pwd_;

      std::string data_src_host_;
      std::string data_tgt_host_;

      std::map <std::string, int> retry_nodes_;

      struct job_info_t 
      { 
        std::string host; 
        std::string pwd; 
      };

      std::map <std::string, job_info_t> job_info_;

    public:
      scheduler (void);
      ~scheduler (void);

      void set_scheduler      (std::string s);

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

