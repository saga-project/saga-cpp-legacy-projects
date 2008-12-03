
#include "logger.hpp"

#define DIR "./"


logger::logger (const std::string & name)
    : name_ (name),
      dir_  (DIR)
{
  open ();
  log ("\n -- opening log ------------------------------ \n");
}

logger::~logger (void)
{
  log (" -- closing log ------------------------------ \n\n");
  close ();
}

void logger::open (void)
{
  saga::filesystem::directory d (dir_, saga::filesystem::Write);

  f_ = d.open (saga::url (name_), 
               saga::filesystem::Create |
               saga::filesystem::Write  | 
               saga::filesystem::Append );
}

void logger::close (void)
{
  f_.close ();
}


