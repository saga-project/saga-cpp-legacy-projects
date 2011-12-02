
#include <sys/types.h>
#include <pwd.h>

#include <saga/saga/adaptors/utils.hpp>

#include "master.hpp"


namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    master::master (std::string session)
      : max_id_      (0)
      , session_     (session)
    {
      // get default session name (unix username)
      if ( session_.empty () )
      {
        session_ = ::getpwuid (::getuid ())->pw_name;
      }

      // open master advert dir
      try
      {
        ad_url_  = std::string (ADVERT_BASE_URL) + "/" + session_ + "/";
        ad_      = saga::advert::directory (ad_url_, saga::advert::CreateParents 
                                                   | saga::advert::ReadWrite );

        // pick up existing worker ads
        std::vector <saga::url> entries = ad_.list ();

        for ( unsigned int i = 0; i < entries.size (); i++ )
        {
          advert a (ad_url_ + entries[i].get_string ());
          ads_[a.get_id ()] = a;
        }
      }
      catch ( const saga::exception & e )
      {
        // clean up
        try { ad_.close  (); } catch ( ... ) { } 
        try { ads_.clear (); } catch ( ... ) { } 

        throw saga::no_success (std::string ("master: invalid session?  ") + e.what ());
      }
      
      LOG << "master started: " << ::getpid ();
    }


    ////////////////////////////////////////////////////////////////////
    master::~master (void)
    {
      // note that we do *not* call shutdown, so that the workers are kept alive
      // between individual master runs
    }


    ////////////////////////////////////////////////////////////////////
    void master::shutdown (void)
    {
      // kill all worker
      admap_t :: iterator it;

      // first send QUIT command
      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        worker_stop (it->first);
      }

      // keep state around for now...
      // ad_.remove (saga::advert::Recursive);
    }

    ////////////////////////////////////////////////////////////////////
    id_t master::worker_start (worker_description & d)
    {
      id_t id = max_id_ + 1; // ignore overrun of id_t

      try
      {
        saga::url u = std::string (ADVERT_BASE_URL) + "/" + session_ + "/" + itoa (id);

        // add worker advert URL to job description as last command line
        // argument
        std::vector <std::string> args;

        if ( d.jd.attribute_exists (saga::job::attributes::description_arguments) )
        {
          args = d.jd.get_vector_attribute (saga::job::attributes::description_arguments);
        }

        args.push_back (u.get_string ());

        d.jd.set_vector_attribute (saga::job::attributes::description_arguments, args);


        // create job
        saga::job::service js            (d.rm);
        saga::job::job j = js.create_job (d.jd);


        // register job under new id
        ads_[id] = advert (js, j, u, id);
        max_id_++;


        // advert is created, not job can be started and will register 
        // (in due time)
        j.run ();

        int cnt = 0;
        state s = ads_[id].get_state ();

        while ( s              != Started && 
                j.get_state () == saga::job::Running )
        {
          LOG << "master: waiting for worker " << id << " to register";
          cnt++;
          ::sleep (1);
          s = ads_[id].get_state ();
        }

        if ( ads_[id].get_state () == Started )
        {
          // great, we can use that worker
          ads_[id].set_state (Idle);
        }
        else
        {
          ads_[id].set_state (Failed);
        }

        LOG << "master: worker_start done for " << id;
      }
      catch ( const saga::exception & e )
      {
        throw saga::no_success (std::string ("master: worker_start: error: ") + e.what ());
      }
      
      return id;
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_stop (id_t id)
    {
      try
      {
        // ads_[id].dump ();
        ads_[id].set_task ("quit");

        // let that sink in ;-)
        ::sleep (TIMEOUT);

        if ( ads_[id].get_state () != Quit )
        {
          LOG << " stopping worker " << id;
          ads_[id].get_job ().cancel ();
          ads_[id].set_state  (Failed);
          ads_[id].set_error  ("Worker ignored QUIT command");
        }

        // clean out advert
        // but then also state and error msgs are gone...
        ads_[id].purge ();
      }
      catch ( const saga::exception & e )
      {
        throw saga::no_success (std::string ("cannot stop worker: ") + e.what ());
      }
    }


    ////////////////////////////////////////////////////////////////////
    std::vector <id_t> master::worker_list (void)
    {
      std::vector <id_t> ret;

      admap_t :: iterator it;
      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        ret.push_back (it->first);
      }

      return ret;
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_run (std::string task,
                             argvec_t    args)
    {
      admap_t :: iterator it;

      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        worker_run (it->first, task, args);
      }
    }

                             
    ////////////////////////////////////////////////////////////////////
    void master::worker_run (id_t        id, 
                             std::string task,
                             argvec_t    args)
    {
      // FIXME: checks

      // run task on given worker if worker is idle
      int ok = 0;

      admap_t :: iterator it;
      
      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        if ( id == 0 || id == it->first )
        {
          if ( it->second.get_state () == Idle )
          {
            try
            {
              it->second.run (task, args);
              ok++;
            }
            catch ( const saga::exception & e )
            {
              throw saga::no_success (std::string ("master: worker_run: error: ") + e.what ());
            }
          }
        }
      }

      if ( 0 == ok )
      {
        throw saga::no_success ("master: could not find idle worker to assign task " + task);
      }
    }


    ////////////////////////////////////////////////////////////////////
    state master::worker_get_state (id_t id)
    {
      // FIXME: checks
      return ads_[id].get_state ();
    }


    ////////////////////////////////////////////////////////////////////
    std::string master::worker_get_error (id_t id)
    {
      // FIXME: checks
      return ads_[id].get_error ();
    }


    ////////////////////////////////////////////////////////////////////
    std::string master::worker_get_task (id_t id)
    {
      // FIXME: checks
      return ads_[id].get_task ();
    }


    ////////////////////////////////////////////////////////////////////
    argvec_t master::worker_get_args (id_t id)
    {
      // FIXME: checks
      return ads_[id].get_par_in ();
    }


    ////////////////////////////////////////////////////////////////////
    argvec_t master::worker_get_results (id_t id)
    {
      if ( ads_[id].get_state () != Done )
      {
        throw saga::no_success ("master: cannot get results, state != Done");
      }

      return ads_[id].get_par_out ();
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_wait (id_t id)
    {
      admap_t :: iterator it;

      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        if ( id == 0 || id == it->first )
        {
          it->second.wait ();
        }
      }
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_reset (id_t id)
    {
      admap_t :: iterator it;

      for ( it = ads_.begin (); it != ads_.end (); it++ )
      {
        if ( id == 0 || id == it->first )
        {
          // reset state to idle
          ads_[id].set_state   (Idle);
          ads_[id].set_task    ("");
          ads_[id].set_error   ("");
          ads_[id].set_par_in  (noargs_);
          ads_[id].set_par_out (noargs_);
        }
      }
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_dump (id_t id)
    {
      ads_[id].dump ();
    }

  } // namespace master_worker

} // namespace saga_pm


