
#include <vector>

#include <saga/saga.hpp>

#include "util/split.hpp"
#include "util/thread.hpp"
#include "util/scoped_lock.hpp"

#include "node.hpp"


namespace diggedag
{
  node::node (diggedag::node_description & nd, 
              std::string                  name)
    : nd_    (nd)
    , name_  (name)
    , state_ (diggedag::Pending)
  {
    // std::cout << "create node " << std::endl;
  }

  node::node (std::string cmd, 
              std::string name)
    : name_  (name)
    , state_ (diggedag::Pending)
  {
    // std::cout << "create node " << std::endl;

    // parse cmd into node description
    std::vector <std::string> elems = diggedag::split (cmd);

    nd_.set_attribute ("Executable", elems[0]);
    nd_.set_attribute ("Interactive", saga::attributes::common_false);

    std::cout << "Exe: " << elems[0] << std::endl;

    elems.erase (elems.begin ());

    nd_.set_vector_attribute ("Arguments", elems);
  }

  node::~node (void)
  {
    // std::cout << "delete node " << std::endl;
    thread_join ();
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
      util::scoped_lock l (mtx_);

      if ( Pending != state_ )
        return;

      // we have work to do...
      state_ = Running;
    }

    // all input edges are Ready, i.e. all input data are available.  We
    // can thus really execute the node application.
    //
    // So: run the application, in extra thread
    thread_run ();
  }


  // thread_work is called when the node is fired, and all prerequesites
  // are fullfilled, i.e. all input data are available.  First we execute
  // the node's application, then we fire all outgoing edges, to get data
  // staged out.
  void node::thread_work (void)
  {
    // ### scheduler hook
    scheduler_->hook_node_run_pre (dag_, this);

    // TODO: run the saga job for the job description here

    // FIXME: for now, we simply fake work by sleeping for some amount of
    // time
    {
      try {
        saga::job::description jd (nd_);

        saga::job::service js;
        saga::job::job j = js.create_job (jd);

        j.run  ();
        j.wait ();

        if ( j.get_state () != saga::job::Done )
        {
          std::cout << "       node " << name_ 
                    << " : job failed - cancel"
                    << jd.get_attribute ("Executable");

          std::vector <std::string> args = jd.get_vector_attribute ("Arguments");

          for ( unsigned int i = 0; i < args.size (); i++ )
          {
            std::cout << " " << args[i];
          }
         
          std::cout << std::endl;

          state_ = Failed;

          // ### scheduler hook
          scheduler_->hook_node_run_fail (dag_, this);

          std::cout << "       node " << name_ << " done" << std::endl;
          return;
        }
        else
        {
          std::cout << "       node " << name_ 
                    << " : job done" << std::endl;
        }
      }
      catch ( const saga::exception & e )
      {
        std::cout << "       node " << name_ 
                  << " : job execution threw exception - cancel\n"
                  << e.what () << std::endl;

        state_ = Failed;

        // ### scheduler hook
        scheduler_->hook_node_run_fail (dag_, this);

        std::cout << "       node " << name_ << " done" << std::endl;
        return;
      }
    }


    {
      util::scoped_lock l (mtx_);

      if ( state_ != Stopped )
      {
        // get data staged out, e.g. fire outgoing edges
        for ( unsigned int i = 0; i < edge_out_.size (); i++ )
        {
          std::cout << "       node " << name_ << " fires edge "
                    << edge_out_[i]->get_src () << "->" 
                    << edge_out_[i]->get_tgt () << std::endl;
          edge_out_[i]->fire ();
        }

        // done
        state_ = Ready;
      }
    }

    // ### scheduler hook
    scheduler_->hook_node_run_done (dag_, this);

    std::cout << "       node " << name_ << " done" << std::endl;
    return;
  }


  void node::stop (void)
  {
    util::scoped_lock l (mtx_);

    state_ = Stopped;
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

