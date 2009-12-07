
#ifndef DIGEDAG_SCHEDULER_HPP
#define DIGEDAG_SCHEDULER_HPP

#include <vector>
#include <deque>

#include <saga/saga.hpp>

#include "dag.hpp"
#include "node.hpp"
#include "edge.hpp"

#include "util/mutex.hpp"
#include "util/thread.hpp"


namespace digedag
{
  class dag;
  class watch_tasks;
  class scheduler : public digedag::util::thread, 
                    public boost::enable_shared_from_this <scheduler>
  {
    private:
      struct job_info_t 
      { 
        std::string rm;
        std::string host;
        std::string pwd;
        std::string path;
      };

      std::map <std::string, job_info_t>   job_info_;

      std::string                          src_; // scheduling policy

      std::string                          data_src_pwd_;
      std::string                          data_tgt_pwd_;

      std::string                          data_src_host_;
      std::string                          data_tgt_host_;

      saga::session                      & session_;
      dag                                * dag_;

      bool                                 stopped_;

      // queues
      std::deque <sp_t <node> >            queue_nodes_;
      std::deque <sp_t <edge> >            queue_edges_;

      saga::task_container                 tc_nodes_;
      saga::task_container                 tc_edges_;

      sp_t <watch_tasks>                   watch_nodes_;
      sp_t <watch_tasks>                   watch_edges_;

      int                                  max_nodes_;
      int                                  max_edges_;
      
      int                                  active_nodes_;
      int                                  active_edges_;

      std::map <saga::task, sp_t <node> >  node_task_map_;
      std::map <saga::task, sp_t <edge> >  edge_task_map_;

      util::mutex                          mtx_;


    public:
      scheduler  (dag * d, 
                  const std::string & src, 
                  saga::session       session);
      ~scheduler (void);

      void parse_src             (void);
      void stop                  (void);

      void thread_work           (void);

      void hook_dag_create       (void);
      void hook_dag_destroy      (void);
      void hook_dag_schedule     (void);
      void hook_dag_run_pre      (void);
      void hook_dag_run_post     (void);
      void hook_dag_run_done     (void);
      void hook_dag_run_fail     (void);
      void hook_dag_wait         (void);

      void hook_node_add         (sp_t <node> n);
      void hook_node_remove      (sp_t <node> n);
      void hook_node_run_pre     (sp_t <node> n);
      void hook_node_run_done    (sp_t <node> n);
      void hook_node_run_fail    (sp_t <node> n);

      void hook_edge_add         (sp_t <edge> e);
      void hook_node_remove      (sp_t <edge> e);
      void hook_edge_run_pre     (sp_t <edge> e);
      void hook_edge_run_done    (sp_t <edge> e);
      void hook_edge_run_fail    (sp_t <edge> e);

      saga::session
           hook_saga_get_session (void);

      void work_finished         (saga::task  t, 
                                  std::string flag);
  };

  // as long as the task_container.wait() and also the callbacks on
  // task_container are broken, we use this watch_tasks class for notification
  // on finished tasks.  It will constantly cycle over the contents of a given
  // task container in its worker thread, and will call 'cb' in the main thread
  // if any task changed its state to Done or Failed.
  class watch_tasks : public digedag::util::thread
  {
    private:
      saga::task_container tc_;
      sp_t <scheduler>     s_;
      std::string          f_;

    public:
      watch_tasks (sp_t <scheduler>     s, 
                   saga::task_container tc, 
                   std::string          flag) 
        : tc_ (tc)
        , s_  (s)
        , f_  (flag)
      {
        thread_run ();
      }

      void thread_work (void)
      {

        while ( true )
        {
          std::vector <saga::task> tasks = tc_.list_tasks ();
          
          for ( unsigned int i = 0; i < tasks.size (); i++ )
          {
            saga::task        t = tasks[i];
            saga::task::state s = t.get_state ();

            if ( s == saga::task::Done   || 
                 s == saga::task::Failed )
            {
              std::cout << " === task " << t.get_id () << " finished: " << t.get_state () << std::endl;

              tc_.remove_task (t);

              s_->work_finished (t, f_);
            }
          }

          ::sleep (1);
        }
      }
  };

} // namespace digedag

#endif // DIGEDAG_SCHEDULER_HPP

