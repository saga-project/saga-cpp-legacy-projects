
#include "client.hpp"

client::client (std::string                  name, 
                saga::job::job               job, 
                boost::shared_ptr <endpoint> ep)
  : name_ (name)
  , job_  (job)
  , ep_   (ep)
{
  id_ = job_.get_job_id ();

  log_ << "endpoint : " << ep_->name_        << "\n";
  log_ << "id       : " << id_               << "\n";
  log_ << "number   : " << name_             << "\n";
  log_ << "state    : " << job_.get_state () << "\n";
}


client::~client (void)
{
  // write log of events
  std::ofstream fout ((std::string ("client.") + name_ + ".info").c_str ());

  fout << "================================" << std::endl;
  fout << log_.str ();
  fout << "================================" << std::endl;
  fout.close ();
}

