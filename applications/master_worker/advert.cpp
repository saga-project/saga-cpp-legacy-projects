
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {
    ////////////////////////////////////////////////////////////////////
    advert::advert (void)
      : ok_     (false)
      , s_      (Unknown)
      , url_    ("")
    {
      // we touch the advert object here. to ensure its early construction.
      // That way, the object should stay alive to be used in the
      // destructor.
      ad_.get_id ();
    }


    ////////////////////////////////////////////////////////////////////
    advert::advert (saga::url url)
      : ok_     (false)
      , create_ (true) // client creates the advert
      , s_      (Unknown)
      , url_    (url_)
    {
      // we touch the advert object here. to ensure its early construction.
      // That way, the object should stay alive to be used in the
      // destructor.
      ad_.get_id ();
    }


    ////////////////////////////////////////////////////////////////////
    advert::advert (saga::job::service js, 
                    saga::job::job     job, 
                    saga::url          url)
      : ok_     (false)
      , create_ (false) // master does not create the adverts
      , s_      (Unknown)
      , js_     (js_)
      , job_    (job_)
      , url_    (url_)
    {
      // we touch the advert object here. to ensure its early construction.
      // That way, the object should stay alive to be used in the
      // destructor.
      ad_.get_id ();
    }


    ////////////////////////////////////////////////////////////////////
    advert::~advert (void)
    {
    }


    ////////////////////////////////////////////////////////////////////
    bool advert::init (void)
    {
      if ( ! ok_ ) 
      {
        try 
        {
          if ( create_ )
          {
            // if the ad exists, it is simply being reused, and re-initialized.
            // Not that this can cause race conditions if different runs have
            // overlapping name spaces!
            //
            // FIXME: make cleaning optional, so that old ads can be preserved if needed,

            // create the advert, initialize all attributes, set id and 
            // state (Started)
            ad_ = saga::advert::entry (url_, saga::advert::Read     | 
                                             saga::advert::Create   );

            std::string               empty ("");
            std::vector <std::string> empty_vec;

            empty_vec.push_back (empty);

            id_ = get_worker_id ();

            ad_.set_attribute        ("id",       id_);
            ad_.set_attribute        ("task",     empty);
            ad_.set_attribute        ("error",    empty);
            ad_.set_attribute        ("task",     empty);
            ad_.set_vector_attribute ("par_in",   empty_vec);
            ad_.set_vector_attribute ("par_out",  empty_vec);

            // advert is initalized, we can set the state to notify master of
            // successfull startup
            ad_.set_attribute ("state", "Started");
          }
          else
          {
            // simply open advert in rw mode, and cache id
            ad_ = saga::advert::entry (url_, saga::advert::ReadWrite);
            id_ = ad_.get_attribute ("id");

            // FIXME: verify/reset state
            std::cout << "open ad ok " << id_ << std::endl;
          }

          ok_ = true; // no exception means success :-)

          return true; // signal that this run actually *did* something
        }
        catch ( const saga::does_not_exist & e )
        {
          // do nothing, not yet ok, retry later
        }
        // other exceptions fall through
      }

      return false; // this run did nothing (useful)
    }


    ////////////////////////////////////////////////////////////////////
    void advert::run (std::string command, 
                 argvec_t    args)
    {
      argvec_t ret;

      std::cout << " running test on 0: " << get_state () << std::endl;

      if ( (! ok_) || (Idle != get_state ()) )
      {
        throw saga::no_success ("Cannot run command - worker not idle");
      }

      set_task    (command);
      set_par_in  (args);
      set_state   (Assigned);
    }


    ////////////////////////////////////////////////////////////////////
    argvec_t advert::wait (void)
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      state s = get_state ();

      if ( Done == s )
        return get_par_out ();

      if ( Failed == s )
        throw saga::no_success ("command failed");

      if ( Busy != s )
        throw saga::no_success ("worker in incorrect state");

      while ( Busy == get_state () )
      {
        :: sleep (1);
        std::cout << "waiting for worker " << job_.get_job_id () << std::endl;
      }

      if ( Failed == s )
        throw saga::no_success ("command failed");

      if ( Done != s )
        throw saga::no_success ("worker in incorrect state");

      return get_par_out ();
    }


    ////////////////////////////////////////////////////////////////////
    void advert::purge (void)
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      ad_.remove ();
    }
    


    ////////////////////////////////////////////////////////////////////
    saga::job::job advert::get_job (void) 
    { 
      return job_;
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_state (state s) 
    { 
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      if ( ! ad_.attribute_exists ("state") )
      {
        throw saga::no_success ("no state found");
      }

      std::string old = ad_.get_attribute ("state");
      ad_.set_attribute ("state", state_to_string (s));

      std::cout << " --- set_state: " << old << " -> " << s << " -> " << state_to_string (s) << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    state advert::get_state (void) 
    { 
      if ( ! ok_ )
      {
        return Unknown;
        // throw saga::no_success ("worker in incorrect state");
      }

      if ( ! ad_.attribute_exists ("state") )
      {
        throw saga::no_success ("no state found");
      }

      std::string s = ad_.get_attribute ("state");

      std::cout << " --- get_state: " << s << std::endl;
      return string_to_state (s);
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_error (std::string e) 
    { 
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      ad_.set_attribute ("error", e);
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_error (void) 
    { 
      if ( ! ok_ )
      {
        return ("worker not yet alive");
      }

      if ( ! ad_.attribute_exists ("error") )
      {
        return ("no error");
      }

      std::string error = ad_.get_attribute ("error");

      std::cout << " --- get_error: " << error << std::endl;
      return error;

    }


    ////////////////////////////////////////////////////////////////////
    std::vector <std::string> advert::get_par_in (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      if ( ! ad_.attribute_exists ("par_in") )
      {
        throw saga::no_success ("worker does not have in parameters, yet");

        // std::vector <std::string> par_in;
        // return par_in;
      }

      return (ad_.get_vector_attribute ("par_in"));
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_par_in (std::vector <std::string> pi)
    {
      ad_.set_vector_attribute ("par_in", pi);
    }


    ////////////////////////////////////////////////////////////////////
    std::vector <std::string> advert::get_par_out (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      if ( ! ad_.attribute_exists ("par_out") )
      {
        throw saga::no_success ("worker does not have out parameters, yet");

        // std::vector <std::string> par_out;
        // return par_out;
      }

      return (ad_.get_vector_attribute ("par_out"));
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_par_out (std::vector <std::string> po)
    {
      ad_.set_vector_attribute ("par_out", po);
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_task (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("worker in incorrect state");
      }

      if ( ! ad_.attribute_exists ("task") )
      {
        throw saga::no_success ("worker does not have a task, yet");
      }

      std::string t = ad_.get_attribute ("task");
      std::cout << " worker " << id_ << " : task found: " << t << std::endl;

      return t;
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_task (std::string t)
    {
      ad_.set_attribute ("task", t);
      std::cout << " worker " << id_ << " : task set to " << t << std::endl;
    }


  } // namespace master_worker

} // namespace saga_pm

