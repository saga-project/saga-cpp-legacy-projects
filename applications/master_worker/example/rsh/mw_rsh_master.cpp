
#include <stdio.h>
#include <master_worker.hpp>

std::string prompt (std::string msg = "")
{
  std::string line;

  std::cout << msg << " : " << std::flush;
  std::getline (std::cin, line);

  return line;
}

void output (saga_pm::master_worker::argvec_t lines)
{
  for ( unsigned int i = 0; i < lines.size (); i++ )
  {
    std::cout << lines[i] << std::endl;
  }
}


int main ()
{
  try 
  {
    // create a master, and init it's session
    saga_pm::master_worker::master m;

    // describe a worker to start
    saga_pm::master_worker::worker_description wd;

    // prompt user for target host (url), and set all other worker info
    std::string input = prompt ("worker rm url");
    wd.rm = input;

    std::vector <std::string> args;
    args.push_back ("mw_rsh_worker");

    wd.jd.set_attribute        (saga::job::attributes::description_executable, "saga-run.sh");
    wd.jd.set_vector_attribute (saga::job::attributes::description_arguments,  args);


    // create the worker on target host
    id_t id = m.worker_start (wd);
    if ( m.worker_get_state (id) == saga_pm::master_worker::Failed )
    {
      std::cerr << "could not start worker" << std::endl;
      return (-1);
    }


    // now we prompt for commands, and send them to the (single) worker.
    // The command is there executed as shell command, and the output of the
    // command is retuned as result vec, and here printed.  If that is done, we
    // prompt for the next command, until 'quit' stops that loop.

    input = prompt ("command");

    while ( input != "quit" )
    {
      // run the given command on the rsh worker
      saga_pm::master_worker::argvec_t cmd;
      cmd.push_back (input);

      m.worker_run  (id, "rsh", cmd);
      m.worker_wait (id);

      // check what happened
      switch ( m.worker_get_state () )
      {
        case saga_pm::master_worker::Done :
          output (m.worker_get_results (id));
          break;

        case saga_pm::master_worker::Failed :
          std::cerr << "error: " << m.worker_get_error () << std::endl;
          break;

        default :
          std::cerr << "error: worker in unknown state!" << std::endl;
      }

      // reset the worker into a (hopefully) clean state
      m.worker_reset (id);

      // ask for next command or 'quit'
      input = prompt ("command");
    }


    // tel worker to shut down
    m.worker_run  (id, "quit");
    m.worker_wait (id);
    m.worker_stop (id);
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

