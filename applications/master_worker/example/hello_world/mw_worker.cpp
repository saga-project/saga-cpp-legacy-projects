
#include <saga/saga.hpp>

#include <master_worker.hpp>

class my_worker : public saga_pm::master_worker::worker
{
  public:
    my_worker (saga::url u)
      : saga_pm::master_worker::worker (u)
    {
      register_task ("test", this, saga_pm::master_worker::to_voidstar (0, &my_worker::call_test));
    }

    saga_pm::master_worker::argvec_t call_test (saga_pm::master_worker::argvec_t av)
    {
      saga_pm::master_worker::argvec_t ret;

      LOG << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

      ret.push_back ("hello world");

      return ret;
    }
};


int main (int argc, char** argv)
{
  try 
  {
    saga::job::service js; // init logging :-(

    LOG << "WORKER" << std::endl;

    if ( argc == 2 )
    {

      LOG << "WORKER START" << std::endl;


      // create worker class, which registers with master
      // and also adds calls to the call_map
      saga::url u (argv[1]);
      my_worker w (u); 

      // run the worker loop
      w.run ();

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

