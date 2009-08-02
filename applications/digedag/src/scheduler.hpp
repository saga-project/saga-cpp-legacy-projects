
#ifndef DIGEDAG_SCHEDULER_HPP
#define DIGEDAG_SCHEDULER_HPP

#include <vector>

#include <saga/saga.hpp>

#include "util/mutex.hpp"


namespace digedag
{
  class dag;
  class node;
  class edge;
  class scheduler 
  {
    private:
      bool stopped_;

      util::mutex mtx_;
      std::string policy_; // scheduling policy

      std::string data_src_pwd_;
      std::string data_tgt_pwd_;

      std::string data_src_host_;
      std::string data_tgt_host_;

      std::map <std::string, int> retry_nodes_;

      struct job_info_t 
      { 
        std::string rm;
        std::string host;
        std::string pwd;
        std::string path;
      };

      std::map <std::string, job_info_t> job_info_;

    public:
      scheduler (void);
      ~scheduler (void);

      void stop               (void);

      void set_scheduler      (std::string s);

      void hook_dag_create    (digedag::dag * d);
      void hook_dag_destroy   (digedag::dag * d);
      void hook_dag_schedule  (digedag::dag * d);
      void hook_dag_run_pre   (digedag::dag * d);
      void hook_dag_run_post  (digedag::dag * d);
      void hook_dag_run_done  (digedag::dag * d);
      void hook_dag_run_fail  (digedag::dag * d);
      void hook_dag_wait      (digedag::dag * d);

      void hook_node_add      (digedag::dag * d, digedag::node * n);
      void hook_node_remove   (digedag::dag * d, digedag::node * n);
      void hook_node_run_pre  (digedag::dag * d, digedag::node * n);
      void hook_node_run_done (digedag::dag * d, digedag::node * n);
      void hook_node_run_fail (digedag::dag * d, digedag::node * n);

      void hook_edge_add      (digedag::dag * d, digedag::edge * e);
      void hook_node_remove   (digedag::dag * d, digedag::edge * e);
      void hook_edge_run_pre  (digedag::dag * d, digedag::edge * e);
      void hook_edge_run_done (digedag::dag * d, digedag::edge * e);
      void hook_edge_run_fail (digedag::dag * d, digedag::edge * e);
  };

} // namespace digedag

#endif // DIGEDAG_SCHEDULER_HPP

