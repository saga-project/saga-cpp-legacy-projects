
#include <vector>
#include <fstream>

#include <saga/saga.hpp>

#include "util/split.hpp"
#include "util/scoped_lock.hpp"

#include "dag.hpp"
#include "enum.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "scheduler.hpp"

namespace digedag
{
  scheduler::scheduler (dag               * d, 
                        const std::string & src)
    : stopped_       (    false),
      session_       (     true),
      dag_           (        d),
      max_nodes_     (       10),
      max_edges_     (       10),
      active_nodes_  (        0),
      active_edges_  (        0)
  {
    pthread_mutex_t m = mtx_.get ();

    src_ = src;

    parse_src ();
  }

  scheduler::~scheduler (void)
  {
  }

  void scheduler::stop (void)
  {
    stopped_ = true;
  }

  void scheduler::parse_src (void)
  {
    if ( stopped_ ) return;

    if ( src_.empty () )
    {
      return;
    }

    std::fstream fin;
    std::string  line;

    fin.open (src_.c_str (), std::ios::in);

    if ( fin.fail () )
    {
      std::cerr << "opening " << src_ << " failed" << std::endl;
      throw "Cannot open file";
    }

    unsigned int lnum = 1;

    while ( std::getline (fin, line) )
    {
      std::vector <std::string> words = split (line);

      if ( words.size () < 1 )
      {
        std::cerr << "parser error (1) in " << src_ << " at line " << lnum << std::endl;
      }
      else if ( words[0] == "#" )
      {
        // ignore comments
      }
      else if ( words[0] == "context" )
      {
        try 
        {
          // split context line words into key=val pairs, and set those as
          // attributes.  Add that respective context to the dag's session on
          // hook_dag_create
          saga::context c;

          // leave out context keyword
          for ( unsigned int i = 1; i < words.size (); i++ )
          {
            // all other words, split at '='
            std::vector <std::string> elems = split (words[i], "=", 2);

            if ( 0 == elems.size () )
            {
              // nothing to do
              // this should never happen though unless words[i] was empty.
            }
            else if ( 1 == elems.size () )
            {
              // empty value
              c.set_attribute (elems[0], "");
            }
            else // never have more than two elems, as per split limit above
            {
              // set key and value
              c.set_attribute (elems[0], elems[1]);
            }
          }

          session_.add_context (c);
        }
        catch ( const saga::exception & e )
        {
          // error in handling context line
          std::cerr << "context error in " << src_ 
                    << " at line " << lnum 
                    << ": \n" << e.what () 
                    << std::endl;
        }
      }
      else if ( words[0] == "data" )
      {
        if ( words.size () != 4 )
        {
          std::cerr << "parser error (2) in " << src_ << " at line " << lnum << std::endl;
        }
        else if ( words[1] == "INPUT" )
        {
          data_src_host_ = words[2];
          data_src_pwd_  = words[3];
        }
        else if ( words[1] == "OUTPUT" )
        {
          data_tgt_host_ = words[2];
          data_tgt_pwd_  = words[3];
        }
        else
        {
          std::cerr << "parser error (3) in " << src_ << " at line " << lnum << std::endl;
        }
      }
      else if ( words[0] == "job" )
      {
        if ( words.size () < 5 )
        {
          std::cerr << "parser error (4) in " << src_ << " at line " << lnum << std::endl;
        }
        else
        {
          job_info_[words[1]].rm   = words[2];
          job_info_[words[1]].host = words[3];
          job_info_[words[1]].pwd  = words[4];

          if ( words.size () >= 6 )
          {
            job_info_[words[1]].path = words[5];
          }
        }
      }
      

      lnum++;
    }
  }

  /////////////////////////////////////////////////////////////////
  //
  // dag hooks
  //
  void scheduler::hook_dag_create (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_destroy (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_schedule (void)
  {
    if ( stopped_ ) return;

    util::scoped_lock sl (mtx_);

    // start watching the task containers, even if they are still empty
    watch_nodes_ = sp_t <watch_tasks> (new watch_tasks (shared_from_this(), tc_nodes_, "node"));
    watch_edges_ = sp_t <watch_tasks> (new watch_tasks (shared_from_this(), tc_edges_, "edge"));

    // walk throgh the dag, and assign execution host for nodes, and data
    // prefixes for edges
    std::map <node_id_t, node_map_t> nodes = dag_->get_nodes ();
    std::map <edge_id_t, edge_map_t> edges = dag_->get_edges ();

    // first, fix pwd and host for INPUT and OUTPUT nodes
    sp_t <node> input  = nodes["INPUT"];
    sp_t <node> output = nodes["OUTPUT"];

    input->set_pwd   (data_src_pwd_);
    input->set_host  (data_src_host_);
    output->set_pwd  (data_tgt_pwd_);
    output->set_host (data_tgt_host_);


    // now fix all other nodes, too
    {
      std::map <node_id_t, node_map_t> :: const_iterator it;
      std::map <node_id_t, node_map_t> :: const_iterator begin = nodes.begin ();
      std::map <node_id_t, node_map_t> :: const_iterator end   = nodes.end ();

      for ( it = begin; it != end; it++ )
      {
        std::string id = it->first;
        sp_t <node> n  = it->second;

        if ( job_info_.find (id) != job_info_.end () )
        {
          n->set_rm   (job_info_[id].rm);
          n->set_host (job_info_[id].host);
          n->set_pwd  (job_info_[id].pwd);

          if ( ! job_info_[id].path.empty () )
          {
            n->set_path (job_info_[id].path);
          }
        }
      }
    }
  }


  void scheduler::hook_dag_run_pre (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // start the scheduler thread which executes nodes and edges
    thread_run ();
  }


  void scheduler::hook_dag_run_post (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_done (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_fail (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    std::cout << " === dag failed!  exit." << std::endl;
    ::exit (3);
  }


  void scheduler::hook_dag_wait (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }



  /////////////////////////////////////////////////////////////////
  //
  // node hooks
  //
  void scheduler::hook_node_add (sp_t <node> n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_remove (sp_t <node> n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // FIXME: remove from queue if needed
  }


  void scheduler::hook_node_run_pre (sp_t <node> n)           
  {
    if ( stopped_ ) return;

    util::scoped_lock sl (mtx_);

    // queue the node for work
    queue_nodes_.push_back (n);
    std::cout << " === adding   node to   queue: " 
              << n->get_name_s () << std::endl;
  }


  void scheduler::hook_node_run_done (sp_t <node> n)           
  {
  }


  // NOTE that this implementation is recursive!  no locks, please...
  void scheduler::hook_node_run_fail (sp_t <node> n)           
  {
    if ( stopped_ ) return;

    std::cout << " === node failed: " << n->get_name_s () 
              << " (" << active_nodes_ << ")"
              << std::endl;

    active_nodes_--;
    assert (active_nodes_ >= 0);

    std::cout << " === node failed: " << n->get_name_s () << std::endl;
    ::exit (1);
  }



  /////////////////////////////////////////////////////////////////
  //
  // edge hooks
  //
  void scheduler::hook_edge_add (sp_t <edge> e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_remove (sp_t <edge> e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // FIXME: remove from queue if needed
  }


  void scheduler::hook_edge_run_pre (sp_t <edge> e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // add edge to queue
    queue_edges_.push_back (e);
    std::cout << " === adding   edge to   queue: " 
              << e->get_name_s () << std::endl;
  }


  void scheduler::hook_edge_run_done (sp_t <edge> e)           
  {
    if ( stopped_ ) return;

    std::cout << " egde done: " << e->get_name_s () << std::endl;
    active_edges_--;
    assert (active_edges_ >= 0);
  }


  void scheduler::hook_edge_run_fail (sp_t <edge> e)           
  {
    if ( stopped_ ) return;

    active_edges_--;
    assert (active_edges_ >= 0);

    std::cout << " === edge failed: " << e->get_name_s () << std::endl;
    ::exit (2);
  }


  saga::session scheduler::hook_saga_get_session (void)
  {
    util::scoped_lock sl (mtx_);
    return session_;
  }


  void scheduler::thread_work (void)
  {
    // work the node and edge queues:
    // 
    // if 
    //  - work is in the queues  
    //    AND
    //  - not more than max_nodes/max_edges are running
    // then
    //  - start new nodes/edges, removing them from the queue
    while ( true )
    {
      while ( max_nodes_           > active_nodes_ &&
              queue_nodes_.size () > 0             )
      {
        util::scoped_lock sl (mtx_);

        std::cout << " === starting node from queue: "
                  << queue_nodes_.front ()->get_name_s () 
                  << " (" << queue_nodes_.size () << ":" << max_nodes_ << " > " << active_nodes_ << ")" 
                  << std::endl;

        active_nodes_++;

        saga::task t = queue_nodes_.front ()->work_start ();

        tc_nodes_.add_task (t);
        node_task_map_[t] = queue_nodes_.front ();

        queue_nodes_.pop_front ();
      }

      while ( max_edges_           > active_edges_ &&
              queue_edges_.size () > 0             )
      {
        util::scoped_lock sl (mtx_);

        std::cout << " === starting edge from queue: "
                  << queue_edges_.front ()->get_name_s () 
                  << " (" << queue_edges_.size () << ":" << max_edges_ << " > " << active_edges_ << ")" 
                  << std::endl;

        active_edges_++;

        saga::task t = queue_edges_.front ()->work_start ();

        tc_edges_.add_task (t);

        edge_task_map_[t] = queue_edges_.front ();

        queue_edges_.pop_front ();
      }

      ::sleep (1);
    }
  }

  void scheduler::work_finished (saga::task  t, 
                                 std::string flag)
  {
    if ( stopped_ ) return;

    if ( flag == "node" )
    {
      active_nodes_--;
      assert (active_nodes_ >= 0);

      sp_t <node> n = node_task_map_[t];

      std::cout << " === task done: " << n->get_name_s () 
                << " (" << active_nodes_ << ")"
                << std::endl;

      if ( t.get_state () == saga::task::Done )
      {
        n->work_done ();
      }
      else 
      {
        n->work_failed ();
      }
    }
    else if ( flag == "edge" )
    {
      active_edges_--;
      assert (active_edges_ >= 0);

      sp_t <edge> e = edge_task_map_[t];

      std::cout << " === task done: " << e->get_name_s () 
                << " (" << active_edges_ << ")"
                << std::endl;

      if ( t.get_state () == saga::task::Done )
      {
        e->work_done ();
      }
      else 
      {
        e->work_failed ();
      }

    }
    else
    {
      std::cerr << "unknown task finished.  flag: " << flag << std::endl;
      exit (-1);
    }
  }

} // namespace digedag

