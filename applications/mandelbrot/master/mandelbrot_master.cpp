
#include <string>
#include <iostream>

#include "mandelbrot.hpp"

int main (int argc, char** argv)
{
  int ret = 0;

  try
  {
    ////////////////////////////////////////////////////////////////////
    std::string ini_file ("./mandelbrot.ini");

    if ( argc > 1 )
    {
      ini_file = argv[1];
      std::cout << "using ini_file from argv: " << ini_file << std::endl;
    }
    else
    {
      // get mandelbrot ini_file from env
      char* env = ::getenv ("SAGA_MANDELBROT_INI");
      if ( NULL != env )
      {
        ini_file = ::strdup (env);
        std::cout << "using ini file from $SAGA_MANDELBROT_INI: " << ini_file << std::endl;
      }
      else
      {
        std::cout << "using default ini file: " << ini_file << std::endl;
      }
    }

    ////////////////////////////////////////////////////////////////////

    // timestamp
    system ("/bin/date");

    // create our mandelbrot master
    mandelbrot m (ini_file); 

    // compute() distributes the work, gathers the
    // results, and displays the mandelbrot set
    ret = m.compute    ();

    // timestamp
    system ("/bin/date");

    ////////////////////////////////////////////////////////////////////
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

  // nothing to catch - report compute's result :-)
  return ret;
}


