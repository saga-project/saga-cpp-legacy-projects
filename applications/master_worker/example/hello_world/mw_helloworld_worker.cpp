
#include <saga/saga.hpp>

#include <master_worker.hpp>

class my_worker : public saga_pm::master_worker::worker
{
  public:
    my_worker (saga::url u)
      : saga_pm::master_worker::worker (u)
    {
      register_task ("hello", this, saga_pm::master_worker::to_voidstar (0, &my_worker::call_hello));
    }

    saga_pm::master_worker::argvec_t call_hello (saga_pm::master_worker::argvec_t av)
    {
      saga_pm::master_worker::argvec_t ret;

      ret.push_back ("world");

      char host[256];
      ::gethostname (host, 256); // ignore errors
      ret.push_back (host);

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
      LOG << "WORKER START";

      // create worker class, which registers with master
      // and also adds calls to the call_map
      saga::url u (argv[1]);
      my_worker w (u); 

      // run the worker loop
      w.run ();

      LOG << "WORKER DONE";
    }
    else
    {
      LOG << "need 1 argument (advert url)";
      exit (-1);
    }
  }
  catch ( const saga::exception & e )
  {
    LOG << " saga exception: " << e.what ();
  }

  catch ( const std::exception & e )
  {
    LOG << " std exception: " << e.what ();
  }

  catch ( ... )
  {
    LOG << " some exception ";
  }

  return 0;
}

