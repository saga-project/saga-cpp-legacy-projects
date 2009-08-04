
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

#define RETRY_MAX 10

namespace digedag
{
  scheduler::scheduler (void)
    :stopped_ (false)
  {
    pthread_mutex_t m = mtx_.get ();
  }

  scheduler::~scheduler (void)
  {
  }

  void scheduler::stop (void)
  {
    stopped_ = true;
  }

  void scheduler::set_scheduler (std::string s)
  {
    if ( stopped_ ) return;

    policy_ = s;

    // open the policy file
    std::fstream fin;
    std::string  line;

    fin.open (policy_.c_str (), std::ios::in);

    if ( fin.fail () )
    {
      std::cerr << "opening " << policy_ << " failed" << std::endl;
      throw "Cannot open file";
    }

    unsigned int lnum = 1;

    while ( std::getline (fin, line) )
    {
      std::vector <std::string> words = digedag::split (line);

      if ( words.size () < 1 )
      {
        std::cerr << "parser error (1) in " << policy_ << " at line " << lnum << std::endl;
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
            std::vector <std::string> elems = digedag::split (words[i], "=", 2);

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

          contexts_.push_back (c);
        }
        catch ( const saga::exception & e )
        {
          // error in handling context line
          std::cerr << "context error in " << policy_ 
                    << " at line " << lnum 
                    << ": \n" << e.what () 
                    << std::endl;
        }
        
      }
      else if ( words[0] == "data" )
      {
        if ( words.size () != 4 )
        {
          std::cerr << "parser error (2) in " << policy_ << " at line " << lnum << std::endl;
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
          std::cerr << "parser error (3) in " << policy_ << " at line " << lnum << std::endl;
        }
      }
      else if ( words[0] == "job" )
      {
        if ( words.size () != 6 )
        {
          std::cerr << "parser error (4) in " << policy_ << " at line " << lnum << std::endl;
        }
        else
        {
          job_info_[words[1]].rm   = words[2];
          job_info_[words[1]].host = words[3];
          job_info_[words[1]].pwd  = words[4];
          job_info_[words[1]].path = words[5];
        }
      }
      

      lnum++;
    }
  }

  void scheduler::hook_dag_create (digedag::dag  * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // we do not simply add contexts to the dag's session, but create a new
    // session with _only_ the contexts we got here.  If we don't have any
    // contexts, we leave the dag's session alone.

    if ( contexts_.size () > 0 )
    {
      try {
        saga::session sd = d->get_session (); // the dag session
        saga::session sf;                     // our fresh session

        // add context to new session.  SAGA's shallow copy comes in handy
        // here
        for ( unsigned int i = 0; i < contexts_.size (); i++ )
        {
          sf.add_context (contexts_[i]);
        }

        // replace the sd impl with the clean sf impl
        sd = sf;
      }
      catch ( const saga::exception & e )
      {
        std::cerr << "cannot add context to dag's session: " << e.what () << std::endl;
      }
    }

  }


  void scheduler::hook_dag_destroy (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_schedule (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // walk throgh the dag, and assign execution host for nodes, and data
    // prefixes for edges
    std::map <node_id_t, node_map_t> nodes = d->get_nodes ();
    std::map <edge_id_t, edge_map_t> edges = d->get_edges ();

    // first, fix pwd and host for INPUT and OUTPUT nodes
    node * input  = nodes["INPUT"];
    node * output = nodes["OUTPUT"];

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
        node *      n  = it->second;

        if ( job_info_.find (id) != job_info_.end () )
        {
          n->set_rm   (job_info_[id].rm);
          n->set_host (job_info_[id].host);
          n->set_pwd  (job_info_[id].pwd);
          n->set_path (job_info_[id].path);
        }
      }
    }
  }


  void scheduler::hook_dag_run_pre (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_post (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_done (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_fail (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_wait (digedag::dag * d)                     
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }



  void scheduler::hook_node_add (digedag::dag  * d,
                                 digedag::node * n)           
  {
    if ( stopped_ ) return;
    pthread_mutex_t m = mtx_.get ();
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_remove (digedag::dag  * d,
                                    digedag::node * n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_pre (digedag::dag  * d,
                                     digedag::node * n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_done (digedag::dag  * d,
                                      digedag::node * n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  // NOTE that this implementation is recursive!  no locks, please...
  void scheduler::hook_node_run_fail (digedag::dag  * d,
                                      digedag::node * n)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // std::string name = n->get_name ();

    // if ( retry_nodes_.find (name) != retry_nodes_.end () )
    // {
    //   if ( retry_nodes_[name] >= RETRY_MAX )
    //   {
    //     // don't retry again - dag just failed
    //     d->set_state (Failed);
    //     // FIXME: should cancel dag
    //   }
    //   else
    //   {
    //     // retry node
    //     retry_nodes_[name]++;
    //     n->set_state (Pending);
    //     std::cout << "retry " << name << std::endl;
    //     n->fire ();
    //   }
    // }
    // else
    // {
    //   // retry the first time
    //   retry_nodes_[name] = 1;
    //   n->set_state (Pending);
    //   n->fire ();
    // }
  }



  void scheduler::hook_edge_add (digedag::dag  * d,
                                 digedag::edge * e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);

    // an edge may have an empty src or tgt node.  An empty src node implies
    // that data need to be staged in, from the data_src_ directory .  An empty
    // tgt node implies that data need to be staged out, to the data_tgt_
    // directory.  The latter may need to be created.


  }


  void scheduler::hook_node_remove (digedag::dag  * d,
                                    digedag::edge * e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_pre (digedag::dag  * d,
                                     digedag::edge * e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_done (digedag::dag  * d,
                                      digedag::edge * e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_fail (digedag::dag  * d,
                                      digedag::edge * e)           
  {
    if ( stopped_ ) return;
    util::scoped_lock sl (mtx_);
  }

} // namespace digedag

