
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
      , id_     (0)
    {
    }


    ////////////////////////////////////////////////////////////////////
    advert::advert (saga::url url)
      : ok_     (false)
      , s_      (Unknown)
      , url_    (url)
    {
      // create or reconnect to advert
      try 
      {
        // try to open.  if the ad exists, it is simply being reused, and
        // re-initialized.  Note that this can cause race conditions if
        // different runs have overlapping name spaces!
        ad_     = saga::advert::entry (url_, saga::advert::ReadWrite);
        id_     = ::atoi (ad_.get_attribute ("id").c_str ());
        ok_     = true;

        LOG << "open ad ok " << id_ << std::endl;
      } 
      catch ( const saga::exception & e )
      {
          LOG << " worker: cannot open advert " 
              << url_ << " : \n" << e.what () << std::endl;
          exit (-1);
      }
    }


    ////////////////////////////////////////////////////////////////////
    advert::advert (saga::job::service js, 
                    saga::job::job     job, 
                    saga::url          url,
                    id_t               id)
      : ok_     (false)
      , s_      (Unknown)
      , js_     (js)
      , job_    (job)
      , url_    (url)
      , id_     (id)
    {
      try
      {
        // create the advert, initialize all attributes, set id and 
        // state (Started)
        LOG << "creating ad at " << url_;

        ad_ = saga::advert::entry (url_, saga::advert::ReadWrite | 
                                   saga::advert::Create    );

        std::string               empty ("");
        std::vector <std::string> empty_vec;

        empty_vec.push_back (empty);

        id_ = get_worker_id ();

        ad_.set_attribute        ("id",       itoa (id_));
        ad_.set_attribute        ("task",     empty);
        ad_.set_attribute        ("error",    empty);
        ad_.set_attribute        ("task",     empty);
        ad_.set_vector_attribute ("par_in",   empty_vec);
        ad_.set_vector_attribute ("par_out",  empty_vec);

        // advert is initalized, we can set the state to notify master of
        // successfull startup
        ad_.set_attribute        ("state",    "Started");

        std::cout << "create ad ok " << id_ << std::endl;

        ok_ = true; // no exception means success :-)
      }
      catch ( const saga::exception & e )
      {
          LOG << " worker: cannot create advert " 
              << url_ << " : \n" << e.what () << std::endl;
          exit (-1);
      }
    }


    ////////////////////////////////////////////////////////////////////
    advert::~advert (void)
    {
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

      while ( Busy == s )
      {
        :: sleep (1);
        std::cout << "waiting for worker " << job_.get_job_id () << std::endl;
        s = get_state ();
      }


      if ( Failed == s )
      {
        ok_ = false;
        throw saga::no_success ("command failed");
      }

      if ( Done != s )
      {
        ok_ = false;
        throw saga::no_success ("worker in incorrect state");
      }

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
    void advert::dump (void)
    {
      if ( ! ok_ )
      {
        std::cout << "advert: dump: not yet initialized" << std::endl;
        return;
      }

      try
      {
        std::vector <std::string> attribs = ad_.list_attributes ();

        std::cout << " -----------------------------------------------------" << std::endl;
        std::cout << "  dumping worker " << get_id () << " @ " << url_ << std::endl;
        std::cout << std::endl;

        LOG       << " -----------------------------------------------------" << std::endl;
        LOG       << "  dumping worker " << get_id () << " @ " << url_ << std::endl;
        LOG       << std::endl;

        for ( unsigned int i = 0; i < attribs.size (); i++ )
        {
          if ( ad_.attribute_is_vector (attribs[i]) )
          {
            std::cout << "  " << attribs[i] << " \t:";
            LOG       << "  " << attribs[i] << " \t:";
            
            std::vector <std::string> vals =  ad_.get_vector_attribute (attribs[i]);

            for ( unsigned int j = 0; j < vals.size (); j++ )
            {
              if ( j > 0 )
              {
                std::cout << ",";
                LOG       << ",";
              }

              std::cout << " " << vals[i];
              LOG       << " " << vals[i];
            }
          }
          else
          {
            std::cout << "  " << attribs[i] << " \t: " << ad_.get_attribute (attribs[i]) << std::endl;
            LOG       << "  " << attribs[i] << " \t: " << ad_.get_attribute (attribs[i]) << std::endl;
          }
        }
      }
      catch ( const saga::exception & e )
      {
        std::cout << "advert: dump error: \n" << e.what () << std::endl;
        LOG       << "advert: dump error: \n" << e.what () << std::endl;
      }
    }
    


    ////////////////////////////////////////////////////////////////////
    saga::job::job advert::get_job (void) 
    { 
      return job_;
    }


    ////////////////////////////////////////////////////////////////////
    id_t advert::get_id (void) 
    { 
      if ( ! ad_.attribute_exists ("id") )
      {
        throw saga::no_success ("no id found");
      }

      id_t id = ::atoi (ad_.get_attribute ("id").c_str ());

      std::cout << " --- get_id: " << id << std::endl;

      return id;
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_state (state s) 
    { 
      if ( ! ad_.attribute_exists ("state") )
      {
        throw saga::no_success ("no state attribute found");
      }

      std::string old = ad_.get_attribute ("state");
      ad_.set_attribute ("state", state_to_string (s));

      std::cout << " --- set_state: " << old << " -> " << s << " -> " << state_to_string (s) << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    state advert::get_state (void) 
    { 
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
      ad_.set_attribute ("error", e);
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_error (void) 
    { 
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

