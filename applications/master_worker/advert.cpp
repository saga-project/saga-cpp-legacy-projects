
#include "advert.hpp"

namespace saga_pm
{
  namespace master_worker
  {

    ////////////////////////////////////////////////////////////////////
    advert::advert (void)
      : url_ ("")
    {
    }


    ////////////////////////////////////////////////////////////////////
    advert::advert  (saga::url u, 
                     bool      create)
      : url_ (u)
    {
      std::cout << "advert: new URL" << url_ << std::endl;

      if ( create )
      {
        if ( is_master () )
        {
          std::cout << "master MUST NOT create ads!" << std::endl;
          throw saga::no_success ("Master must not create worker adverts.");
        }

        // if the ad exists, we remove it before (ads don't have Truncate :-/ )
        //
        // FIXME: make this optional, so that old ads can be preserved if needed,
        // different instances don't clash, and/or performance can be optimized
        try {
          saga::advert::entry (url_, saga::advert::Write).remove ();
        } catch ( const saga::exception & e ) { /* ignore errors */ }


        // create the advert, initialize all attributes, set id and 
        // state (Started)
        ad_ = saga::advert::entry (url_, saga::advert::Read          | 
                                   saga::advert::Create        | 
                                   saga::advert::CreateParents | 
                                   saga::advert::Exclusive     );

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
        std::cout << "open ad" << std::endl;
        // simply open advert in rw mode, and cache id
        ad_ = saga::advert::entry (url_, saga::advert::ReadWrite);
        id_ = ad_.get_attribute ("id");

        // FIXME: verify/reset state
        std::cout << "open ad ok " << id_ << std::endl;
      }
    }

    ////////////////////////////////////////////////////////////////////
    advert::advert (const advert & orig)
    {
      id_   = orig.id_;
      url_  = orig.url_;
      ad_   = orig.ad_;
    }

    ////////////////////////////////////////////////////////////////////
    advert::~advert (void)
    {
      std::cout << "closing advert " << url_ << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_id (void)
    {
      return id_;
    }


    ////////////////////////////////////////////////////////////////////
    state advert::get_state (void )
    {
      if ( ! ad_.attribute_exists ("state") )
      {
        std::cout << " --- get_state: problem " << std::endl;
        throw saga::no_success ("no state found");
      }

      std::string s = ad_.get_attribute ("state");

      std::cout << " --- get_state: " << s << std::endl;
      return string_to_state (s);
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_state (state s)
    {
      std::cout << " --- set_state: " << std::endl;
      std::string old   = "Unknown";
      std::string check = "Unknown";

      if ( ! ad_.attribute_exists ("state") )
      {
        std::cout << " --- set_state: problem " << std::endl;
        exit (0);
        // throw saga::no_success ("no state found");
      }

      old   = ad_.get_attribute ("state");
      ad_.set_attribute ("state", state_to_string (s));
      check = ad_.get_attribute ("state");

      std::cout << " --- set_state: " << old << " -> " << s << " -> " << check << std::endl;
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_task (void )
    {
      return (ad_.get_attribute ("task"));
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_task (std::string t)
    {

      try 
      {
        std::cout << " advert " << ad_.get_url () << " task setting to " << t << std::endl;
        ad_.set_attribute ("task", t);

        std::string check = ad_.get_attribute ("task");
        std::cout << " advert " << ad_.get_url () << " task set     to " << check << std::endl;

      }
      catch ( const saga::exception & e )
      {
        std::cout << " ad set exception: " << e.what () << std::endl;
      }
    }


    ////////////////////////////////////////////////////////////////////
    std::vector <std::string> advert::get_par_in (void )
    {
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
      return (ad_.get_vector_attribute ("par_out"));
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_par_out (std::vector <std::string> po)
    {
      ad_.set_vector_attribute ("par_out", po);
    }


    ////////////////////////////////////////////////////////////////////
    std::string advert::get_error (void )
    {
      return (ad_.get_attribute ("error"));
    }

    ////////////////////////////////////////////////////////////////////
    void advert::set_error (std::string e)
    {
      ad_.set_attribute ("error", e);
    }
    ////////////////////////////////////////////////////////////////////

  } // namespace master_worker

} // namespace saga_pm

