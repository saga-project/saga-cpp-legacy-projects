
#include "edge_impl.hpp"
#include "node.hpp"

namespace diggedag
{
  namespace impl
  {
    edge::edge (const saga::url & src, 
                const saga::url & tgt) 
      : state_ (Pending)
      , src_   (src)
      , tgt_   (tgt)
    {
      // std::cout << "create edge " << uid_get () << std::endl;
    }

    edge::~edge (void)
    {
      // std::cout << "delete edge " << uid_get () << std::endl;
    }

    void edge::add_node (const diggedag::node & n)
    {
      nodes_.push_back (n);
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
      ::sleep (10);

      {
        // signal that we are done
        util::scoped_lock l = thread_scoped_lock ();
        state_ = Ready;
      }

      // if we are done copying data, we fire all dependend nodes
      for ( unsigned int i = 0; i < nodes_.size (); i++ )
      {
        // this fire may succeed or not - that depends on the availability
        // of _other_ input data to that node.  Only if all data are Ready,
        // the fire will actually do anything.  Thus, only the last fire
        // called on a node (i.e. called from its last Pending Edge) will
        // result in a Running node.
        nodes_[i].fire ();
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

  } // namespace impl

} // namespace diggedag

