
#include <string>
#include <iostream>

#include "mandelbrot.hpp"

int main (int argc, char** argv)
{
  try
  {
    {
      mandelbrot m;

      m.set_output ("x11");
      m.set_njobs  (1);
      m.compute    ();

      std::cout << "\n done - press key to finish\n";
      ::getchar ();
    }
  }
  catch ( saga::exception const & e )
  {
    std::cerr << "SAGA Exception: " << e.what () << std::endl;
    return -1;
  }
  catch ( char const * s )
  {
    std::cerr << "Exception: " << s << std::endl;
    return -2;
  }
  catch ( std::string const & s )
  {
    std::cerr << "Exception: " << s << std::endl;
    return -3;
  }
  catch ( std::exception const & e )
  {
    std::cerr << "STD Exception: " << e.what () << std::endl;
    return -3;
  }
  catch ( ... )
  {
    std::cerr << "Exception: unknown error" << std::endl;
    return -3;
  }

  return 0;
}


