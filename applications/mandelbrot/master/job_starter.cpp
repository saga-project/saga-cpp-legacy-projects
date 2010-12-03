
#include <sstream>
#include <iostream>

#include <saga/saga/adaptors/utils.hpp>

#include "job_starter.hpp"

job_starter::endpoint_::endpoint_ (std::string  name,
                                   std::string  url,
                                   std::string  ctype,
                                   std::string  user,
                                   std::string  pass,
                                   std::string  cert,
                                   std::string  key,
                                   std::string  proxy,
                                   std::string  cadir,
                                   std::string  exe,
                                   std::string  args,
                                   std::string  pwd,
                                   std::string  njobs)
  : name_        (name ),
    url_         (url  ),
    ctype_       (ctype),
    user_        (user ),
    pass_        (pass ),
    cert_        (cert ),
    key_         (key  ),
    proxy_       (proxy),
    cadir_       (cadir),
    exe_         (exe  ),
    args_        (args ),
    pwd_         (pwd  ),
    njobs_       (::atoi (njobs.c_str ()))
{
  saga::session s;

  saga::context c (ctype_);

  c.set_attribute (saga::attributes::context_certrepository, cadir);
  c.set_attribute (saga::attributes::context_usercert      , cert);
  c.set_attribute (saga::attributes::context_userkey       , key);
  c.set_attribute (saga::attributes::context_userid        , user);
  c.set_attribute (saga::attributes::context_userpass      , pass);

  if ( ! proxy.empty () )
  {
    c.set_attribute (saga::attributes::context_userproxy     , proxy);
  }
  // c.set_attribute (saga::attributes::context_uservo        , "");
  // c.set_attribute (saga::attributes::context_lifetime      , "");
  // c.set_attribute (saga::attributes::context_remoteid      , "");
  // c.set_attribute (saga::attributes::context_remotehost    , "");
  // c.set_attribute (saga::attributes::context_remoteport    , "");

  s.add_context (c);

  saga::job::service js (s, url_);

  service_ = js;
}


//////////////////////////////////////////////////////////////////////
job_starter::job_starter (std::string  a_dir, 
                          std::string  ini_file)
  : ini_file_ (ini_file)
{
  // first, initialize all endpoints according to the ini file
  saga::ini::ini ini (ini_file_);

  saga::ini::section cfg    = ini.get_section ("mandelbrot");
  saga::ini::section ep_cfg = cfg.get_section ("backends");

  saga::ini::entry_map backends = ep_cfg.get_entries ();
  saga::ini::entry_map :: iterator it;

  for ( it = backends.begin (); it != backends.end (); it++ )
  {
    std::string key = (*it).first;
    std::string val = (*it).second;

    if ( val == "yes" )
    {
      saga::ini::section backend_config = ep_cfg.get_section (key);

      try 
      {
        std::string url = backend_config.get_entry ("url"  , "");
        std::cout << "creating endpoint '" << key << "' \t (" << url << ") \t ..." << std::flush;

        endpoints_.push_back (endpoint_ (key, url,
                                         backend_config.get_entry ("ctype", ""),
                                         backend_config.get_entry ("user" , ""),
                                         backend_config.get_entry ("pass" , ""),
                                         backend_config.get_entry ("cert" , ""),
                                         backend_config.get_entry ("key"  , ""),
                                         backend_config.get_entry ("proxy", ""),
                                         backend_config.get_entry ("cadir", ""),
                                         backend_config.get_entry ("exe"  , ""),
                                         backend_config.get_entry ("args" , ""),
                                         backend_config.get_entry ("pwd"  , ""),
                                         backend_config.get_entry ("njobs", "1")));
        std::cout << " ok" << std::endl;
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

        std::cout << "running  job "
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

          std::cout << "ok " 
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

