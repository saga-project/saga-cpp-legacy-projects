
#include <saga/saga/adaptors/utils.hpp>

#include "master.hpp"

#define TIMEOUT 5 // seconds to wait for worker to appear


namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    master::master (unsigned int              nworker, 
                    std::string               worker_exe,
                    std::vector <std::string> worker_args)
      : nworker_     (nworker)
      , worker_exe_  (worker_exe)
      , worker_args_ (worker_args)
      , master_ad_   ("advert://localhost/home/merzky/master_worker/")
    {
      // master: spawn off worker
      std::cout << " master: starting " << nworker_ << " worker" << std::endl;

      saga::job::description jd;

      jd.set_attribute        (saga::job::attributes::description_executable, worker_exe_);
      jd.set_vector_attribute (saga::job::attributes::description_arguments, worker_args_);

      std::vector <std::string> env;

      std::string env_verbose   ("SAGA_VERBOSE=1");
      std::string env_logdest   ("SAGA_LOGDESTINATION=cout");
      std::string env_master    ("SAGA_MASTER_ID=0");
      std::string env_worker    ("SAGA_WORKER_ID=");
      std::string env_advert    ("SAGA_WORKER_AD=");


      // create the master advert dir, if it does not exist
      // FIXME: uhm, this should have a unique name
      // FIXME: handle existing dir as error, or pick up workers in there...
      master_ad_ += "0/";
      ad_ = saga::advert::directory (master_ad_, saga::advert::Create | saga::advert::CreateParents);

      LOG << "created advert dir at " << ad_.get_url ();

      // worker ads live in the master advert directory
      std::string worker_advert (master_ad_);

      for ( unsigned int i = 0; i < nworker_; i++ )
      {
        std::vector <std::string> this_env = env;

        this_env.push_back (env_verbose);
        this_env.push_back (env_logdest);
        this_env.push_back (env_master );
        this_env.push_back (env_worker + itoa (i));
        this_env.push_back (env_advert + worker_advert + itoa (i));

        jd.set_attribute        (saga::job::attributes::description_interactive, saga::attributes::common_true);
        jd.set_attribute        (saga::job::attributes::description_output,      "/tmp/t");
        jd.set_vector_attribute (saga::job::attributes::description_environment, this_env);


        // FIXME: jobs should be spawned asynchronously
        saga::job::service js ("fork://localhost/"); // FIXME
        saga::job::job     job = js.create_job (jd);
        job.run ();

        if ( job.get_state () != saga::job::Running )
        {
          std::cerr << "could not run job " << i << std::endl;
        }

        std::cout << " started worker " << i << " : " << job.get_job_id () << std::endl;
        std::cout << "                " << worker_advert + itoa (i)        << std::endl;

        // keep failed jobs, too, for cleanup
        worker_ads_.push_back (advert (js, job, worker_advert + itoa (i)));
      }

      // wait for the worker ads to appear.  If they don't, fail after some
      // timeout due to missing worker
      unsigned int timer     = 0;
      unsigned int worker_ok = 0;

      while ( timer     < TIMEOUT  &&
              worker_ok < nworker_ )
      {
        std::cout << "checking : " << worker_ok << " - " << nworker_ << std::endl;

        for ( unsigned int i = 0; i < worker_ads_.size (); i++ )
        {
          if ( worker_ads_[i].init () )
          {
            // worker_ad just got initialized successfully!
            std::cout << "         check " << i << std::endl;

            if ( worker_ads_[i].get_state () == Started )
            {
              std::cout << "         ack   " << i << std::endl;
              worker_ads_[i].set_state (Idle);
              worker_ok++;
            }
          }
        }


        if ( worker_ok < nworker_ )
        {
          ::sleep (1);

          timer++;
        }
      }

      if ( worker_ok < nworker_ )
      {
        std::cout << "workers went AWOL" << std::endl;
        // FIXME: continue with those workers we actually found
        throw saga::no_success ("workers went AWOL");
      }

      std::cout << "check ok : " << worker_ok << " - " << nworker_ << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    master::~master (void)
    {
      shutdown ();
    }


    ////////////////////////////////////////////////////////////////////
    void master::shutdown (void)
    {
      // kill all worker
      // FIXME: make this configurable, so that a later master can pick up 
      // existing or old workers, similar to session resuming

      // first send QUIT command
      for ( unsigned int i = 0; i < worker_ads_.size (); i++ )
      {
        std::cout << " quitting worker " << i << " : " << worker_ads_[i].get_job ().get_job_id () 
                  << " (" << state_to_string (worker_ads_[i].get_state ()) << ")" << std::endl;
        worker_ads_[i].set_task ("quit");
      }

      // let that sink in ;-)
      ::sleep (TIMEOUT);

      // kill zombie workers, and purge adverts
      for ( unsigned int i = 0; i < worker_ads_.size (); i++ )
      {
        if ( worker_ads_[i].get_state () != Quit )
        {
          std::cout << " stopping worker " << i << " : " << worker_ads_[i].get_job ().get_job_id () << std::endl;
          worker_ads_[i].get_job ().cancel ();
          worker_ads_[i].set_state  (Failed);
          worker_ads_[i].set_error  ("Worker ignored QUIT command");
        }

        // TODO: clean out the adverts
        // worker_ads_[i].purge ();  // this is take care of below!
      }

      // ad_.remove (saga::advert::Recursive);
    }

    ////////////////////////////////////////////////////////////////////
    int master::worker_run (std::string task, 
                            argvec_t    args)
    {
      // FIXME: find idle worker, assign task to first one.  if none found, put
      // task onto queue
      std::cout << " running test on 0" << std::endl;
      worker_ads_[0].run ("test", args);
      std::cout << " running test on ok" << std::endl;

      // FIXME: check for success
      return 0; // return worker index
    }

    ////////////////////////////////////////////////////////////////////
    argvec_t master::worker_wait (int tid)
    {
      return worker_ads_[tid].wait ();
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm


