
#include "edge.hpp"

#include "util/scoped_lock.hpp"

namespace diggedag
{
  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : src_url_  (src)
    , tgt_url_  (tgt)
    , src_path_ (src_url_.get_path ())
    , tgt_path_ (tgt_url_.get_path ())
    , state_    (Pending)
    , src_node_ (NULL)
    , tgt_node_ (NULL)
  {
    if ( tgt_url_ == "" )
    {
      tgt_url_ = src_url_;
      tgt_path_ = tgt_url_.get_path ();
    }

    // std::cout << "create edge " << std::endl;
  }

  edge::~edge (void)
  {
    // std::cout << "delete edge " << std::endl;
    thread_join ();
  }

  bool edge::operator== (const edge & e)
  {
    if ( src_url_   == e.src_url_     &&
         tgt_url_   == e.tgt_url_     &&
         src_path_  == e.src_path_    &&
         tgt_path_  == e.tgt_path_    &&
         state_     == e.state_       &&
      // src_node_  == e.src_node_    &&
      // tgt_node_  == e.tgt_node_    &&
         dag_       == e.dag_         &&
         scheduler_ == e.scheduler_   )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void edge::add_src_node (diggedag::node * src)
  {
    src_node_ = src;
  }

  void edge::add_tgt_node (diggedag::node * tgt)
  {
    tgt_node_ = tgt;
  }

  void edge::dryrun (void)
  {
    std::cout << "         edge : " 
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "[" << src_url_        << "\t -> " << tgt_url_  << "]"
              << std::endl;
    std::cout << "                run  " << tgt_node_->get_name () << std::endl;

    tgt_node_->dryrun ();
  }


  // fire() checks if there is still work to do, and if so, starts
  // a thread to do it.
  void edge::fire (void)
  {
    std::cout << "         edge : " 
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "[" << src_url_        << "\t -> " << tgt_url_  << "] "
              << state_to_string (state_) 
              << std::endl;

    // ### scheduler hook
    scheduler_->hook_edge_run_pre (dag_, this);

    // check if copy was done, or started, before (!Pending).  
    // If not, mark that we start the work (Running)
    {
      util::scoped_lock l (mtx_);

      if ( Pending != state_ )
      {
        std::cout << "edge is ! pending - fire canceled" << std::endl;
        return;
      }

      // check if there is anything to do, at all
      if ( src_url_ == tgt_url_ )
      {
        std::cout << "         edge : copy not needed. "
                  << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
                  << " fire node " << tgt_node_->get_name () << std::endl;
        
        state_ = Done;

        // fire dependent node
        tgt_node_->fire ();

        // ### scheduler hook
        scheduler_->hook_edge_run_done (dag_, this);

        return;
      }
      else
      {
        std::cout << "         edge : copy needed, starting thread "
                  << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
                  << std::endl;

        // we have work to do...
        state_ = Running;

        // start the threaded operation
        thread_run ();
      }
    }
  }


  // thread_work is the workload, i.e. the data copy operation
  void edge::thread_work (void)
  {
    std::cout << " ###     edge : " 
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "[" << src_url_        << "\t -> " << tgt_url_  << "]"
              << std::endl << std::flush;

    // FIXME: perform the real remote saga file copy from src to tgt here
    // (if both are not identical)
    try 
    {
      std::cout << " ### trying to copy " << src_url_ << " to " << tgt_url_ << std::endl;

      saga::filesystem::file f_src (src_url_);
      f_src.copy (tgt_url_, saga::filesystem::Overwrite
                          | saga::filesystem::CreateParents);
    }
    catch ( const saga::exception & e ) 
    {
#if  0
      std::cerr << "edge failed to copy data " 
                << src_url_ << "->" << tgt_url_ 
                <<  std::endl
                << e.what () << std::endl;

      {
        util::scoped_lock l (mtx_);

        state_ = Failed;
      }

      // ### scheduler hook
      scheduler_->hook_edge_run_fail (dag_, this);
#else
      {
        util::scoped_lock l (mtx_);

        state_ = Done;
      }

      // ### scheduler hook
      scheduler_->hook_edge_run_done (dag_, this);
#endif

      return;
    }

    // if we are done copying data, we fire the dependend node
    // this fire may succeed or not - that depends on the availability
    // of _other_ input data to that node.  Only if all data are Done,
    // the fire will actually do anything.  Thus, only the last fire
    // called on a node (i.e. called from its last Pending Edge) will
    // result in a Running node.
    
    {
      if ( state_ != Stopped )
      {

        std::cout << "         edge : "
                  << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
                  << " fire node " << tgt_node_->get_name () << std::endl;
        
        // done
        state_ = Done;

        // fire dependent node
        tgt_node_->fire ();
      }
    }

    // ### scheduler hook
    scheduler_->hook_edge_run_done (dag_, this);

    std::cout << "         edge : "
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "done" << std::endl;
    return;
  }


  void edge::stop (void)
  {
    util::scoped_lock l (mtx_);

    state_ = Stopped;
  }

  void edge::dump (void)
  {
    std::cout << "         edge : " 
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "[" << src_url_        << "\t -> " << tgt_url_  << "] "
              << "(" << state_to_string (get_state ()) << ")"
              << std::endl;
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


  void edge::set_pwd_src (std::string pwd)
  {
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "edge sets src pwd to: " << pwd << std::endl;
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "     - " << src_url_ << std::endl;
    src_url_.set_path (pwd  + src_path_);
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "     + " << src_url_ << std::endl;
  }

  void edge::set_pwd_tgt (std::string pwd)
  {
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "edge sets tgt pwd to: " << pwd << std::endl;
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "     - " << tgt_url_ << std::endl;
    tgt_url_.set_path (pwd  + tgt_path_);
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "     + " << tgt_url_ << std::endl;
  }

  void edge::set_host_src (std::string host) 
  {
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "edge sets src host to: " << host << std::endl;
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "     - " << src_url_ << std::endl;
    src_url_.set_host (host);
    // if ( src_node_->get_name () == "INPUT" ) std::cout << "     + " << src_url_ << std::endl;
  }

  void edge::set_host_tgt (std::string host) 
  {
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "edge sets tgt host to: " << host << std::endl;
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "     - " << tgt_url_ << std::endl;
    tgt_url_.set_host (host);
    // if ( tgt_node_->get_name () == "OUTPUT" ) std::cout << "     + " << tgt_url_ << std::endl;
  }



  void edge::set_dag (diggedag::dag * d)
  {
    dag_       = d;
    scheduler_ = dag_->get_scheduler ();
    // FIXME: use the scheduler
  }

} // namespace diggedag

