
#include <saga/saga/adaptors/utils.hpp>

#include "master.hpp"

#define TIMEOUT 5 // seconds to wait for worker to react on a task


namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    master::master (void)
      : initialized_ (false)
      , max_id_      (0)
      , session_     ("")
    {
      // master: spawn off worker
      std::cout << "master c'tor" << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    master::~master (void)
    {
      // note that we do *not* call shutdown, so that the workers are kept alive
      // between individual master runs
    }

    ////////////////////////////////////////////////////////////////////
    void master::initialize (std::string session)
    {
      std::cout << "master init" << std::endl;
      if ( initialized_ )
      {
        std::cerr << "master: initialize failed : already initialized session " 
                  << session_ << std::endl;
      }
      else
      {
        // get default session name (uid)
        if ( session.empty () )
        {
          session = itoa (::getuid ());
        }

        // open master advert dir
        try
        {
          session_ = session;
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

          initialized_ = true;
        }
        catch ( const saga::exception & e )
        {
          // clean up
          try { ad_.close  (); } catch ( ... ) { } 
          try { ads_.clear (); } catch ( ... ) { } 

          std::cerr << " master: cannot create / open / reconnect MW session at " 
                    << ad_url_ << " : \n" << e.what () << std::endl;
        }
      }
      std::cout << "master init done" << std::endl;
    }

    ////////////////////////////////////////////////////////////////////
    void master::shutdown (void)
    {
      if ( initialized_ )
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
    }

    ////////////////////////////////////////////////////////////////////
    id_t master::worker_start (worker_description & d)
    {
      id_t id = max_id_ + 1; // ignore overrun of id_t

      if ( ! initialized_ )
      {
        std::cerr << "cannot run worker before initilize()" << std::endl;
        id = 0;
      }
      else
      {
        std::cout << "master: worker_start" << std::endl;

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
          ads_[id].dump ();
          while ( s != Started )
          {
            cnt++;
            std::cout << "master: waiting for worker to register" << cnt << std::endl;
            ::sleep (TIMEOUT);
            s = ads_[id].get_state ();
          }

          if ( ads_[id].get_state () == Started )
          {
            // great, we can use that worker
            ads_[id].set_state (Idle);
            std::cout << "master: worker registered, now idle " <<  std::endl;
            std::cout << "----------------------" << std::endl;
            ads_[id].dump ();
            std::cout << "----------------------" << std::endl;
          }
          else
          {
            ads_[id].set_state (Failed);
          }

          std::cout << "master: worker_start done" << std::endl;
        }
        catch ( const saga::exception & e )
        {
          std::cerr << " master: cannot run worker :\n" << e.what () << std::endl;
          id = 0;
        }
      }

      return id;
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_stop (id_t id)
    {
      if ( ! initialized_ )
      {
        std::cerr << "cannot stop worker before initilize()" << std::endl;
      }
      else
      {
        try
        {
          std::cout << " quitting worker " << id
                    << " (" << state_to_string (ads_[id].get_state ()) << ")" 
                    << std::endl;

          // ads_[id].dump ();
          ads_[id].set_task ("quit");

          // let that sink in ;-)
          ::sleep (TIMEOUT);

          if ( ads_[id].get_state () != Quit )
          {
            std::cout << " stopping worker " << id << std::endl;
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
          std::cerr << " master: cannot stop worker :\n" << e.what () << std::endl;
          id = 0;
        }
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
                             id_t        id)
    {
      argvec_t    args;
      worker_run (task, args, id);
    }

    ////////////////////////////////////////////////////////////////////
    void master::worker_run (std::string task,
                             argvec_t    args,
                             id_t        id)
    {
      // FIXME: checks
      if ( ! initialized_ )
      {
        std::cerr << "master: worker_run : not yet initialized " << std::endl;
        return;
      }

      std::cerr << "master: worker_run " << std::endl;

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
              std::cerr << " master: cannot assign task " << task 
                        << " to " << it->first << " : \n" << e.what () << std::endl;
            }
          }
        }
      }

      if ( 0 == ok )
      {
        std::cerr << "master: could not find idle worker to assign task " << task << std::endl;
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
    std::vector <std::string> master::worker_get_results (id_t id)
    {
      // FIXME: checks
      return ads_[id].get_par_out ();
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_wait (id_t id)
    {
      ads_[id].wait ();
    }


    ////////////////////////////////////////////////////////////////////
    void master::worker_dump (id_t id)
    {
      ads_[id].dump ();
    }

  } // namespace master_worker

} // namespace saga_pm


