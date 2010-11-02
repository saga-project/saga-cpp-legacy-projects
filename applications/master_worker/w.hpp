
#ifndef SAGA_PM_MASTER_WORKER_H_HPP
#define SAGA_PM_MASTER_WORKER_H_HPP

#include "master_worker.hpp"
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    // private representation of worker job.  This class simplifies state
    // management on master side of things.
    class w
    {
      public:
        bool                ok;
        state               s;
        saga::job::service  js;
        saga::job::job      job;
        saga::url           url;
        advert              ad;

        w (saga::job::service js_, 
           saga::job::job     job_, 
           saga::url          url_)
          : ok  (false)
            , s   (Unknown)
            , js  (js_)
            , job (job_)
            , url (url_)
      {
      }

        ~w (void)
        {
        }


        bool init (void)
        {
          if ( ! ok ) 
          {
            try 
            {
              ad = advert (url, false);
            }
            catch ( const saga::does_not_exist & e )
            {
              return false;
            }
            catch ( const saga::exception & e )
            {
              return false;
            }
          }

          ok = true;
          return ok;
        }

        void run (std::string command, 
                  argvec_t    args)
        {
          argvec_t ret;

          std::cout << " running test on 0: " << get_state () << std::endl;

          if ( Idle != get_state () )
            throw saga::no_success ("Cannot run command - worker not idle");


          ad.set_task    (command);
          ad.set_par_in  (args);
          ad.set_state   (Assigned);
        }


        argvec_t wait (void)
        {
          state s = get_state ();

          if ( Done == s )
            return ad.get_par_out ();

          if ( Failed == s )
            throw saga::no_success ("command failed");

          if ( Busy != s )
            throw saga::no_success ("worker in incorrect state");

          while ( Busy == get_state () )
          {
            :: sleep (1);
            std::cout << "waiting for worker " << job.get_job_id () << std::endl;
          }

          if ( Failed == s )
            throw saga::no_success ("command failed");

          if ( Done != s )
            throw saga::no_success ("worker in incorrect state");

          return ad.get_par_out ();
        }

        void set_state (state s) { 
          if ( ok ) 
            ad.set_state (s); 
        }

        state get_state (void) { 
          if ( ok ) 
            return ad.get_state (); 
          return Unknown;
        }

        void set_error (std::string e) { 
          if ( ok ) 
            ad.set_error (e); 
        }

        std::string get_error (void) { 
          if ( ok ) 
            return ad.get_error (); 
          return "worker not alive";
        }
    };

  } // namespace master_worker

} // namespace saga_pm

#endif // SAGA_PM_MASTER_WORKER_H_HPP

