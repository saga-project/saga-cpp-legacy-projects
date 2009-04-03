
#include "diggedag.hpp"
#include "util/thread.hpp"

class test : public diggedag::util::thread
           , public diggedag::util::uid
{
  public:
    test (void) 
    {
      std::cout << "1 thread id for " << uid_get () << " is " << ::pthread_self () << std::endl;
    }
    
    ~test (void) 
    {
      std::cout << "4 thread id for " << uid_get () << " is " << ::pthread_self () << std::endl;
    }

    void thread_work (void)
    {
      std::cout << "2 thread id for " << uid_get () << " is " << ::pthread_self () << std::endl;
    }

    void thread_cb (void)
    {
      std::cout << "3 thread id for " << uid_get () << " is " << ::pthread_self () << std::endl;
    }
};



int main (int argc, char** argv)
{
  try
  {
    if ( 0 )
    {
      test t1;
      test t2;

      t1.thread_run ();
      t2.thread_run ();

      t1.thread_wait ();
      t2.thread_wait ();
    }

    if ( 0 )
    {
      diggedag::dag d;

      diggedag::node::description nd;

      diggedag::node n1 (nd, "1");
      diggedag::node n2 (nd, "2");

      diggedag::edge e1 ("s1", "t2");

      d.add_node (n1);
      d.add_node (n2);

      d.add_edge (e1, n1, n2);

      d.fire ();

      std::cout << "dag    running..." << std::endl;
      // wait til the dag had a chance to finish
      while ( diggedag::Running == d.get_state () )
      {
        ::sleep (1);
        std::cout << "dag    waiting..." << std::endl;
      }

      ::sleep (3);
    }

    if ( 1 )
    {
      diggedag::dag d1;

      diggedag::node::description nd;
      diggedag::node n1 (nd, "1");
      diggedag::node n2 (nd, "2");
      diggedag::node n3 (nd, "3");
      diggedag::node n4 (nd, "4");
      diggedag::node n5 (nd, "5");
      diggedag::node n6 (nd, "6");
      diggedag::node n7 (nd, "7");

      diggedag::edge e1 ("s1", "t4");
      diggedag::edge e2 ("s1", "t3");
      diggedag::edge e3 ("s2", "t3");
      diggedag::edge e4 ("s2", "t3");
      diggedag::edge e5 ("s2", "t5");
      diggedag::edge e6 ("s3", "t4");
      diggedag::edge e7 ("s3", "t4");
      diggedag::edge e8 ("s6", "t7");

      d1.add_node (n1);
      d1.add_node (n2);
      d1.add_node (n3);
      d1.add_node (n4);
      d1.add_node (n5);
      d1.add_node (n6);
      d1.add_node (n7);

      d1.add_edge (e1, n1, n4);
      d1.add_edge (e2, n1, n3);
      d1.add_edge (e3, n2, n3);
      d1.add_edge (e4, n2, n3);
      d1.add_edge (e5, n2, n5);
      d1.add_edge (e6, n3, n4);
      d1.add_edge (e7, n3, n4);
      d1.add_edge (e8, n6, n7);

      diggedag::dag d2 = d1;

      d2.fire ();

      ::sleep (3);
      
      // wait til the dag had a chance to finish
      while ( diggedag::Running == d2.get_state () )
      {
        std::cout << "dag    waiting..." << std::endl;
        ::sleep (1);
      }
    }

    std::cout << "done" << std::endl;
  }
  catch ( const char * s )
  {
    std::cerr << "char* exception: " << s << std::endl;
  }
  catch ( const std::string & s )
  {
    std::cerr << "string exception: " << s << std::endl;
  }
}

