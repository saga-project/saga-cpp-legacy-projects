
#include "endpoint.hpp"

endpoint::endpoint (std::string           name,
                    mb_util::ini::section ini)
  : name_      (name)
  , ini_       (ini)
  , valid_     (false)
  , cnt_jreq_  (0)
  , cnt_jrun_  (0)
  , cnt_jreg_  (0)
  , cnt_iass_  (0)
  , cnt_iok_   (0)
  , cancelled_ (false)
{
  ctype_  =         ini_.get_entry ("ctype", "" );
  url_    =         ini_.get_entry ("url"  , "" );
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
  debug_  = ::atoi (ini_.get_entry ("debug", "0").c_str ());

  // ini_.dump ();

  try
  {
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

    // std::cout << " contexttype   : " << ctype_     << std::endl;
    // std::cout << " usercert      : " << cert_      << std::endl;
    // std::cout << " userkey       : " << key_       << std::endl;
    // std::cout << " userid        : " << user_      << std::endl;
    // std::cout << " userpass      : " << pass_      << std::endl;
    // std::cout << " certrepository: " << cadir_     << std::endl;
    // std::cout << " proxy         : " << proxy_     << std::endl;

    saga::job::service js (s, url_);

    service_ = js;
    valid_   = true;
  }
  catch ( const saga::exception & e )
  {
    std::cout << "endpoint startup failed:\n";
    std::cout << e.what () << std::endl;

    log_ << "endpoint startup failed:\n";
    log_ << e.what ();
  }
}


endpoint::~endpoint (void)
{
  cancel ();
}

void endpoint::cancel (void)
{
  if ( cancelled_ )
    return;

  // write log of events
  std::ofstream fout ((std::string ("endpoint.") + name_ + ".txt").c_str ());

  fout << "================================" << std::endl;
  fout << "endpoint ini section"             << std::endl;
  fout << "================================" << std::endl;
  
  ini_.dump (0, fout);
  
  fout << "================================" << std::endl;
  fout << log_.str ();
  fout << "================================" << std::endl;
  if ( valid_ ) {
    fout << "  status    : ok"                 << std::endl;
  } else {
    fout << "  status    : failed"             << std::endl;
  }
  fout << "  #jobreq   : " << cnt_jreq_      << std::endl;
  fout << "  #jobok    : " << cnt_jrun_      << std::endl;
  fout << "  #jobreg   : " << cnt_jreg_      << std::endl;
  fout << "  #itemreq  : " << cnt_iass_      << std::endl;
  fout << "  #itemok   : " << cnt_iok_       << std::endl;
  fout << "================================" << std::endl;
  fout.close ();

  cancelled_ = true;
}

void endpoint::dump (void)
{
  std::cout << "dumping   endpoint " << name_ << " (" << this << ")" << std::endl;

  std::cout << "================================" << std::endl;
  std::cout << "endpoint ini section"             << std::endl;
  std::cout << "================================" << std::endl;
  
  ini_.dump ();
  
  std::cout << "================================" << std::endl;
  std::cout << log_.str ();
  std::cout << "================================" << std::endl;
  if ( valid_ ) {
    std::cout << "  status    : ok"                 << std::endl;
  } else {
    std::cout << "  status    : failed"             << std::endl;
  }
  std::cout << "  #jobreq   : " << cnt_jreq_      << std::endl;
  std::cout << "  #jobok    : " << cnt_jrun_      << std::endl;
  std::cout << "  #jobreg   : " << cnt_jreg_      << std::endl;
  std::cout << "  #itemreq  : " << cnt_iass_      << std::endl;
  std::cout << "  #itemok   : " << cnt_iok_       << std::endl;
  std::cout << "================================" << std::endl;
}

