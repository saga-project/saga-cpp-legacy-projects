
#include <saga/saga.hpp>

#include <boost/bind.hpp>

#include "node.hpp"


namespace diggedag
{
  class node;
  extern "C"
  {
    void thread_startup (void * arg)
    {
      diggedag::node * n = (diggedag::node *) arg;
      n->thread_work ();
    }
  }

  void node::fire (void)
  {
    thread_ = boost::thread (boost::bind (&diggedag::thread_startup, this));
  }


  void node::thread_work (void)
  {
    try
    {
      saga::job::service js;
      saga::job::job j = js.run_job ("/usr/bin/touch /tmp/test");

      j.wait ();
    }
    catch ( const saga::exception & e )
    {
      std::cout << " job execution threw exception - cancel\n"
                << e.what () << std::endl;

      return;
    }
  }

} // namespace diggedag

