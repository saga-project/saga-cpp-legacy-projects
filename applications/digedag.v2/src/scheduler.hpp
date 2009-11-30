
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
      std::string src_; // scheduling policy

      std::string data_src_pwd_;
      std::string data_tgt_pwd_;

      std::string data_src_host_;
      std::string data_tgt_host_;

      struct job_info_t 
      { 
        std::string rm;
        std::string host;
        std::string pwd;
        std::string path;
      };

      std::map <std::string, job_info_t> job_info_;

      saga::session session_;

      sp_t <dag> dag_;


    public:
      scheduler  (dag * d, 
                  const std::string & src);
      ~scheduler (void);

      void parse_src             (void);
      void stop                  (void);

      void hook_dag_create       (void);
      void hook_dag_destroy      (void);
      void hook_dag_schedule     (void);
      void hook_dag_run_pre      (void);
      void hook_dag_run_post     (void);
      void hook_dag_run_done     (void);
      void hook_dag_run_fail     (void);
      void hook_dag_wait         (void);

      void hook_node_add         (node & n);
      void hook_node_remove      (node & n);
      void hook_node_run_pre     (node & n);
      void hook_node_run_done    (node & n);
      void hook_node_run_fail    (node & n);

      void hook_edge_add         (edge & e);
      void hook_node_remove      (edge & e);
      void hook_edge_run_pre     (edge & e);
      void hook_edge_run_done    (edge & e);
      void hook_edge_run_fail    (edge & e);

      saga::session
           hook_saga_get_session (void);
  };

} // namespace digedag

#endif // DIGEDAG_SCHEDULER_HPP

