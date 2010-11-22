
#include <saga/saga.hpp>

#include "master_worker.hpp"
#include "master.hpp"
#include "worker.hpp"

int main (int argc, char** argv)
{
  // init logging
  saga::session s; 

  try 
  {
    if ( saga_pm::master_worker::is_master () )
    {
      LOG << "MASTER";

      saga_pm::master_worker::master m (1, "test_mw");

      // m.run ();
      // m.wait ();

      LOG << "MASTER DONE";
    }
    else
    {
      LOG << "WORKER";

      saga_pm::master_worker::call_map_t call_map;

      LOG << "worker 1";

      saga_pm::master_worker::worker w (call_map);

      LOG << "worker 2";
      w.run ();

      LOG << "WORKER DONE";
    }
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

