
#include <saga/saga.hpp>

#include <master_worker.hpp>

class my_worker : public saga_pm::master_worker::worker
{
  public:
    my_worker (saga::url u)
      : saga_pm::master_worker::worker (u)
    {
      register_task ("rsh", this, saga_pm::master_worker::to_voidstar (0, &my_worker::call_rsh));
    }

    saga_pm::master_worker::argvec_t call_rsh (saga_pm::master_worker::argvec_t args)
    {
      // we interpret the arguments to the call as shell command line, run it
      // locally (via saga::job), and return the resulting output as array of
      // lines.

      saga_pm::master_worker::argvec_t ret;
      std::string                      cmd;

      for ( unsigned int i = 0; i < args.size (); i++ )
      {
        if ( i > 0 )
        {
          cmd == " ";
        }

        cmd += args[i];
      }


      saga::job::ostream in;
      saga::job::istream err;
      saga::job::istream out;

      saga::job::service js ("fork://localhost/");
      saga::job::job j = js.run_job (cmd, "localhost", in, out, err);

      j.wait ();

      std::string output;

      while ( std::getline (out, output) )
      {
        ret.push_back (output);
      }

      return ret;
    }
};


int main (int argc, char** argv)
{
  try 
  {
    saga::job::service js; // init logging :-(

    std::cerr << "WORKER" << std::endl << std::endl;
    LOG       << "WORKER" << std::endl;

    if ( argc == 2 )
    {
      std::cerr << "WORKER START" << std::endl;
      LOG       << "WORKER START";

      // create worker class, which registers with master
      // and also adds calls to the call_map
      saga::url u (argv[1]);
      my_worker w (u); 

      // run the worker loop
      w.run ();

      std::cerr << "WORKER DONE" << std::endl;
      LOG       << "WORKER DONE";
    }
    else
    {
      std::cerr << "need 1 argument (advert url)" << std::endl;
      LOG       << "need 1 argument (advert url)";
      exit (-1);
    }
  }
  catch ( const saga::exception & e )
  {
    std::cerr << " saga exception: " << e.what () << std::endl;
    LOG       << " saga exception: " << e.what ();
  }

  catch ( const std::exception & e )
  {
    std::cerr << " std exception: " << e.what () << std::endl;
    LOG       << " std exception: " << e.what ();
  }

  catch ( ... )
  {
    std::cerr << " some exception " << std::endl;
    LOG       << " some exception ";
  }

  return 0;
}

