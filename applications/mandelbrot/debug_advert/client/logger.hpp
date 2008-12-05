
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <sstream>

#include <saga/saga.hpp>


class logger
{
  private:
    saga::filesystem::file f_;

  public:
    logger (const std::string & name)
      : f_ (saga::url (name), 
            saga::filesystem::Create |
            saga::filesystem::Write  | 
            saga::filesystem::Append ) 
        // FIXME: append seems to be broken in thye default file adaptor?
    {
      log ("\n -- opening log ------------------------------ \n");
    }

    ~logger (void)
    {
      log (" -- closing log ------------------------------ \n\n");
    }


    template <typename T1> void log (T1 const & t1)
    {
      std::stringstream ss;

      ss        << t1;
      std::cout << t1;

      f_.write (saga::buffer (ss.str ()));
    }
};

#endif // LOGGER_HPP

