
#include "edge.hpp"

namespace diggedag
{
  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : src_   (src)
    , tgt_   (tgt)
    , state_ (Pending)
  {
    if ( tgt_ == "" )
      tgt_ = src_;

    // std::cout << "create edge " << std::endl;
  }

  edge::~edge (void)
  {
    // std::cout << "delete edge " << std::endl;
  }

  void edge::add_src_node (diggedag::node * src)
  {
    src_node_ = src;
  }

  void edge::add_tgt_node (diggedag::node * tgt)
  {
    tgt_node_ = tgt;
  }

  // fire() checks if there is still work to do, and if so, starts
  // to do it, asynchronously
  void edge::fire (void)
  {
    std::cout << "fire   edge " << src_ << "->" << tgt_ <<  std::endl;

    // check if copy was done, or started, before (!Pending).  
    // If not, mark that we start the work (Running)
    {
      if ( Pending != state_ )
        return;

      // check if there is anything to do, at all
      if ( src_ == tgt_ )
      {
        state_ = Ready;
        return;
      }

      // we have work to do...
      state_ = Running;
    }

    // FIXME: perform the real remote saga file copy from src to tgt here
    // (if both are not identical)
    {
      saga::filesystem::file f_src (src_);
      task_ = f_src.copy <saga::task::Async> (tgt_, saga::filesystem::Overwrite);
    }

    // now we add a callback to get notified when the copy is done.  
    task_.add_callback (saga::metrics::task_state, 
                        boost::bind (&edge::cb, this, _1, _2, _3));
  }


  bool edge::cb (saga::monitorable o, 
                 saga::metric      m, 
                 saga::context     c)
  {
    std::cout << " --------------- edge cb " 
              << src_ << " " << tgt_ << std::endl; 

    switch ( task_.get_state () )
    {
      // if we are done copying data, we fire the dependend node
      // this fire may succeed or not - that depends on the availability
      // of _other_ input data to that node.  Only if all data are Ready,
      // the fire will actually do anything.  Thus, only the last fire
      // called on a node (i.e. called from its last Pending Edge) will
      // result in a Running node.
      case saga::task::Done :
        {
          tgt_node_->fire ();

          state_ = Ready;

          // ### scheduler hook
          scheduler_->hook_edge_run_done (dag_, this);

          return false;
        }

      case saga::task::Running :
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
          scheduler_->hook_edge_run_fail (dag_, this);

          return false;
        }
    }
  }


  void edge::erase_src (void)
  {
    // FIXME: remove the src data
  }

  void edge::erase_tgt (void)
  {
    // FIXME: remove the tgt data
  }

  diggedag::state edge::get_state (void) const
  {
    return state_;
  }

  void edge::set_dag (diggedag::dag * d)
  {
    dag_       = d;
    scheduler_ = dag_->get_scheduler ();
    // FIXME: use the scheduler
  }

} // namespace diggedag

