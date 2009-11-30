
#include <iostream>
#include <saga/saga.hpp>

int main ()
{
  try 
  {
    saga::url u_src ("any://localhost/tmp/0/2mass-atlas-990502s-j1460209.fits");
    saga::url u_tgt ("any://localhost/tmp/2/2mass-atlas-990502s-j1460209.fits");

    saga::filesystem::file f_src (u_src);
    f_src.copy (u_tgt, saga::filesystem::Overwrite | saga::filesystem::CreateParents);
  }
  catch ( const saga::exception & e )
  {
    std::cerr << e.what () << std::endl;
  }

  return (0);
}

