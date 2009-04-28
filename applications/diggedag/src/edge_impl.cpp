
#include "edge_impl.hpp"

namespace diggedag
{
  namespace impl
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

    void edge::add_src_node (const diggedag::node & src)
    {
      src_node_ = src;
    }

    void edge::add_tgt_node (const diggedag::node & tgt)
    {
      tgt_node_ = tgt;
    }

    // fire() checks if there is still work to do, and if so, starts
    // a thread to do it.
    void edge::fire (void)
    {
      std::cout << "fire   edge " << src_ << "->" << tgt_ <<  std::endl;

      // check if copy was done, or started, before (!Pending).  
      // If not, mark that we start the work (Running)
      {
        util::scoped_lock l = thread_scoped_lock ();

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

      // start the threaded operation
      thread_run ();
    }


    // thread_work is the workload, i.e. the data copy operation
    void edge::thread_work (void)
    {
      // FIXME: perform the real remote saga file copy from src to tgt here
      // (if both are not identical)
      {
        saga::url u_src (src_);
        saga::url u_tgt (tgt_);

        saga::filesystem::file f_src (u_src);
        f_src.copy (u_tgt, saga::filesystem::Overwrite);
      }

      {
        // signal that we are done
        util::scoped_lock l = thread_scoped_lock ();
        state_ = Ready;
      }

      // if we are done copying data, we fire the dependend node
      // this fire may succeed or not - that depends on the availability
      // of _other_ input data to that node.  Only if all data are Ready,
      // the fire will actually do anything.  Thus, only the last fire
      // called on a node (i.e. called from its last Pending Edge) will
      // result in a Running node.
      tgt_node_.fire ();
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

    void edge::set_edge (diggedag::edge & e)
    {
      edge_ = e;
    }

    void edge::set_dag (diggedag::dag & d)
    {
      dag_       = d;
      scheduler_ = dag_.get_scheduler ();
    }

  } // namespace impl

} // namespace diggedag

