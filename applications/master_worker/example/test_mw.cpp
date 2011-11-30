
#include <saga/saga.hpp>

#include <master_worker.hpp>

int main (int argc, char** argv)
{
  // init logging
  saga::session s; 

  try 
  {
    if ( argc == 1 )
    {
      LOG << "MASTER START" << std::endl;

      saga_pm::master_worker::master m;
      m.initialize ("test");

      LOG << "MASTER TEST" << std::endl;

      saga_pm::master_worker::worker_description wd;

      wd.rm = "fork://localhost/";
      wd.jd.set_attribute (saga::job::attributes::description_executable, argv[0]);

      m.worker_start (wd);

      std::vector <saga_pm::master_worker::id_t> ids = m.worker_list ();

      for ( unsigned int i = 0; i < ids.size (); i++ )
      {
        m.worker_dump (ids[i]);
      }

      m.worker_run ("quit");

      for ( unsigned int i = 0; i < ids.size (); i++ )
      {
        m.worker_dump (ids[i]);
      }

      LOG << "MASTER DONE" << std::endl;
    }
    else if ( argc == 2 )
    {
      saga::job::service js;

      LOG << "WORKER START" << std::endl;

      saga_pm::master_worker::call_map_t call_map;

      saga_pm::master_worker::worker w (saga::url (argv[1]), call_map);

      w.run ();

      LOG << "WORKER DONE" << std::endl;
    }
    else
    {
      LOG << "need 0 (master) or 1 (client) arguments" << std::endl;
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

