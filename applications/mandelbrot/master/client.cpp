
#include "client.hpp"

client::client (std::string                  name, 
                saga::job::job               job, 
                boost::shared_ptr <endpoint> ep)
  : name_   (name)
  , job_    (job)
  , ep_     (ep)
  , cnt_i1_ (0)
  , cnt_i2_ (0)
  , valid_  (true)
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
  log_ << "#assigned : " << cnt_i1_           << "\n";
  log_ << "#done     : " << cnt_i2_           << "\n";
  log_ << "last state: " << job_.get_state () << "\n";

  // write log of events
  std::ofstream fout ((std::string ("client.") + name_ + ".txt").c_str ());

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
    std::cout << "inconsistent client state!\n";
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
      std::cout << "killing   job " << name_ << " \t ... " << std::flush;
      job_.cancel ();

      if ( job_.get_state () == saga::job::Running )
      {
        std::cout << "Failed " << job_.get_state () << std::endl;
      }
      else
      {
        std::cout << "ok " << job_.get_state () << std::flush;
        std::cout << " "   << ep_->args_ << std::endl;
      }
    }
    catch ( const saga::exception & e ) 
    {
      std::cout << "failed " << job_.get_state () << std::endl;
      // so what? ;-)
    }

  }
}

