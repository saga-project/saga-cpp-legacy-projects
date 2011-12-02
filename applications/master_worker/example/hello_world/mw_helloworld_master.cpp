
#include <stdio.h>
#include <master_worker.hpp>

void prompt (void)
{
  // std::cout << " > " << std::flush;
  // ::getchar ();
}


int main ()
{
  try 
  {
    // start a master
    saga_pm::master_worker::master m;

    // create a worker description
    saga_pm::master_worker::worker_description wd;

    std::vector <std::string> args;
    args.push_back ("mw_helloworld_worker"); // executable name
    wd.jd.set_attribute        (saga::job::attributes::description_executable, "saga-run.sh");
    wd.jd.set_vector_attribute (saga::job::attributes::description_arguments,  args);


    // create one worker on localhost
    wd.rm = "fork://localhost/";
    id_t id = m.worker_start (wd);

    // run the hello
    m.worker_run  (id, "hello");
    m.worker_wait (id);

    saga_pm::master_worker::argvec_t res = m.worker_get_results (id);
    for ( unsigned int i = 0; i < res.size (); i++ )
    {
      std::cout << "hello " << res[i] << std::endl;
    }

    // prepare worker for next run
    m.worker_reset (id);

    // tell worker to quit
    m.worker_run  (id, "quit");
    m.worker_wait (id);

    LOG << "MASTER DONE" << std::endl;
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

