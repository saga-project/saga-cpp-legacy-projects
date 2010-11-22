
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


//////////////////////////////////////////////////////////////////////
job_starter::job_starter (int          njobs, 
                          std::string  a_dir)
{
  // first, initialize all endpoints according to the ini file

  std::string ini_file ("mandelbrot.ini");
  char * ini_env = ::getenv ("SAGA_MANDELBROT_INI");

  if ( NULL != ini_env )
  {
    ini_file = ini_env;
  }

  std::cout << "using configuration in " << ini_file << std::endl;

  saga::ini::ini ini (ini_file);


  saga::ini::section mandelbrot_config = ini.get_section ("mandelbrot");
  saga::ini::section backends_config   = mandelbrot_config.get_section ("backends");

  saga::ini::entry_map backends = backends_config.get_entries ();
  saga::ini::entry_map :: iterator it;

  for ( it = backends.begin (); it != backends.end (); it++ )
  {
    std::string key = (*it).first;
    std::string val = (*it).second;

    if ( val == "yes" )
    {
      saga::ini::section backend_config = backends_config.get_section (key);

      endpoints_.push_back (endpoint_ (key,       
                                       backend_config.get_entry ("url"  , ""),
                                       backend_config.get_entry ("ctype", ""),
                                       backend_config.get_entry ("user" , ""),
                                       backend_config.get_entry ("pass" , ""),
                                       backend_config.get_entry ("cert" , ""),
                                       backend_config.get_entry ("key"  , ""),
                                       backend_config.get_entry ("proxy", ""),
                                       backend_config.get_entry ("cadir", ""),
                                       backend_config.get_entry ("exe"  , ""),
                                       backend_config.get_entry ("pwd"  , "")));
    }
  }

  if ( mandelbrot_config.has_entry ("job_num") )
  {
     njobs_ = ::atoi (mandelbrot_config.get_entry ("job_num").c_str ());
  }

  if ( njobs <= 0 )
  {
     njobs_ = endpoints_.size ();
  }
  else
  {
    njobs_ = njobs;
  }


  // we use an extra index to iterate over the endpoints, to avoid deadlock if
  // an endpoints fails repeatedly
  // we don't try more than njobs *#endpoints time
  bool          todo = true;
  unsigned int  idx  = 0;

  while ( todo )
  {
    ::sleep (1);
    if ( jobs_.size () >=  njobs_ ||
         idx           >= (njobs_ * endpoints_.size ()) )
    {
      // either we have enough jobs, or we tried often enough
      todo = false;
      continue;
    }

    // try the next endpoint
    endpoint_ ep = endpoints_[idx % endpoints_.size ()];

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
    ident << jobs_.size () + 1;
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
      std::cerr << "Could not start client on " << ep.service_.get_url () << std::endl;
      j.cancel ();
      // do not count this job
      continue;
    }

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

    std::cout << "created job number " 
              << jobs_.size () << "/" << njobs_ 
              << " on " 
              << ep.url_
              << " : " 
              << jobid
              << std::endl;

    // use next endpoint
    idx++;
  }

  if ( jobs_.size () == 0 )
  {
    throw "Could not start any jobs!";
  }

  if ( jobs_.size () < njobs_ )
  {
    std::cout << " could not start all " << njobs_ << " jobs - continue with " << jobs_.size () << std::endl;
  }
}


job_starter::~job_starter (void)
{
}

