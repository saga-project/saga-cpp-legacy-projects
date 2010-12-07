
#include <string>
#include <iostream>

#include "mandelbrot.hpp"

int main (int argc, char** argv)
{
  ////////////////////////////////////////////////////////////////////
  // timestamp
  std::cout << "start   : " << std::flush;
  system ("/bin/date");

  int ret = 0;

  try
  {
    ////////////////////////////////////////////////////////////////////
    // create our mandelbrot master
    mandelbrot m;

    ////////////////////////////////////////////////////////////////////
    // compute() distributes the work, gathers the
    // results, and displays the mandelbrot set
    ret = m.compute    ();
  }

  // catch exceptions from SAGA
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA Exception: " << e.what () << std::endl;
    return -1;
  }

  // catch exceptions from the mandelbrot class and the output device
  catch ( char const * s )
  {
    std::cerr << "Mandelbrot Exception: " << s << std::endl;
    return -2;
  }

  // catch standard exceptions
  catch ( const std::exception & e )
  {
    std::cerr << "std::exception: " << e.what () << std::endl;
    return -3;
  }

  // catch anythin exceptions
  catch ( ... )
  {
    std::cerr << "some exception" << std::endl;
    return -3;
  }

  ////////////////////////////////////////////////////////////////////
  // timestamp
  std::cout << "stop    : " << std::flush;
  system ("/bin/date");

  ////////////////////////////////////////////////////////////////////
  // nothing to catch - report compute's result :-)
  return ret;
}


