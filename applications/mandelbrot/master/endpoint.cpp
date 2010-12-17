
#include "endpoint.hpp"

endpoint::endpoint (std::string           name,
                      mb_util::ini::section ini)
  : name_   (name)
  , ini_    (ini)
  , valid_  (true)
  , cnt_j1_ (0)
  , cnt_j2_ (0)
  , cnt_j3_ (0)
  , cnt_i1_ (0)
  , cnt_i2_ (0)
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
    log_ << "endpoint startup failed:\n";
    log_ << e.what ();

    valid_ = false;
  }
}


endpoint::~endpoint (void)
{
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
  fout << "  #jobreq   : " << cnt_j1_          << std::endl;
  fout << "  #jobok    : " << cnt_j2_          << std::endl;
  fout << "  #jobreg   : " << cnt_j3_          << std::endl;
  fout << "  #itemreq  : " << cnt_i1_          << std::endl;
  fout << "  #itemok   : " << cnt_i2_          << std::endl;
  fout << "================================" << std::endl;
  fout.close ();
}

