
#ifndef DIGEDAG_SCHEDULER_HPP
#define DIGEDAG_SCHEDULER_HPP

#include <set>
#include <deque>
#include <vector>

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
                    public util::enable_shared_from_this <scheduler>
  {
    private:
      struct job_info_t 
      { 
        std::string rm;
        std::string host;
        std::string pwd;
        std::string path;
      };

      std::map <std::string, job_info_t>     job_info_;

      std::string                            src_; // scheduling policy

      std::string                            data_src_pwd_;
      std::string                            data_tgt_pwd_;

      std::string                            data_src_host_;
      std::string                            data_tgt_host_;

      saga::session                        & session_;
      dag                                  * dag_;

      bool                                   stopped_;

      // queues
      std::deque <boost::shared_ptr <node> > queue_nodes_;
      std::deque <boost::shared_ptr <edge> > queue_edges_;

      saga::task_container                   tc_nodes_;
      saga::task_container                   tc_edges_;

      watch_tasks                          * watch_nodes_;
      watch_tasks                          * watch_edges_;

      int                                    max_nodes_;
      int                                    max_edges_;
      
      int                                    active_nodes_;
      int                                    active_edges_;

      std::map <saga::task, boost::shared_ptr <node> > node_task_map_;
      std::map <saga::task, boost::shared_ptr <edge> > edge_task_map_;

      util::mutex                            mtx_;

      // list of known nodes and edges
      std::set <std::string>                known_nodes_;
      std::set <std::string>                known_edges_;


    public:
      scheduler  (dag * d, 
                  const std::string & src, 
                  saga::session       session);
      ~scheduler (void);

      void parse_src             (void);
      void stop                  (void);

      void thread_work           (void);

      bool hook_dag_create       (void);
      bool hook_dag_destroy      (void);
      bool hook_dag_schedule     (void);
      bool hook_dag_run_pre      (void);
      bool hook_dag_run_post     (void);
      bool hook_dag_run_done     (void);
      bool hook_dag_run_fail     (void);
      bool hook_dag_wait         (void);

      bool hook_node_add         (boost::shared_ptr <node> n);
      bool hook_node_remove      (boost::shared_ptr <node> n);
      bool hook_node_run_pre     (boost::shared_ptr <node> n);
      bool hook_node_run_done    (boost::shared_ptr <node> n);
      bool hook_node_run_fail    (boost::shared_ptr <node> n);

      bool hook_edge_add         (boost::shared_ptr <edge> e);
      bool hook_edge_remove      (boost::shared_ptr <edge> e);
      bool hook_edge_run_pre     (boost::shared_ptr <edge> e);
      bool hook_edge_run_done    (boost::shared_ptr <edge> e);
      bool hook_edge_run_fail    (boost::shared_ptr <edge> e);

      saga::session
           hook_saga_get_session (void);

      void work_finished         (saga::task  t, 
                                  std::string flag);

      void dump_map (const std::map <saga::task, boost::shared_ptr <edge> >  & map);
  };

  // task_container are broken, we use this watch_tasks class for notification
  // on finished tasks.  It will constantly cycle over the contents of a given
  // task container in its worker thread, and will call 'cb' in the main thread
  // if any task changed its state to Done or Failed.
  class watch_tasks : public digedag::util::thread
  {
    private:
      saga::task_container              tc_;
      boost::shared_ptr <scheduler>     s_;
      std::string                       f_;
      bool                              todo_;

    public:
      watch_tasks (boost::shared_ptr <scheduler> s, 
                   saga::task_container          tc, 
                   std::string                   flag) 
        : tc_   (tc)
        , s_    (s)
        , f_    (flag)
        , todo_ (true)

      {
        thread_run ();
      }

      ~watch_tasks (void)
      {
        todo_ = false;
        thread_join ();
      }

      void thread_work (void)
      {
        while ( todo_ )
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

          // avoid busy wait
          if ( tasks.size () == 0 )
          {
            ::sleep (1);
          }
        }
      }
  };

} // namespace digedag

#endif // DIGEDAG_SCHEDULER_HPP

