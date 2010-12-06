
#include <sstream>
#include <iostream>
#include <fstream>

#include <saga/saga/adaptors/utils.hpp>

#include "job_starter.hpp"

job_starter::endpoint_::endpoint_ (std::string           name,
                                   mb_util::ini::section ini)
  : name_ (name),
    ini_  (ini )
{
  ctype_  =         ini_.get_entry ("ctype", "" );
  user_   =         ini_.get_entry ("user" , "" );
  pass_   =         ini_.get_entry ("pass" , "" );
  cert_   =         ini_.get_entry ("cert" , "" );
  key_    =         ini_.get_entry ("key"  , "" );
  proxy_  =         ini_.get_entry ("proxy", "" );
  cadir_  =         ini_.get_entry ("cadir", "" );
  exe_    =         ini_.get_entry ("exe"  , "" );
  args_   =         ini_.get_entry ("args" , "" );
  pwd_    =         ini_.get_entry ("pwd"  , "" );
  njobs_  = ::atoi (ini_.get_entry ("njobs", "1").c_str ());

  saga::session s;

  saga::context c (ctype_);

  c.set_attribute (saga::attributes::context_certrepository, cadir_);
  c.set_attribute (saga::attributes::context_usercert      , cert_);
  c.set_attribute (saga::attributes::context_userkey       , key_);
  c.set_attribute (saga::attributes::context_userid        , user_);
  c.set_attribute (saga::attributes::context_userpass      , pass_);

  if ( ! proxy_.empty () )
  {
    c.set_attribute (saga::attributes::context_userproxy   , proxy_);
  }
  // c.set_attribute (saga::attributes::context_uservo        , "");
  // c.set_attribute (saga::attributes::context_lifetime      , "");
  // c.set_attribute (saga::attributes::context_remoteid      , "");
  // c.set_attribute (saga::attributes::context_remotehost    , "");
  // c.set_attribute (saga::attributes::context_remoteport    , "");

  s.add_context (c);

  saga::job::service js (s, url_);

  service_ = js;

  // dump ini section for this endpoint
  std::ofstream fout ((std::string ("endpoint.") + name + ".ini").c_str ());
  ini_.dump (0, fout);
  fout.close ();
}


//////////////////////////////////////////////////////////////////////
job_starter::job_starter (std::string       a_dir, 
                          mb_util::ini::ini ini)
  : ini_ (ini)
{
  // first, initialize all endpoints according to the ini file
  mb_util::ini::section   cfg      = ini_.get_section ("mandelbrot");
  mb_util::ini::section   ep_cfg   = cfg.get_section  ("backends");
  mb_util::ini::entry_map backends = ep_cfg.get_entries ();
  mb_util::ini::entry_map :: iterator it;

  for ( it = backends.begin (); it != backends.end (); it++ )
  {
    std::string key = (*it).first;
    std::string val = (*it).second;

    if ( val == "yes" )
    {
      mb_util::ini::section backend_config = ep_cfg.get_section (key);

      try 
      {
        std::string url = backend_config.get_entry ("url"  , "");
        std::cout << "creating  endpoint '" << key << "' \t ..." << std::flush;

        endpoints_.push_back (endpoint_ (key, backend_config));
        std::cout << " ok  (" << url << ")" << std::endl;
      }
      catch ( const saga::exception & e )
      {
        std::cerr << " failed "
               // << ": " << e.what () 
                  << std::endl;
      }
    }
  }


  // for each endpoint, we run 'njobs' jobs.  
  for ( unsigned int e = 0; e < endpoints_.size (); e++ )
  {
    // try the next endpoint
    endpoint_ ep = endpoints_[e];

    for ( unsigned int j = 0; j < ep.njobs_; j++ )
    {
      try
      {
        // create a job description
        saga::job::description jd;
        jd.set_attribute (saga::job::attributes::description_executable, ep.exe_);

        // client parameters:
        // 0: path to advert directory to be used (job bucket)
        // 1: jobnum, == name of work bucket for that job (is that in loop later)
        std::vector <std::string> args;
        args.push_back ("mandelbrot_client ");
        args.push_back (a_dir);

        std::stringstream ident;
        ident << jobs_.size ();
        args.push_back (ident.str ());

        // append ep args
        std::vector <std::string> epargs = saga::adaptors::utils::split (ep.args_, ' ');
        for ( unsigned int a = 0; a < epargs.size (); a++ )
        {
          args.push_back (epargs[a]);
        }


        jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

        if ( ! ep.pwd_.empty () )
        {
          jd.set_attribute (saga::job::attributes::description_working_directory, ep.pwd_);
        }

        // let the clients store stdout/stderr to /tmp/mandelbrot_client.[id].out/err
        // FIXME: this should get enabled once the bes adaptor supports it, and
        // is able to stage the output files back into the pwd
        # if 0
        {
          int id = jobs_.size ();

          std::string out;
          std::string err;

          out += ("/tmp/mandelbrot_client.");
          out += boost::lexical_cast <std::string> (id);
          out += ".out";

          err += ("/tmp/mandelbrot_client.");
          err += boost::lexical_cast <std::string> (id);
          err += ".err";

          jd.set_attribute (saga::job::attributes::description_output, out);
          jd.set_attribute (saga::job::attributes::description_error,  err);
        }
        # endif

        std::cout << "starting  job "
                  << jobs_.size ()
                  << " on "
                  << ep.name_ 
                  << " \t ... " << std::flush;

        saga::job::job j = ep.service_.create_job (jd);

        j.run ();

        if ( saga::job::Running != j.get_state () )
        {
          std::cout << "failure - could not run " 
                    << ep.exe_ << " " << ep.args_ 
                    << std::endl;
          j.cancel (); // clean up resources
          // do not use this job
        }
        else
        {
          std::string jobid (j.get_job_id ());

          // trim jobid for readability
          if ( jobid.size () > 54 )
          {
            jobid.erase (55);

            jobid[52] = '.';
            jobid[53] = '.';
            jobid[54] = '.';
          }

          // keep job
          jobs_.push_back (j);

          std::cout << "ok  " 
                    << jobid 
                    << " " << ep.args_
                    << std::endl;
        }
      }
      catch ( const saga::exception & e )
      {
        std::cout << "failure - could not start exe " << ep.exe_ << " " << ep.args_ 
              //  << ": "                    << e.what ()
                  << std::endl;
      }
    }
  }

  if ( jobs_.size () == 0 )
  {
    throw "Could not start any jobs!";
  }
}


job_starter::~job_starter (void)
{
}

