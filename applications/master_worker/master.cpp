
#include <saga/saga/adaptors/utils.hpp>

#include "master.hpp"

#define TIMEOUT 5 // seconds to wait for worker to appear


namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    master::master (unsigned int nworker, 
                    std::string  worker_exe)
      : nworker_    (nworker)
        , worker_exe_ (worker_exe)
    {
      // master: spawn off worker
      std::cout << " master: starting " << nworker_ << " worker" << std::endl;
      js_ = saga::job::service ("fork://localhost/"); // fixme

      saga::job::service     local ("fork://localhost");
      saga::job::self        self = local.get_self ();
      saga::job::description jd   = self.get_description (); 

      jd.set_attribute ("Executable", worker_exe_);

      std::vector <std::string> env;

      if ( jd.attribute_exists (saga::job::attributes::description_environment) )
      {
        // we copy SAGA_LOCATION and LD_LIBRARY_PATH/DYLD_LIBRARY_PATH from the
        // old env
        std::vector <std::string> old_env = jd.get_vector_attribute (saga::job::attributes::description_environment);

        for ( unsigned int i = 0; i < old_env.size (); i++ )
        {
          std::vector <std::string> elems = saga::adaptors::utils::split (old_env[i], '=', 2);

          if ( elems.size () >=1 )
          {
            if ( elems[0] == "SAGA_LOCATION"     ||
                 elems[0] == "LD_LIBRARY_PATH"   ||
                 elems[0] == "DYLD_LIBRARY_PATH" )
            {
              env.push_back (old_env[i]);
            }
          }
        }
      }

      std::string env_verbose   ("SAGA_VERBOSE=4");
      std::string env_logdest   ("SAGA_LOGDESTINATION=cout");
      std::string env_master    ("SAGA_MASTER_ID=0");
      std::string env_worker    ("SAGA_WORKER_ID=");
      std::string env_advert    ("SAGA_WORKER_AD=");
      std::string worker_advert ("advert://localhost/home/merzky/master_worker/"); 

      //worker_advert += self.get_job_id () + "/";  // FIXME: [] need shell escapes
      worker_advert += "0/";

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
        saga::job::job job = js_.create_job (jd);
        job.run ();

        if ( job.get_state () != saga::job::Running )
        {
          std::cerr << "could not run job " << i << std::endl;
        }

        std::cout << " starting worker " << i << " : " << job.get_job_id () << std::endl;
        std::cout << "                 " << worker_advert + itoa (i)        << std::endl;

        // keep failed jobs, too, for cleanup
        worker_.push_back (w (js_, job, worker_advert + itoa (i)));
      }

      // wait for the worker ads to appear.  If they don't, fail after some
      // timeout due to missing worker
      time_t now = ::time (NULL);
      if ( (time_t)-1 == now )
      {
        throw saga::no_success ("Could not get system time");
      }
      time_t       end       = now + TIMEOUT;
      unsigned int worker_ok = 0;

      while ( now       < end      &&
              worker_ok < nworker_ )
      {
        std::cout << "checking at " << now << " (" << end << ") : " << worker_ok << " - " << nworker_ << std::endl;

        for ( unsigned int i = 0; i < worker_.size (); i++ )
        {
          if ( ! worker_[i].ok )
          {
            std::cout << "         init  " << i << std::endl;
            worker_[i].init ();
          }

          if ( worker_[i].ok )
          {
            std::cout << "         check " << i << std::endl;
            if ( worker_[i].get_state () == Started )
            {
              std::cout << "         ack   " << i << std::endl;
              worker_[i].set_state (Idle);
              worker_ok++;
            }
          }
        }


        if ( worker_ok < nworker_ )
        {
          ::sleep (1);

          now = ::time (NULL);
          if ( (time_t)-1 == now )
          {
            throw saga::no_success ("Could not get system time");
          }
        }
        std::cout << "check done  " << now << " (" << end << ") : " << worker_ok << " - " << nworker_ << std::endl;
      }

      if ( worker_ok < nworker_ )
      {
        std::cout << "workers went AWOL" << std::endl;
        // throw saga::no_success ("workers went AWOL");
      }

      std::cout << "check ok    " << now << " (" << end << ") : " << worker_ok << " - " << nworker_ << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    master::~master (void)
    {
      // we can't call shutdown, as the adverts are getting closed first, and
      // shutdown tries to check advert for state consistency *sigh*
      // shutdown ();
    }


    ////////////////////////////////////////////////////////////////////
    void master::shutdown (void)
    {
      // kill all worker
      // FIXME: make this configurable, so that a master can pick up existing
      // or old workers, similar to session resuming

      // first send QUIT command
      for ( unsigned int i = 0; i < worker_.size (); i++ )
      {
        std::cout << " quitting worker " << i << " : " << worker_[i].job.get_job_id () 
          << " (" << state_to_string (worker_[i].get_state ()) << ")" << std::endl;
        worker_[i].ad.set_task ("quit");
      }

      // let that sink in ;-)
      ::sleep (TIMEOUT);

      // if worker did not quit, kill it
      for ( unsigned int i = 0; i < worker_.size (); i++ )
      {
        if ( worker_[i].get_state () != Quit )
        {
          std::cout << " stopping worker " << i << " : " << worker_[i].job.get_job_id () << std::endl;
          worker_[i].job.cancel ();
          worker_[i].set_state  (Failed);
          worker_[i].set_error  ("Worker ignored QUIT command");
        }
      }

      // TODO: clean out the adverts
    }

    ////////////////////////////////////////////////////////////////////
    int master::worker_run (std::string task, 
                            argvec_t    args)
    {
      // FIXME: find idle worker, assign task to first one.  if none found, put
      // task onto queue
      std::cout << " running test on 0" << std::endl;
      // worker_[0].run ("test", args);
      std::cout << " running test on ok" << std::endl;

      // FIXME: check for success
      return 0; // return worker index
    }

    ////////////////////////////////////////////////////////////////////
    argvec_t master::worker_wait (int tid)
    {
      return worker_[tid].wait ();
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm


