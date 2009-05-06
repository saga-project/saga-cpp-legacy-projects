
#include <vector>
#include <sstream>

#include <saga/saga.hpp>

#include "util/split.hpp"
#include "util/thread.hpp"
#include "util/scoped_lock.hpp"

#include "node.hpp"


namespace diggedag
{
  node::node (diggedag::node_description & nd, 
              std::string                  name)
    : nd_      (nd)
    , name_    (name)
    , state_   (diggedag::Pending)
    , is_void_ (false)
  {
    std::stringstream ss;

    ss << nd.get_attribute ("Executable");

    std::vector <std::string> args = nd.get_vector_attribute ("Arguments");

    for ( unsigned int i = 0; i < args.size (); i++ )
    {
      ss << " " << args[i];
    }

    cmd_ = ss.str ();
  }

  node::node (std::string cmd, 
              std::string name)
    : cmd_     (cmd)
    , name_    (name)
    , state_   (diggedag::Pending)
    , is_void_ (false)
  {
    // parse cmd into node description
    std::vector <std::string> elems = diggedag::split (cmd_);

    nd_.set_attribute ("Executable", elems[0]);
    nd_.set_attribute ("Interactive", saga::attributes::common_false);

    elems.erase (elems.begin ());

    nd_.set_vector_attribute ("Arguments", elems);
  }

  node::node (void)
    : cmd_     ("-")
    , name_    ("void")
    , state_   (diggedag::Pending)
    , is_void_ (true)
  {
  }

  node::~node (void)
  {
    thread_join ();
  }

  void node::set_name (const std::string name)
  {
    name_ = name;
  }

  void node::add_edge_in (diggedag::edge * e)
  {
    edge_in_.push_back (e);
  }

  void node::add_edge_out (diggedag::edge * e)
  {
    edge_out_.push_back (e);
  }


  void node::dryrun (void)
  {
    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Done != edge_in_[i]->get_state () )
      {
        return;
      }
    }

    if ( Pending != state_ )
      return;

    dag_->log (std::string ("         node : ") + name_ + "   \t -> " + cmd_);

    state_ = Done;

    for ( unsigned int i = 0; i < edge_out_.size (); i++ )
    {
      edge_out_[i]->dryrun ();
    }
  }


  void node::reset (void)
  {
    state_ = Pending;

    for ( unsigned int i = 0; i < edge_out_.size (); i++ )
    {
      edge_out_[i]->reset ();
    }
  }


  // ensure the node application is run.  Before doing that, the input data
  // edges need to be Done, to ensure that input data are available for the
  // application.  If they are not ready, fire has no effect.
  void node::fire (void)
  {
    // ### scheduler hook
    scheduler_->hook_node_run_pre (dag_, this);

    // if an incoming edge failed, we can give up
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Failed == edge_in_[i]->get_state () )
      {
        state_ = Failed;

        return;
      }
    }

    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Done != edge_in_[i]->get_state () )
      {
        return;
      }
    }

    dag_->log (std::string ("         node : ") + name_ + "   \t -> " + cmd_);

    // Check if node was started before (!Pending).  
    // If not, mark that we start the work (Running).
    {
      if ( Pending != state_ )
        return;

      // we have work to do...
      state_ = Running;
    }

    // all input edges are Done, i.e. all input data are available.  We
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
    if ( is_void_ )
    {
      // do nothing
    }
    else
    {
      // not void: there is work to do
      try {
        saga::job::description jd (nd_);

        saga::job::service js;
        saga::job::job j = js.create_job (jd);

        j.run  ();
        j.wait ();

        if ( j.get_state () != saga::job::Done )
        {
          dag_->log (std::string ("       node ") + name_ 
                    + " : job failed - cancel: "  + cmd_);

          state_ = Failed;

          // ### scheduler hook
          scheduler_->hook_node_run_fail (dag_, this);

          return;
        }
      }
      catch ( const saga::exception & e )
      {
        dag_->log (std::string ("       node ") + name_ 
                   + " : job threw - cancel: "  + e.what ());

        state_ = Failed;

        // ### scheduler hook
        scheduler_->hook_node_run_fail (dag_, this);

        return;
      }
    }


    {
      if ( state_ != Stopped )
      {
        // done
        state_ = Done;

        // get data staged out, e.g. fire outgoing edges
        for ( unsigned int i = 0; i < edge_out_.size (); i++ )
        {
          edge_out_[i]->fire ();
        }
      }
    }

    // ### scheduler hook
    scheduler_->hook_node_run_done (dag_, this);

    return;
  }


  void node::stop (void)
  {
    state_ = Stopped;
  }

  void node::dump (bool deep)
  {
    dag_->log (std::string ("       node ") + name_ + "(" + host_ + ", " 
               + pwd_ +")" + " (" + state_to_string (get_state ()) + ")");

    if ( deep )
    {
      dag_->log (" edges in:");
      for ( unsigned int i = 0; i < edge_in_.size (); i++ )
      {
        edge_in_[i]->dump ();
      }

      dag_->log (" edges out:");
      for ( unsigned int i = 0; i < edge_out_.size (); i++ )
      {
        edge_out_[i]->dump ();
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

  void node::set_state (state s)
  {
    state_ = s;
  }

  diggedag::state node::get_state (void)
  {
    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Done != edge_in_[i]->get_state () )
      {
        return Incomplete;
      }
    }

    return state_;
  }

  void node::set_pwd (std::string pwd)
  {
    pwd_ = pwd;
    saga::url u_pwd (pwd_);
    nd_.set_attribute ("WorkingDirectory", u_pwd.get_path ());

    // set pwd for all incoming and outgoing edges
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      edge_in_[i]->set_pwd_tgt (pwd);
    }

    for ( unsigned int i = 0; i < edge_out_.size (); i++ )
    {
      edge_out_[i]->set_pwd_src (pwd);
    }
  }

  void node::set_host (std::string host)
  {
    host_ = host;

    std::vector <std::string> chosts;
    chosts.push_back (host);
    nd_.set_vector_attribute ("CandidateHosts", chosts);

    // set host for all incoming and outgoing edges
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      edge_in_[i]->set_host_tgt (host);
    }

    for ( unsigned int i = 0; i < edge_out_.size (); i++ )
    {
      edge_out_[i]->set_host_src (host);
    }
  }

  void node::set_dag (diggedag::dag * d)
  {
    dag_       = d;
    scheduler_ = dag_->get_scheduler ();
  }

} // namespace diggedag

