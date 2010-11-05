
#include <saga/saga.hpp>

#include "master_worker.hpp"
#include "master.hpp"
#include "worker.hpp"

int main ()
{
  try 
  {
    saga_pm::master_worker::argvec_t args;

    saga_pm::master_worker::master m (1, "test_mw", args);

    // m.run ();
    // m.wait ();
  }
  catch ( const saga::exception & e )
  {
    std::cout << " saga exception: " << e.what () << std::endl;
  }

  catch ( const std::exception & e )
  {
    std::cout << " std exception: " << e.what () << std::endl;
  }

  catch ( ... )
  {
    std::cout << " some exception " << std::endl;
  }

  return 0;
}

