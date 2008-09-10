
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <saga/saga.hpp>


// a simple logger class, self explainnig.  only interesting thing is that it
// uses saga::filesystem::file for I/O
class logger
{
  private:
    saga::filesystem::file f_;
    std::string            name_;
    saga::url              loc_;
    saga::url              dir_;

  public:
    logger (const std::string & name);
   ~logger (void);

    void open  (void);
    void close (void);


    template <typename T1>
    void log (T1 const & t1)
    {
      // open ();
      SAGA_OSSTREAM tmp;

      tmp       << t1;
   // std::cout << t1;

      f_.write (saga::buffer (SAGA_OSSTREAM_GETSTRING (tmp)));

      // close ();
    }
};

#endif // LOGGER_HPP

