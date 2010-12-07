
#include "client.hpp"

client::client (std::string                  name, 
                saga::job::job               job, 
                boost::shared_ptr <endpoint> ep)
  : name_  (name)
  , job_   (job)
  , ep_    (ep)
  , cnt_a_ (0)
  , cnt_d_ (0)
  , valid_ (true)
{
  id_ = job_.get_job_id ();

  id_short_ = id_;

  // trim jobid for readability
  if ( id_short_.size () > 54 )
  {

    id_short_[52] = '.';
    id_short_[53] = '.';
    id_short_[54] = '.';
  }
}


client::~client (void)
{
  cancel ();

  log_ << "number    : " << name_             << "\n";
  log_ << "id        : " << id_               << "\n";
  log_ << "short id  : " << id_short_         << "\n";
  log_ << "endpoint  : " << ep_->name_        << "\n";
  log_ << "#assigned : " << cnt_a_            << "\n";
  log_ << "#done     : " << cnt_d_            << "\n";
  log_ << "last state: " << job_.get_state () << "\n";

  // write log of events
  std::ofstream fout ((std::string ("client.") + name_ + ".info").c_str ());

  fout << "================================" << std::endl;
  fout << log_.str ();
  fout << "================================" << std::endl;
  fout.close ();
}

saga::job::state client::get_state (void)
{
  if ( ! valid_ )
  {
    return saga::job::Failed;
  }

  try
  {
    return job_.get_state ();
  }
  catch ( const saga::incorrect_state )
  {
    std::cerr << "inconsistent client state!\n";
    return saga::job::Unknown;
  }
}


void client::cancel (void)
{
  if ( valid_ )
  {
    valid_ = false;

    try 
    {
      std::cout << "killing   job " << name_ << std::endl;
      job_.cancel ();
    }
    catch ( const saga::exception & e ) 
    {
      // so what? ;-)
    }
  }

}

