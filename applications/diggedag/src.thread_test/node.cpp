
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
    state_ = Running;

    thread_ = boost::thread (boost::bind (&diggedag::thread_startup, this));
  }


  void node::thread_work (void)
  {
    try
    {
      saga::job::description jd (jd_);

      saga::job::service js;
      saga::job::job j = js.create_job (jd);

      j.run  ();
      j.wait ();

      if ( j.get_state () != saga::job::Done )
      {
        std::cout << " job failed - cancel" << std::endl;

        state_ = Failed;

        return;
      }
      else
      {
        std::cout << " job done" << std::endl;
      }
    }
    catch ( const saga::exception & e )
    {
      std::cout << " job execution threw exception - cancel\n"
                << e.what () << std::endl;

      state_ = Failed;

      return;
    }

    state_ = Ready;
  }

} // namespace diggedag

