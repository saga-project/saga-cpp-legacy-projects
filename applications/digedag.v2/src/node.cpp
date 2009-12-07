
#include <vector>
#include <sstream>

#include <saga/saga.hpp>

#include "util/split.hpp"

#include "node.hpp"
#include "scheduler.hpp"


namespace digedag
{
  node::node (node_description & nd, 
              std::string        name, 
              sp_t <scheduler>   scheduler,
              saga::session      session)
    : created_   (     false)
    , nd_        (        nd)
    , rm_        (        "")
    , name_      (      name)
    , state_     (Incomplete)
    , is_void_   (     false)
    , fired_     (     false)
    , t_valid_   (     false)
    , scheduler_ ( scheduler)
    , session_   (   session)
  {
    std::stringstream ss;

    if ( ! nd.attribute_exists (node_attributes::executable) )
    {
      throw ("Cannot handle node w/o executable");
    }

    ss << nd.get_attribute
      (node_attributes::executable);

    if ( nd.attribute_exists (node_attributes::arguments) )
    {
      std::vector <std::string> args = nd.get_vector_attribute (node_attributes::arguments);

      for ( unsigned int i = 0; i < args.size (); i++ )
      {
        ss << " " << args[i];
      }

      cmd_ = ss.str ();
    }
  }

  node::node (std::string      cmd, 
              std::string      name,
              sp_t <scheduler> scheduler,
              saga::session    session)
    : created_   (     false)
    , rm_        (        "")
    , cmd_       (       cmd)
    , name_      (      name)
    , state_     (Incomplete)
    , is_void_   (     false)
    , fired_     (     false)
    , t_valid_   (     false)
    , scheduler_ ( scheduler)
    , session_   (   session)
  {
    // parse cmd into node description
    std::vector <std::string> elems = split (cmd_);

    nd_.set_attribute (node_attributes::executable, elems[0]);
    nd_.set_attribute (node_attributes::interactive, saga::attributes::common_false);

    elems.erase (elems.begin ());

    nd_.set_vector_attribute (node_attributes::arguments, elems);
  }

  node::node (sp_t <scheduler> scheduler, 
              saga::session    session)
    : created_   (     false)
    , rm_        (        "")
    , cmd_       (       "-")
    , name_      (    "void")
    , state_     (Incomplete)
    , is_void_   (      true)
    , fired_     (     false)
    , t_valid_   (     false)
    , scheduler_ ( scheduler)
    , session_   (   session)
  {
  }

  node::~node (void)
  {
  }

  void node::set_name (const std::string name)
  {
    name_ = name;
  }

  void node::add_edge_in (sp_t <edge> e)
  {
    edge_in_.push_back (e);
  }

  void node::add_edge_out (sp_t <edge> e)
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

    std::cout << std::string ("         node : ") << name_ << "   \t -> " << cmd_ << std::endl;

    state_ = Done;

    for ( unsigned int i = 0; i < edge_out_.size (); i++ )
    {
      edge_out_[i]->dryrun ();
    }
  }


  void node::reset (void)
  {
    state_ = Incomplete;

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
    // update state
    get_state ();

    // Check if node was started before (!Pending).  
    // If not, mark that we start the work (Running).
    if ( Pending != state_ )
    {
      // std::cout << std::string (" ===     node : ") << name_ << " not pending" << std::endl;
      std::cout << std::string (" ===     node : ") << name_ << ": " << state_to_string (state_) << std::endl;
      return;
    }

    // all input edges are Done, i.e. all input data are available.  We
    // can thus really execute the node application.
    //
    // So: run the application, in extra thread
    std::cout << std::string (" ===   node ") << name_ << " starting up" << std::endl;

    // ### scheduler hook - leave it to the scheduler to call our work routine
    scheduler_->hook_node_run_pre (shared_from_this ());
  }

  saga::task node::work_start (void)
  {
    if ( state_ == Stopped )
      return task_;

    assert ( state_ == Pending );

    // we have work to do, an scheduler lets us go ahead
    state_ = Running;

    if ( is_void_ )
    {
      // do nothing
      
      std::cout << std::string (" ===   node ") << name_ << " is void" << std::endl;
      
      // FIXME: we can't fake a noop task :-(
      saga::filesystem::directory d (session_, "any://localhost//");

      task_ = d.get_url <saga::task::Async> ();
      t_valid_ = true;

      std::cout << " === fake task created: " 
                << task_.get_id () << " - " 
                << task_.get_state () << std::endl;
    }
    else
    {
      saga::job::description jd (nd_);

      jd.set_attribute (saga::job::attributes::description_working_directory,  "/tmp/0/");
   // jd.set_attribute (saga::job::attributes::description_interactive,  "true");
   // jd.set_attribute (saga::job::attributes::description_input,        "/dev/null");
   // jd.set_attribute (saga::job::attributes::description_output,       
   //                   std::string ("/tmp/out.") + get_name ());
   // jd.set_attribute (saga::job::attributes::description_error,       
   //                   std::string ("/tmp/err.") + get_name ());
 
      saga::job::service js (session_, rm_);
      saga::job::job     j = js.create_job (jd);

      j.run  ();

      task_ = j;
      t_valid_ = true;
    }

    return task_;
  }


  void node::work_done (void)
  {
    if ( state_ == Stopped )
      return;

    assert ( state_ != Failed );
    assert ( state_ != Done   );

    state_ = Done;

    std::cout << std::string (" === node done: ")
              << get_name_s () << std::endl;

    if ( state_ == Done && ! fired_ )
    {
      fired_ = true;

      // get data staged out, e.g. fire outgoing edges
      for ( unsigned int i = 0; i < edge_out_.size (); i++ )
      {
        std::cout << " === node " << get_name () << " fires edge " 
                  << edge_out_[i]->get_name_s () << std::endl;

        edge_out_[i]->fire ();
      }
    }

    // ### scheduler hook
    scheduler_->hook_node_run_done (shared_from_this ());

    return;
  }

  void node::work_failed (void)
  {
    if ( state_ == Stopped )
      return;

    assert ( state_ != Failed );
    assert ( state_ != Done   );

    try 
    {
      task_.rethrow ();
    }
    catch ( const saga::exception & e )
    {
      std::cout << " === node " << get_name_s () 
                << " set to failed by scheduler: "
                << e.what () << std::endl;
    }

    state_ = Failed;
  }


  void node::stop ()
  {
    if ( t_valid_ )
    {
      task_.cancel ();
    }

    state_ = Stopped;
  }

  void node::dump (bool deep)
  {
    std::cout << std::string ("       node ") << name_ << "(" << host_ << ", " 
               << pwd_ +")" << " (" << state_to_string (get_state ()) << ")" << std::endl;

    if ( deep )
    {
      std::cout << " edges in:" << std::endl;
      for ( unsigned int i = 0; i < edge_in_.size (); i++ )
      {
        edge_in_[i]->dump ();
      }

      std::cout << " edges out:" << std::endl;
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

  std::string node::get_name_s (void) const
  {
    return name_;
  }

  node_description node::get_description (void) const
  {
    return nd_;
  }

  void node::set_state (state s)
  {
    state_ = s;
  }


  // FIXME: it is not nice to have such fundamental side effects in get_state!
  // That code needs to eventually move into a callback on the job state metric.
  state node::get_state (void)
  {
    if ( state_ == Incomplete )
    {
      // check if any input data failed
      for ( unsigned int i = 0; i < edge_in_.size (); i++ )
      {
        if ( Failed == edge_in_[i]->get_state () )
        {
          std::cout << " === node " << get_name_s () << " failed due to failing edge " 
                    << edge_in_[i]->get_name_s () << std::endl;
          state_ = Failed;
          return state_;
        }
      }


      // check if all input data are ready
      for ( unsigned int i = 0; i < edge_in_.size (); i++ )
      {
        if ( Done != edge_in_[i]->get_state () )
        {
          state_ = Incomplete;
          return state_;
        }
      }

      // no dep failed, all Done - we are pending!
      state_ = Pending;
    }


    // we can only depend the node state from the job state if a job was
    // actually created
    if ( created_ )
    {
      switch ( j_.get_state () )
      {
        case saga::job::New:
          std::cout << " === node is almost running: " << name_ << std::endl;
          state_ = Pending;
          break;

        case saga::job::Running:
          // std::cout << " === node is running: " << name_ << std::endl;
          state_ = Running;
          // FIXME
          // ::sleep (1);
          break;

        case saga::job::Done:
          state_ = Done;
          j_.cancel ();
          scheduler_->hook_node_run_done (shared_from_this ());
          break;


          // Canceled, Failed, Unknown, New - all invalid
        default:
          state_ = Failed;
          j_.cancel ();

          std::cout << std::string ("       node ") << name_ 
            << " : job failed - cancel: " << cmd_ << std::endl;

          // ### scheduler hook
          scheduler_->hook_node_run_fail (shared_from_this ());

          break;

      } // switch
    }

    return state_;
  }

  void node::set_pwd (std::string pwd)
  {
    pwd_ = pwd;
    saga::url u_pwd (pwd_);

    nd_.set_attribute (node_attributes::working_directory, u_pwd.get_path ());

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
    nd_.set_vector_attribute (node_attributes::candidate_hosts, chosts);

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

    if ( nd_.attribute_exists (node_attributes::environment) )
    {
      std::vector <std::string> old_env = nd_.get_vector_attribute (node_attributes::environment);
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
    nd_.set_vector_attribute (node_attributes::environment, new_env);

    // FIXME: condor adaptor does not evaluate path.  Thus, we set the
    // executable name to absolute file name
    // Note that this mechanism required rm_ to be set first
    if ( nd_.attribute_exists (node_attributes::executable) &&
         saga::url (rm_).get_scheme () == "condor" )
    {
      if ( ! path.empty () )
      {
        nd_.set_attribute (node_attributes::executable,
                           path + "/" + nd_.get_attribute (node_attributes::executable));
      }
    }
  }
} // namespace digedag

