
#include <saga/saga.hpp>

#include <master_worker.hpp>

int main ()
{
  try 
  {
    saga::job::service js; // init logging :-(

    saga_pm::master_worker::master m;

    m.initialize ("test");
    std::cout << " ====== 1 " << std::endl;


    saga_pm::master_worker::worker_description wd;

    std::vector <std::string> args;
    args.push_back ("mw_worker");
    std::cout << " ====== 2 " << std::endl;

    wd.rm = "fork://localhost/";
    wd.jd.set_attribute        (saga::job::attributes::description_executable, "saga-run.sh");
    wd.jd.set_vector_attribute (saga::job::attributes::description_arguments,  args);

    id_t id = m.worker_start (wd);
    m.worker_dump (id);
    std::cout << " ====== 3 " << std::endl;

    m.worker_run  (id, "test");
    m.worker_dump (id);
    std::cout << " ====== 4 " << std::endl;

    m.worker_wait (id);
    m.worker_dump (id);
    std::cout << " ====== 5 " << std::endl;

    saga_pm::master_worker::argvec_t res = m.worker_get_results (id);
    std::cout << "results for test()" << std::endl;
    for ( unsigned int i = 0; i < res.size (); i++ )
    {
      std::cout << "  " << i << " : " << res[i] << std::endl;
    }
    std::cout << " ====== 6 " << std::endl;


    m.worker_run  (id, "quit");
    m.worker_dump (id);
    std::cout << " ====== 7 " << std::endl;

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

