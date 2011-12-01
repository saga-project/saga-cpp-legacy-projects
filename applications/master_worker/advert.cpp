
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
                                   saga::advert::CreateParents );

        LOG << "created ad at " << url_;
        std::string               empty ("");
        std::vector <std::string> empty_vec;

        empty_vec.push_back (empty);

        id_ = get_worker_id ();

        ad_.set_attribute        ("id",       itoa (id_));
        ad_.set_attribute        ("task",     empty);
        ad_.set_attribute        ("error",    empty);
        ad_.set_attribute        ("task",     empty);
        ad_.set_attribute        ("state",    itoa (Unknown));
        ad_.set_vector_attribute ("par_in",   empty_vec);
        ad_.set_vector_attribute ("par_out",  empty_vec);

        ok_ = true; // no exception means success :-)
      }
      catch ( const saga::exception & e )
      {
          LOG << " advert: cannot create advert " 
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
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: run: not initialized");
      }

      if ( Idle != get_state () )
      {
        throw saga::no_success ("Cannot run command - worker not idle");
      }

      set_task    (command);
      set_par_in  (args);
      set_state   (Assigned);
    }


    ////////////////////////////////////////////////////////////////////
    void advert::wait (void)
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: wait: not initialized");
      }

      state s = get_state ();

      while ( Done   != s &&
              Failed != s &&
              Quit   != s )
      {
        LOG << "waiting for worker " << get_id ();

        :: sleep (1);
        s = get_state ();
      }


      // if ( Failed == s )
      // {
      //   throw saga::no_success ("command failed");
      // }

      // if ( Done != s )
      // {
      //   throw saga::no_success ("worker in incorrect state");
      // }
    }


    ////////////////////////////////////////////////////////////////////
    void advert::purge (void)
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: purge: not initialized");
      }

      ad_.remove ();
    }
    

    ////////////////////////////////////////////////////////////////////
    void advert::dump (void)
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: dump: not initialized");
      }

      try
      {
        std::vector <std::string> attribs = ad_.list_attributes ();

        LOG << " -----------------------------------------------------";
        LOG << "  dumping worker " << get_id () << " @ " << url_;
        LOG << "";

        for ( unsigned int i = 0; i < attribs.size (); i++ )
        {
          if ( ad_.attribute_is_vector (attribs[i]) )
          {
            std::string val;
            val += "  " + attribs[i] + " \t:";
            
            std::vector <std::string> vals =  ad_.get_vector_attribute (attribs[i]);

            for ( unsigned int j = 0; j < vals.size (); j++ )
            {
              if ( j > 0 )
              {
                val += ",";
              }

              val += " " + vals[j];
            }

            LOG << val;
          }
          else
          {
            LOG << "  " << attribs[i] << " \t: " << ad_.get_attribute (attribs[i]) << std::endl;
          }
        }
      }
      catch ( const saga::exception & e )
      {
        throw saga::no_success (std::string ("advert: dump error: ") + e.what ());
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
    }


    ////////////////////////////////////////////////////////////////////
    state advert::get_state (void) 
    { 
      if ( ! ad_.attribute_exists ("state") )
      {
        throw saga::no_success ("no state found");
      }

      std::string s = ad_.get_attribute ("state");

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

      return error;

    }


    ////////////////////////////////////////////////////////////////////
    argvec_t advert::get_par_in (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: get_par_in: not initialized");
      }

      if ( ! ad_.attribute_exists ("par_in") )
      {
        throw saga::no_success ("worker does not have in parameters, yet");

        // argvec_t par_in;
        // return par_in;
      }

      return (ad_.get_vector_attribute ("par_in"));
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_par_in (argvec_t pi)
    {
      ad_.set_vector_attribute ("par_in", pi);
    }


    ////////////////////////////////////////////////////////////////////
    argvec_t advert::get_par_out (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: get_par_out: not initialized");
      }

      if ( ! ad_.attribute_exists ("par_out") )
      {
        throw saga::no_success ("worker does not have out parameters, yet");

        // argvec_t par_out;
        // return par_out;
      }

      return (ad_.get_vector_attribute ("par_out"));
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_par_out (argvec_t po)
    {
      ad_.set_vector_attribute ("par_out", po);
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_task (void )
    {
      if ( ! ok_ )
      {
        throw saga::no_success ("advert: get_task: not initialized");
      }

      if ( ! ad_.attribute_exists ("task") )
      {
        throw saga::no_success ("worker does not have a task, yet");
      }

      std::string t = ad_.get_attribute ("task");

      return t;
    }


    ////////////////////////////////////////////////////////////////////
    void advert::set_task (std::string t)
    {
      ad_.set_attribute ("task", t);
    }


  } // namespace master_worker

} // namespace saga_pm

