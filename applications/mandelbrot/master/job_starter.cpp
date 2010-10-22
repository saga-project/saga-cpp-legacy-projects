
#include <sstream>
#include <iostream>

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
  c.set_attribute (saga::attributes::context_userproxy     , proxy);
  c.set_attribute (saga::attributes::context_userid        , user);
  c.set_attribute (saga::attributes::context_userpass      , pass);
  c.set_attribute (saga::attributes::context_uservo        , "");
  c.set_attribute (saga::attributes::context_lifetime      , "");
  c.set_attribute (saga::attributes::context_remoteid      , "");
  c.set_attribute (saga::attributes::context_remotehost    , "");
  c.set_attribute (saga::attributes::context_remoteport    , "");

  s.add_context (c);

  saga::job::service js (s, url_);

  std::cout << "created endpoint '" << name_ << "' (" << url_ << ")" << std::endl;

  service_ = js;
}

job_starter::job_starter (unsigned int njobs, 
                          std::string  a_dir)
{
  // first, initialize all endpoint info.  There is likely a more elegant way to
  // do that, but for now, hardcoding may suffice

#define USE 1

#ifndef USE
  // ssh endpoint (cyder) ok
  endpoints_.push_back (endpoint_ (
   "cct-ssh"                                           , // name
   "ssh://cyder.cct.lsu.edu/"                          , // url
   "ssh"                                               , // ctype
   "amerzky"                                           , // user
   ""                                                  , // pass
   ""                                                  , // cert
   ""                                                  , // key
   ""                                                  , // proxy
   ""                                                  , // cadir
   "/home/amerzky/install/bin/saga-run.sh"             , // exe
   "/tmp"                                                // pwd
   ));
#endif

#ifndef USE
  // local endpoint (fork) ok
  endpoints_.push_back (endpoint_ (
   "local-fork"                                        , // name
   "fork://localhost/"                                 , // url
   "UserPass"                                          , // ctype
   ""                                                  , // user
   ""                                                  , // pass
   ""                                                  , // cert
   ""                                                  , // key
   ""                                                  , // proxy
   ""                                                  , // cadir
   "/Users/merzky/install/bin/saga-run.sh"             , // exe
   "/tmp"                                                // pwd
   ));
#endif
  
#ifndef USE
  // SMOA endpoint (BES) ok
  endpoints_.push_back (endpoint_ (
   "smoa-bes"                                          , // name
   "https://grass1.man.poznan.pl:19021"                , // url
   "UserPass"                                          , // ctype
   "ogf"                                               , // user
   "smoa-project.org"                                  , // pass
   ""                                                  , // cert
   ""                                                  , // key
   ""                                                  , // proxy
   "/Users/merzky/.saga/certificates/"                 , // cadir
   "/home/ogf/install/bin/saga-run.sh"                 , // exe
   "/home/ogf/"                                          // pwd
   ));
#endif
  
#ifndef USE
  // LONI endpoint (GRAM)
  endpoints_.push_back (endpoint_ (
   "loni-gram"                                         , // name
   "gram://qb1.loni.org/"                              , // url
   "x509"                                              , // ctype
   ""                                                  , // user
   ""                                                  , // pass
   ""                                                  , // cert
   ""                                                  , // key
   "/tmp/x509up_u501"                                  , // proxy
   "/Users/merzky/.globus/certificates/"               , // cadir
   "/home/merzky/install/bin/saga-run.sh"              , // exe
   "/home/merzky/"                                       // pwd
   ));
#endif
  
#ifdef USE
  // ARC endpoint (BES)
  endpoints_.push_back (endpoint_ (
   "arc-bes"                                           , // name
   "https://interop.grid.niif.hu:60000/arex-ut"        , // url
   "UserPass"                                          , // ctype
   "ogf30"                                             , // user
   "ogf30"                                             , // pass
   "/tmp/x509up_u501"                                  , // cert
   "/tmp/x509up_u501"                                  , // key
   ""                                                  , // proxy
   "/home/merzky/.saga/certificates/"                  , // cadir
   "/usr/local/saga/bin/saga-run.sh"                   , // exe
   "/home/arc/"                                          // pwd
   ));
#endif
  
#ifdef USE
  // Genesis-II endpoint (BES)
  endpoints_.push_back (endpoint_ (
   "genesis2-bes"                                      , // name
   "epr://localhost/Users/merzky/.saga/fg_india.epr"   , // url
   "UserPass"                                          , // ctype
   "ogf30"                                             , // user
   "ogf30"                                             , // pass
   "/tmp/x509up_u501"                                  , // cert
   "/tmp/x509up_u501"                                  , // key
   ""                                                  , // proxy
   "/home/merzky/.saga/certificates/"                  , // cadir
   "/usr/local/saga/bin/saga-run.sh"                   , // exe
   "/home/arc/"                                          // pwd
   ));
#endif
  
#ifdef USE
  // EC2 endpoint (AWS)
  endpoints_.push_back (endpoint_ (
   "ec2-aws"                                           , // name
   "ec2://"                                            , // url
   "aws"                                               , // ctype
   "ogf30"                                             , // user
   "ogf30"                                             , // pass
   "/tmp/x509up_u501"                                  , // cert
   "/tmp/x509up_u501"                                  , // key
   ""                                                  , // proxy
   "/home/merzky/.saga/certificates/"                  , // cadir
   "/usr/local/saga/bin/saga-run.sh"                   , // exe
   "/home/arc/"                                          // pwd
   ));
#endif


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


    std::cout << "created job number " 
              << n + 1 << "/" << njobs 
              << " on " 
              << ep.url_
              << " : " 
              << j.get_job_id ()
              << std::endl;

    // keep job
    jobs_.push_back (j);
  }
}

