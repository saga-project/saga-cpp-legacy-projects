
#include "edge.hpp"

#include "util/scoped_lock.hpp"

namespace digedag
{
  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : src_url_  (src)
    , tgt_url_  (tgt)
    , src_path_ (src_url_.get_path ())
    , tgt_path_ (tgt_url_.get_path ())
    , state_    (Pending)
  {
    if ( tgt_url_ == "" )
    {
      tgt_url_ = src_url_;
      tgt_path_ = tgt_url_.get_path ();
    }
  }

  edge::~edge (void)
  {
    // thread_join ();
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
         scheduler_ == e.scheduler_   )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void edge::add_src_node (sp_t <node> src)
  {
    src_node_ = src;
  }

  void edge::add_tgt_node (sp_t <node> tgt)
  {
    tgt_node_ = tgt;
  }

  void edge::dryrun (void)
  {
    if ( Pending != state_ )
      return;

    if ( ! (src_url_ == tgt_url_) )
    {
      dump ();
    }

    state_ = Done;

    tgt_node_->dryrun ();
  }


  void edge::reset (void)
  {
    state_ = Pending;
    tgt_node_->reset ();
  }


  // fire() checks if there is still work to do, and if so, starts
  // a thread to do it.
  void edge::fire (void)
  {
    // ### scheduler hook
    scheduler_->hook_edge_run_pre (*this);

    // check if copy was done, or started, before (!Pending).  
    // If not, mark that we start the work (Running)
    {
      if ( Pending != state_ )
        return;

      // check if there is anything to do, at all
      if ( src_url_ == tgt_url_ )
      {
        state_ = Done;

        // fire dependent node
        // std::cout << " === edge has nothing to do, fires tagt node: "  << tgt_node_->get_name () << std::endl;
        tgt_node_->fire ();

        // ### scheduler hook
        scheduler_->hook_edge_run_done (*this);

        return;
      }
      else
      {
        // we have work to do...
        state_ = Running;

        // start the threaded operation
        work ();
        state_ = Done;
      }
    }
  }


  // work is the workload, i.e. the data copy operation
  void edge::work (void)
  {
    try 
    {
      std::cout << std::string (" === edge run: ")
                << get_name_s () << std::endl;

      // dump ();

      // the local file adaptor is not thread save if operating on the same
      // directory structure

      saga::session session = scheduler_->hook_saga_get_session ();

      saga::filesystem::file f_src (session, src_url_);


      // first check if file exists
      bool exists = false;
      try 
      {
        std::cout << " === edge ck1: " << src_url_ << " -> " << tgt_url_ << std::endl;
        
        saga::filesystem::file f_tgt (session, tgt_url_);

        std::cout << " === edge ck2: " << src_url_ << " -> " << tgt_url_ << std::endl;

        if ( f_tgt.get_size () == f_src.get_size () )
        {

          std::cout << " === edge ck3: " << src_url_ << " -> " << tgt_url_ << std::endl;
          exists = true;
        }
        else
        {
          std::cout << " === edge ck4: " << src_url_ << " -> " << tgt_url_ << std::endl;
        }
      }
      catch ( ... )
      {
        // does not exist
      }

      if ( ! exists )
      {
        // f_src.copy (tgt_url_, saga::filesystem::Overwrite
        //             | saga::filesystem::CreateParents);
      }
    }
    catch ( const saga::exception & e ) 
    {
      // FIXME: the local adaptor is not doing nicely in multithreaded
      // environments.  Thus, we ignore all errors for now, and rely on the
      // ability of the nodes to flag any missing data files.
#if  1
      std::cout << std::string (" === edge failed to copy data ")
                << get_name_s () << "\n" 
                << e.what () << std::endl;

      {
        state_ = Failed;
      }

      // ### scheduler hook
      scheduler_->hook_edge_run_fail (*this);

      return;
#else

      state_ = Done;

      // ### scheduler hook
      scheduler_->hook_edge_run_done (*this);
#endif
    }

    // if we are done copying data, we fire the dependend node
    // this fire may succeed or not - that depends on the availability
    // of _other_ input data to that node.  Only if all data are Done,
    // the fire will actually do anything.  Thus, only the last fire
    // called on a node (i.e. called from its last Pending Edge) will
    // result in a Running node.
    
    {
      std::cout << std::string (" === edge done: ")
                << get_name_s () << std::endl;

      if ( state_ != Stopped )
      {
        // done
        state_ = Done;

        // fire dependent node
        // std::cout << " === firing dep node " << tgt_node_->get_name () << std::endl;
        tgt_node_->fire ();
      }
    }

    // ### scheduler hook
    scheduler_->hook_edge_run_done (*this);

    return;
  }


  void edge::stop (void)
  {
    state_ = Stopped;
  }

  void edge::dump (void)
  {
    std::cout << std::string ("         edge : ")
              << src_node_->get_name () << "\t -> " << tgt_node_->get_name () 
              << "[" << src_url_.get_string ()       << "\t -> " << tgt_url_.get_string () << "] "
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

  void edge::set_state (state s)
  {
    state_ = s;
  }

  state edge::get_state (void)
  {
    if ( Done   == state_ ||
         Failed == state_ )
    {
      // thread_join ();
    }

    return state_;
  }

  std::string edge::get_name_s (void) const
  {
    return src_node_->get_name () + "->" + tgt_node_->get_name ();
  }

  edge_id_t edge::get_name (void) const
  {
    return edge_id_t (src_node_->get_name (), tgt_node_->get_name ());
  }

  void edge::set_pwd_src (std::string pwd)
  {
    src_url_.set_path (pwd  + src_path_);
  }

  void edge::set_pwd_tgt (std::string pwd)
  {
    tgt_url_.set_path (pwd  + tgt_path_);
  }

  void edge::set_host_src (std::string host) 
  {
    src_url_.set_host (host);
  }

  void edge::set_host_tgt (std::string host) 
  {
    tgt_url_.set_host (host);
  }


  void edge::set_scheduler (sp_t <scheduler> s)
  {
    scheduler_ = s;
  }

} // namespace digedag

