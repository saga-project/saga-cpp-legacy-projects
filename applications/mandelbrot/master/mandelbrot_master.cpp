
#include <string>
#include <iostream>

#include "mandelbrot.hpp"

int main (int argc, char** argv)
{
  try
  {
    {
      system ("/bin/date");

      // create our mandelbrot master, open the x11 
      // output device, and start n client jobs
      mandelbrot m ("x11", 3); 

      // compute() distributes the work, gathers the
      // results, and displays the mandelbrot set
      m.compute    ();

      // done
      std::cout << "\n done - press key to finish\n";
      system ("/bin/date");
      ::getchar ();
    }
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
    std::cerr << "Exception: " << s << std::endl;
    return -2;
  }

  // nothing to catch - declare success :-)
  return 0;
}


