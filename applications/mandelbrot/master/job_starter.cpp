
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
                                   std::string  pwd)
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
    pwd_         (pwd  )
{

  std::cout << "adding  endpoint '" << name_ << "' (" << url_ << ")" << std::endl;
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

  std::cout << "created endpoint '" << name_ << "' (" << url_ << ")" << std::endl;

  service_ = js;
}

job_starter::job_starter (unsigned int njobs, 
                          std::string  a_dir)
{
  // first, initialize all endpoints according to the ini file

  std::string ini_file ("mandelbrot.ini");
  char * ini_env = ::getenv ("SAGA_MANDELBROT_INI");

  if ( NULL != ini_env )
  {
    ini_file = ini_env;
  }

  saga::ini::ini ini (ini_file);


  saga::ini::section config = ini.get_section ("mandelbrot").get_section ("backends");

  saga::ini::entry_map backends = config.get_entries ();
  saga::ini::entry_map :: iterator it;

  for ( it = backends.begin (); it != backends.end (); it++ )
  {
    std::string key = (*it).first;
    std::string val = (*it).second;

    if ( val == "yes" )
    {
      saga::ini::section backend_config = config.get_section (key);

      std::cout << "using backend " << key << std::endl;

      endpoints_.push_back (endpoint_ (key,       
                                       backend_config.get_entry ("url"  ),
                                       backend_config.get_entry ("ctype"),
                                       backend_config.get_entry ("user" ),
                                       backend_config.get_entry ("pass" ),
                                       backend_config.get_entry ("cert" ),
                                       backend_config.get_entry ("key"  ),
                                       backend_config.get_entry ("proxy"),
                                       backend_config.get_entry ("cadir"),
                                       backend_config.get_entry ("exe"  ),
                                       backend_config.get_entry ("pwd"  )));
    }
  }


  for ( unsigned int n = 0; n < njobs; n++ )
  {
    endpoint_ ep = endpoints_[n % endpoints_.size ()];

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
    ident << n + 1;
    args.push_back (ident.str ());

    jd.set_vector_attribute (saga::job::attributes::description_arguments, args);

    if ( ! ep.pwd_.empty () )
    {
      jd.set_attribute (saga::job::attributes::description_working_directory, ep.pwd_);
    }

    saga::job::job j = ep.service_.create_job (jd);

    j.run ();

    if ( saga::job::Running != j.get_state () )
    {
      std::cout << "state: " << j.get_state () << std::endl;
      throw "Could not start client\n";
    }

    std::string jobid (j.get_job_id ());

    // trim jobid for readability
    if ( jobid.size () > 54 )
    {
      jobid.erase (55);

      jobid[51] = '.';
      jobid[52] = '.';
      jobid[53] = '.';
    }

    std::cout << "created job number " 
              << n + 1 << "/" << njobs 
              << " on " 
              << ep.url_
              << " : " 
              << jobid
              << std::endl;

    // keep job
    jobs_.push_back (j);
  }
}


job_starter::~job_starter (void)
{
}

