
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
    : stopped_       (false),
      session_       ( true),
      dag_           (    d)
  {
    pthread_mutex_t m = mtx_.get ();

    src_ = src;
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

    ::exit (3);
  }


  void scheduler::hook_dag_wait (void)
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }



  void scheduler::hook_node_add (node & n)           
  {
    if ( stopped_ ) return;
    pthread_mutex_t m = mtx_.get ();
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_remove (node & n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_pre (node & n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_done (node & n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  // NOTE that this implementation is recursive!  no locks, please...
  void scheduler::hook_node_run_fail (node & n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    ::exit (1);
  }



  void scheduler::hook_edge_add (edge & e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // an edge may have an empty src or tgt node.  An empty src node implies
    // that data need to be staged in, from the data_src_ directory .  An empty
    // tgt node implies that data need to be staged out, to the data_tgt_
    // directory.  The latter may need to be created.


  }


  void scheduler::hook_node_remove (edge & e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_pre (edge & e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_done (edge & e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_fail (edge & e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    ::exit (2);
  }


  saga::session scheduler::hook_saga_get_session (void)
  {
    util::scoped_lock sl (mtx_);
    return session_;
  }

} // namespace digedag

