
#include <saga/saga.hpp>

#include <master_worker.hpp>

int main (int argc, char** argv)
{
  try 
  {
    saga::job::service js; // init logging :-(

    LOG << "WORKER" << std::endl;

    if ( argc == 2 )
    {

      LOG << "WORKER START" << std::endl;

      saga_pm::master_worker::worker w (saga::url (argv[1]));

      LOG << "WORKER DONE" << std::endl;
    }
    else
    {
      LOG << "need 1 arguments (advert url)" << std::endl;
      exit (-1);
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

