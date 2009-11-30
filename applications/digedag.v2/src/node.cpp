
#include <vector>
#include <sstream>

#include <saga/saga.hpp>

#include "util/split.hpp"
#include "util/thread.hpp"
#include "util/scoped_lock.hpp"

#include "node.hpp"


namespace digedag
{
  node::node (digedag::node_description & nd, 
              std::string                  name)
    : nd_      (nd)
    , rm_      ("")
    , name_    (name)
    , state_   (digedag::Pending)
    , is_void_ (false)
  {
    std::stringstream ss;

    if ( ! nd.attribute_exists (digedag::node_attributes::executable) )
    {
      throw ("Cannot handle node w/o executable");
    }

    ss << nd.get_attribute
      (digedag::node_attributes::executable);

    if ( nd.attribute_exists (digedag::node_attributes::arguments) )
    {
      std::vector <std::string> args = nd.get_vector_attribute (digedag::node_attributes::arguments);

      for ( unsigned int i = 0; i < args.size (); i++ )
      {
        ss << " " << args[i];
      }

      cmd_ = ss.str ();
    }
  }

  node::node (std::string cmd, 
              std::string name)
    : rm_      ("")
    , cmd_     (cmd)
    , name_    (name)
    , state_   (digedag::Pending)
    , is_void_ (false)
  {
    // parse cmd into node description
    std::vector <std::string> elems = digedag::split (cmd_);

    nd_.set_attribute (digedag::node_attributes::executable, elems[0]);
    nd_.set_attribute (digedag::node_attributes::interactive, saga::attributes::common_false);

    elems.erase (elems.begin ());

    nd_.set_vector_attribute (digedag::node_attributes::arguments, elems);
  }

  node::node (void)
    : rm_      ("")
    , cmd_     ("-")
    , name_    ("void")
    , state_   (digedag::Pending)
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

  void node::add_edge_in (digedag::edge * e)
  {
    edge_in_.push_back (e);
  }

  void node::add_edge_out (digedag::edge * e)
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
    // dag_->log (std::string ("  ===    node (") + rm_ + "): " + name_ + "   \t ?> " + cmd_);

    // if an incoming edge failed, we can give up
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Failed == edge_in_[i]->get_state () )
      {
        // std::cout << " === edge failed!\n";
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

    dag_->log (std::string (" ===     node : ") + name_ + "   \t -> " + cmd_);

    // Check if node was started before (!Pending).  
    // If not, mark that we start the work (Running).
    {
      if ( Pending != state_ )
      {
        dag_->log (std::string (" ===     node : ") + name_ + " not pending");
        return;
      }
    }

    // all input edges are Done, i.e. all input data are available.  We
    // can thus really execute the node application.
    //
    // So: run the application, in extra thread
    try {
      dag_->log (std::string (" ===     node : ") + name_ + " starting");
      thread_run ();
    }
    catch ( const char* & err )
    {
      dag_->log (std::string (" ===     node : ") + name_ + " failed: " + err);
      state_ = Failed;
    }
  }


  // thread_work is called when the node is fired, and all prerequesites
  // are fullfilled, i.e. all input data are available.  First we execute
  // the node's application, then we fire all outgoing edges, to get data
  // staged out.
  void node::thread_work (void)
  {
    dag_->log (std::string (" ===   node ") + name_ + " starting up");

    // ### scheduler hook
    scheduler_->hook_node_run_pre (dag_, this);

    // we have work to do, an scheduler lets us go ahead
    state_ = Running;

    // TODO: run the saga job for the job description here

    // FIXME: for now, we simply fake work by sleeping for some amount of
    // time
    if ( is_void_ )
    {
      // do nothing
      
      dag_->log (std::string (" ===   node ") + name_ + " is void");

      state_ = Done;
    }
    else
    {
      saga::session session = scheduler_->hook_saga_get_session (dag_);

      // not void: there is work to do
      try {

        // dag_->lock ();

        std::cout << " === creating rm for " << name_ << " - " << rm_ << std::endl;

        saga::job::description jd (nd_);
        saga::job::service     js (session, rm_);
        saga::job::job     j = js.create_job (jd);

        j.run  ();

        // while ( j.get_state () == saga::job::Running )
        // {
        //  // dag_->log (std::string ("       node ") + name_ 
        //                + " : running            : "  + cmd_);
        //   ::sleep (1);
        // }

        j.wait ();

        if ( j.get_state () != saga::job::Done )
        {
          dag_->log (std::string ("       node ") + name_ 
                     + " : job failed - cancel: " + cmd_);

          state_ = Failed;
        
          // ### scheduler hook
          scheduler_->hook_node_run_fail (dag_, this);
        
          return;
        }

        // dag_->unlock ();
      }
      catch ( const saga::exception & e )
      {
        dag_->log (std::string (" ===   node ") + name_ 
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
        // std::cout << " === node fires edges: " << name_ << std::endl;
        // done
        state_ = Done;

        // get data staged out, e.g. fire outgoing edges
        for ( unsigned int i = 0; i < edge_out_.size (); i++ )
        {
          // std::cout << " === node fires edge " << std::endl;
          // edge_out_[i]->dump ();
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

  digedag::node_description node::get_description (void) const
  {
    return nd_;
  }

  void node::set_state (state s)
  {
    state_ = s;
  }

  digedag::state node::get_state (void)
  {
    // check if all input data are ready
    for ( unsigned int i = 0; i < edge_in_.size (); i++ )
    {
      if ( Done != edge_in_[i]->get_state () )
      {
        return Incomplete;
      }
    }

    if ( Done   == state_ ||
         Failed == state_ )
    {
      thread_join ();
    }

    return state_;
  }

  void node::set_pwd (std::string pwd)
  {
    pwd_ = pwd;
    saga::url u_pwd (pwd_);

    nd_.set_attribute (digedag::node_attributes::working_directory, u_pwd.get_path ());

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

  void node::set_rm (std::string rm)
  {
    // std::cout << " === setting rm   to " << rm   << std::endl;
    rm_ = rm;
  }


  void node::set_host (std::string host)
  {
    host_ = host;

    // std::cout << " === setting host to " << host << std::endl;

    std::vector <std::string> chosts;
    chosts.push_back (host);
    nd_.set_vector_attribute (digedag::node_attributes::candidate_hosts, chosts);

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


  void node::set_path (std::string path)
  {
    path_ = path;

    std::vector <std::string> new_env;

    if ( nd_.attribute_exists (digedag::node_attributes::environment) )
    {
      std::vector <std::string> old_env = nd_.get_vector_attribute (digedag::node_attributes::environment);
      bool found = false;

      for ( unsigned int i = 0; i < old_env.size (); i++ )
      {
        std::vector <std::string> words = split (old_env[i], "=");

        if ( words[0] == "PATH" )
        {
          if ( path.empty () )
          {
            new_env.push_back (old_env[i]);
          }
          else
          {
            if ( words.size () == 1 )
            {
              new_env.push_back (words[0] + "=" + path);
            }
            else // assume we have 2 words
            {
              new_env.push_back (words[0] + "=" + words[1] + ":" + path);
            }
          }
          found = true;
        }
        else 
        {
          // not PATH
          new_env.push_back (old_env[i]);
        }
      }
      if ( ! found )
      {
        if ( ! path.empty () )
        {
          new_env.push_back (std::string ("PATH=") + path);
        }
      }
    }
    else
    {
      // no env at all
      if ( ! path.empty () )
      {
        new_env.push_back (std::string ("PATH=") + path);
      }
    }

    // replace env
    nd_.set_vector_attribute (digedag::node_attributes::environment, new_env);

    // FIXME: condor adaptor does not evaluate path.  Thus, we set the
    // executable name to absolute file name
    // Note that this mechanism required rm_ to be set first
    if ( nd_.attribute_exists (digedag::node_attributes::executable) &&
         saga::url (rm_).get_scheme () == "condor" )
    {
      if ( ! path.empty () )
      {
        nd_.set_attribute (digedag::node_attributes::executable,
                           path 
                           + "/" 
                           + nd_.get_attribute (digedag::node_attributes::executable));
      }
    }
  }

  void node::set_dag (digedag::dag * d)
  {
    dag_       = d;
    scheduler_ = dag_->get_scheduler ();
  }

} // namespace digedag

