
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <sstream>

#include <saga/saga.hpp>


// a simple logger class, self explainnig.  The only interesting thing 
// is that it uses saga::filesystem::file for I/O.  Log messages
// also go to stdout.
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
        // FIXME: append seems to be broken in the default file adaptor?
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
      std::cout << t1 << std::endl;

      f_.write (saga::buffer (ss.str ()));
    }
};

#endif // LOGGER_HPP

