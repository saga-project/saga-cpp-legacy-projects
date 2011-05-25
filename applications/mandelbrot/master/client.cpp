
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
  if ( id_short_.size () > 53 )
  {

    id_short_[50] = '.';
    id_short_[51] = '.';
    id_short_[52] = '.';
    id_short_.resize (53);
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
    ep_->log_ << "inconsistent client state!\n";
  }
  catch ( const saga::exception & e )
  {
    std::cout << "failure - could not get client state: " << e.what () << std::endl;
    ep_->log_ << "failure - could not get client state: " << e.what () << std::endl;
  }
  catch ( const std::exception & e )
  {
    std::cout << "failure - could not get client state: " << e.what () << std::endl;
    ep_->log_ << "failure - could not get client state: " << e.what () << std::endl;
  }
  catch ( char const * s )
  {
    std::cout << "failure - could not get client state" << std::endl;
    ep_->log_ << "failure - could not get client state" << std::endl;
  }
  catch ( ... )
  {
    std::cout << "failure - could not get client state" << std::endl;
    ep_->log_ << "failure - could not get client state" << std::endl;
  }

  return saga::job::Unknown;
}


void client::cancel (void)
{
  if ( valid_ )
  {
    valid_ = false;

    try 
    {
      if ( job_.get_state () == saga::job::Running )
      {
        std::cout << "killing   job " << name_ << " \t ... " << std::flush;
        job_.cancel ();

        if ( job_.get_state () == saga::job::Running )
        {
          std::cout << "cancel failed " << job_.get_state () << std::endl;
          ep_->log_ << "cancel failed " << job_.get_state () << std::endl;
        }
        else
        {
          std::cout << "ok " << job_.get_state () << std::flush;
          std::cout << " "   << ep_->args_ << std::endl;
        }
      }
    }
    catch ( const saga::exception & e )
    {
      std::cout << "failure - could not cancel client: " << e.what () << std::endl;
      ep_->log_ << "failure - could not cancel client: " << e.what () << std::endl;
    }
    catch ( const std::exception & e )
    {
      std::cout << "failure - could not cancel client: " << e.what () << std::endl;
      ep_->log_ << "failure - could not cancel client: " << e.what () << std::endl;
    }
    catch ( char const * s )
    {
      std::cout << "failure - could not cancel client" << std::endl;
      ep_->log_ << "failure - could not cancel client" << std::endl;
    }
    catch ( ... )
    {
      std::cout << "failure - could not cancel client" << std::endl;
      ep_->log_ << "failure - could not cancel client" << std::endl;
    }
  }
}

