
#include <iostream>

#include "digedag.hpp"

const saga::session s;

#define N 100

class mythread : public digedag::util::thread
{
  private:
    saga::job::job j_;
    bool have_job_;

  public:
    mythread (void)
      : have_job_ (false)
    {
    }

    void thread_work (void)
    {
      saga::job::service js (s, "fork://localhost");
      j_ = js.run_job ("/bin/sleep 10", "localhost");
      have_job_ = true;

      j_.wait ();
    }

    saga::job::state get_state (void)
    {
      if ( ! have_job_ )
      {
        return saga::job::Unknown;
      }

      return j_.get_state ();
    }
};

int main (int argc, char** argv)
{
  int n = N;

  if ( argc >= 2 )
  {
    n = atoi (argv[1]);
  }

  try
  {
    std::vector <mythread> threads;

    std::cout << "Creating threads: " << std::flush;
    for ( int i = 0; i < n; i++ )
    {
      mythread t;
      threads.push_back (t);
      std::cout << "." << std::flush;
    }
    std::cout << std::endl;

    std::cout << "Running threads:  " << std::flush;
    for ( int i = 0; i < n; i++ )
    {
      threads[i].thread_run ();
      std::cout << "." << std::flush;
    }
    std::cout << std::endl;

    std::cout << "Waiting threads:  " << std::flush;
    bool done = 0;

    while ( ! done )
    {
      done = true;

      for ( int i = 0; i < n; i++ )
      {
        switch ( threads[i].get_state () )
        {
          case saga::job::Running:
            std::cout << "." << std::flush;
            done = false;
            break;
          case saga::job::Done:
            std::cout << "+" << std::flush;
            break;
          case saga::job::Failed:
            std::cout << "-" << std::flush;
            break;
          default:
            std::cout << "?" << std::flush;
            done = false;
            break;
        }
      }

      std::cout << std::endl;

      if ( ! done )
      {
        std::cout << "                  " << std::flush;
      }

      ::sleep (1);
    }

    std::cout << "all done" << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "oops exception: " << std::endl;
  }

  return 0;
}

