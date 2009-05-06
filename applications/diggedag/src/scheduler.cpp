
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

namespace diggedag
{
  scheduler::scheduler (void)
  {
    pthread_mutex_t m = mtx_.get ();
  }

  scheduler::~scheduler (void)
  {
  }

  void scheduler::set_scheduler (std::string s)
  {
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
      std::vector <std::string> words = diggedag::split (line);

      if ( words.size () < 1 )
      {
        std::cerr << "parser error in " << policy_ << " at line " + lnum << std::endl;
      }
      else if ( words[0] == "data" )
      {
        if ( words.size () != 4 )
        {
          std::cerr << "parser error in " << policy_ << " at line " + lnum << std::endl;
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
          std::cerr << "parser error in " << policy_ << " at line " + lnum << std::endl;
        }
      }
      else if ( words[0] == "job" )
      {
        if ( words.size () != 4 )
        {
          std::cerr << "parser error in " << policy_ << " at line " + lnum << std::endl;
        }
        else
        {
          job_info_[words[1]].host = words[2];
          job_info_[words[1]].pwd  = words[3];
        }
      }

      lnum++;
    }
  }

  void scheduler::hook_dag_create (diggedag::dag  * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_destroy (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_schedule (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
    
    // walk throgh the dag, and assign execution host for nodes, and data
    // prefixes for edges
    std::map    <std::string, diggedag::node *> nodes = d->get_nodes ();
    std::vector <diggedag::edge *>              edges = d->get_edges ();

    // first, fix pwd and host for INPUT and OUTPUT nodes
    node * input  = nodes["INPUT"];
    node * output = nodes["OUTPUT"];

    input->set_pwd   (data_src_pwd_);
    input->set_host  (data_src_host_);
    output->set_pwd  (data_tgt_pwd_);
    output->set_host (data_tgt_host_);


    // now fix all other nodes, too
    std::map <std::string, diggedag::node *> :: const_iterator it;
    std::map <std::string, diggedag::node *> :: const_iterator begin = nodes.begin ();
    std::map <std::string, diggedag::node *> :: const_iterator end   = nodes.end ();

    for ( it = begin; it != end; it++ )
    {
      std::string id = (*it).first;
      node *      n  = (*it).second;

      if ( job_info_.find (id) != job_info_.end () )
      {
        n->set_host (job_info_[id].host);
        n->set_pwd  (job_info_[id].pwd);
      }
    }


    for ( unsigned int i = 0; i < edges.size (); i++ )
    {
      node * src = edges[i]->get_src_node ();
      node * tgt = edges[i]->get_tgt_node ();

      if ( src != NULL )
      {
        std::string id = src->get_name ();

        if ( job_info_.find (id) != job_info_.end () )
        {
          src->set_pwd (job_info_[id].pwd);
        }
      }

      if ( tgt != NULL )
      {
        std::string id = tgt->get_name ();

        if ( job_info_.find (id) != job_info_.end () )
        {
          tgt->set_pwd (job_info_[id].pwd);
        }
      }
    }
  }


  void scheduler::hook_dag_run_pre (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_post (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_done (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_run_fail (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_dag_wait (diggedag::dag * d)                     
  {
    util::scoped_lock sl (mtx_);
  }



  void scheduler::hook_node_add (diggedag::dag  * d,
                                 diggedag::node * n)           
  {
    pthread_mutex_t m = mtx_.get ();
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_remove (diggedag::dag  * d,
                                    diggedag::node * n)           
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_pre (diggedag::dag  * d,
                                     diggedag::node * n)           
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_node_run_done (diggedag::dag  * d,
                                      diggedag::node * n)           
  {
    util::scoped_lock sl (mtx_);
  }


  // NOTE that this implementation is recursive!  no locks, please...
  void scheduler::hook_node_run_fail (diggedag::dag  * d,
                                      diggedag::node * n)           
  {
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



  void scheduler::hook_edge_add (diggedag::dag  * d,
                                 diggedag::edge * e)           
  {
    util::scoped_lock sl (mtx_);

    // an edge may have an empty src or tgt node.  An empty src node implies
    // that data need to be staged in, from the data_src_ directory .  An empty
    // tgt node implies that data need to be staged out, to the data_tgt_
    // directory.  The latter may need to be created.
    

  }


  void scheduler::hook_node_remove (diggedag::dag  * d,
                                    diggedag::edge * e)           
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_pre (diggedag::dag  * d,
                                     diggedag::edge * e)           
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_done (diggedag::dag  * d,
                                      diggedag::edge * e)           
  {
    util::scoped_lock sl (mtx_);
  }


  void scheduler::hook_edge_run_fail (diggedag::dag  * d,
                                      diggedag::edge * e)           
  {
    util::scoped_lock sl (mtx_);
  }

} // namespace diggedag


