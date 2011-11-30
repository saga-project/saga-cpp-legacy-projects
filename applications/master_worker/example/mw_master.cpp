
#include <saga/saga.hpp>

#include <master_worker.hpp>

int main ()
{
  try 
  {
    saga::job::service js; // init logging :-(

    saga_pm::master_worker::master m;

    m.initialize ("test");

    LOG << "MASTER TEST" << std::endl;

    LOG << "MASTER init done" << std::endl;

    saga_pm::master_worker::worker_description wd;

    std::vector <std::string> args;
    args.push_back ("mw_worker");

    wd.rm = "fork://localhost/";
    wd.jd.set_attribute        (saga::job::attributes::description_executable, "saga-run.sh");
    wd.jd.set_vector_attribute (saga::job::attributes::description_arguments,  args);

    m.worker_start (wd);

    std::vector <saga_pm::master_worker::id_t> ids = m.worker_list ();

    for ( unsigned int i = 0; i < ids.size (); i++ )
    {
      m.worker_dump (ids[i]);
    }

    LOG << "MASTER started worker" << std::endl;

    m.worker_run ("quit");

    LOG << "MASTER sent quit worker" << std::endl;

    for ( unsigned int i = 0; i < ids.size (); i++ )
    {
      m.worker_dump (ids[i]);
    }

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

