
#include "logger.hpp"

#define DIR "./"


logger::logger (const std::string & name)
    : name_ (name),
      dir_  (DIR)
{
  saga::filesystem::directory d (dir_, saga::filesystem::Write);

  f_ = d.open (saga::url (name_), saga::filesystem::Create | saga::filesystem::Write);

  SAGA_OSSTREAM tmp;

  tmp << "\n -- opening log ------------------------------ \n";

  f_.write (saga::buffer (SAGA_OSSTREAM_GETSTRING (tmp)));
}

logger::~logger (void)
{
  close ();
}

void logger::close (void)
{
  SAGA_OSSTREAM tmp;

  tmp << " -- closing log ------------------------------ \n\n";

  f_.write (saga::buffer (SAGA_OSSTREAM_GETSTRING (tmp)));
  f_.close ();
}


