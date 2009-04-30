
#include <vector>

#include <saga/saga.hpp>

#include "util/split.hpp"

#include "node.hpp"


namespace diggedag
{
  node::node (diggedag::node_description & nd)
    : nd_    (nd)
    , name_  ("")
    , state_ (diggedag::Pending)
  {
    // std::cout << "create node " << std::endl;
  }

  node::node (std::string cmd)
    : name_  ("")
    , state_ (diggedag::Pending)
  {
    // std::cout << "create node " << std::endl;

    // parse cmd into node description
    std::vector <std::string> elems = diggedag::split (cmd);

    nd_.set_attribute ("Executable", elems[0]);

    elems.erase (elems.begin ());

    nd_.set_vector_attribute ("Arguments", elems);
  }

  node::~node (void)
  {
    // std::cout << "delete node " << std::endl;
  }

  void node::set_name (const std::string name)
  {
    name_ = name;
  }

  void node::add_edge_in (diggedag::edge * e)
  {
    // std::cout << "edge-i node " << name_ 
    //           << " : "  << e.get_src () 
    //           << " -> " << e.get_tgt () << std::endl;

    edge_in_.push_back (e);
  }

  void node::add_edge_out (diggedag::edge * e)
  {
    // std::cout << "edge-o node " << name_ 
    //           << " : "  << e.get_src () 
    //           << " -> " << e.get_tgt () << std::endl;

    edge_out_.push_back (e);
  }

  // ensure the node application is run.  Before doing that, the input data
  // edges need to be Ready, to ensure that input data are available for the
  // application.  If they are not ready, fire has no effect.
  void node::fire (void)
  {
    std::cout << " >> ============ state" << std::endl;
    std::cout << "fire   node " << name_ << std::endl;

    // ### scheduler hook
    scheduler_->hook_node_run_pre (dag_, this);

    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Ready != edge_in_[i]->get_state () )
      {
        std::cout << "       node " << name_ << " : edge " 
                  << edge_in_[i]->get_src () << "->" 
                  << edge_in_[i]->get_tgt () << " is not ready - cancel fire" << std::endl;
        std::cout << " << 1 ========== state" << std::endl;
        return;
      }
      else
      {
        std::cout << "       node " << name_ << " : edge " 
          << edge_in_[i]->get_src () << "->" 
          << edge_in_[i]->get_tgt () << " is ready" << std::endl;
      }
    }

    // check if node was started before (!Pending).  
    // If not, mark that we start the work (Running)
    {
      if ( Pending != state_ )
      {
        std::cout << " << 2 ========== state" << std::endl;
        return;
      }

      // we have work to do...
      state_ = Running;
    }

    // all input edges are Ready, i.e. all input data are available.  We
    // can thus really execute the node application.
    // First we execute the node's application, then we fire all outgoing edges,
    // to get data staged out.

    // ### scheduler hook
    scheduler_->hook_node_run_pre (dag_, this);
    
    try 
    {
      saga::job::description jd (nd_);

      saga::job::service js;
      job_ = js.create_job (jd);

      job_.run  ();

      // now we add a callback to get notified when the job is done.  
      std::cout << "adding state cb for " << name_ << std::endl;
      job_.add_callback (saga::job::metrics::state, 
                         boost::bind (&node::cb, this, _1, _2, _3));

      std::cout << "       node " << name_ 
                << " : job done" << std::endl; 
    }
    catch ( const saga::exception & e )
    {
      std::cout << "       node " << name_ 
                << " : job execution threw exception - cancel\n"
                << e.what () << std::endl;

      state_ = Failed;

      // ### scheduler hook
      scheduler_->hook_node_run_fail (dag_, this);

      return;
    }
  }


  bool node::cb (saga::monitorable o, 
                 saga::metric      m, 
                 saga::context     c)
  {
    std::cout << " --------------- node cb " << name_ << std::endl; 

    switch ( job_.get_state () )
    {
      case saga::job::Done :
        {
          // If we are done, we get data staged out, e.g. fire outgoing edges
          for ( unsigned int i = 0; i < edge_out_.size (); i++ )
          {
            // std::cout << "       node " << name_ << " fires edge "
            //           << edge_out_[i].get_src () << "->" 
            //           << edge_out_[i].get_tgt () << std::endl;
            edge_out_[i]->fire ();
          }

          state_ = Ready;

          // ### scheduler hook
          scheduler_->hook_node_run_done (dag_, this);

          return false;
        }

      case saga::job::Running :
        {
          // why did we get called???
          state_ = Running;
          return true; // stay registered
        }

      // all other cases are errors...
      default:
        {
          state_ = Failed; // unknown state...

          // ### scheduler hook
          scheduler_->hook_node_run_fail (dag_, this);

          return false;
        }
    }
  }

  std::string node::get_name (void) const
  {
    return name_;
  }

  diggedag::node_description node::get_description (void) const
  {
    return nd_;
  }

  diggedag::state node::get_state (void)
  {
    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Ready != edge_in_[i]->get_state () )
      {
        return Incomplete;
      }
    }

    return state_;
  }


  void node::set_dag (diggedag::dag * d)
  {
    dag_       = d;
    scheduler_ = dag_->get_scheduler ();
  }

} // namespace diggedag

