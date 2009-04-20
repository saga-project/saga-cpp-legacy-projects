
#include "diggedag.hpp"
#include "util/thread.hpp"

class test : public diggedag::util::thread
{
  public:
    test (void) 
    {
      std::cout << "1 thread id is " << ::pthread_self () << std::endl;
    }
    
    ~test (void) 
    {
      std::cout << "4 thread id is " << ::pthread_self () << std::endl;
    }

    void thread_work (void)
    {
      std::cout << "2 thread id is " << ::pthread_self () << std::endl;
    }

    void thread_cb (void)
    {
      std::cout << "3 thread id is " << ::pthread_self () << std::endl;
    }
};


// FIXME: we should use thread callbacks for state changes, everywhere.  Just
// for the beauty of it ;-)
//
int main (int argc, char** argv)
{
  try
  {
    if ( 1 )
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

      diggedag::node_description nd;

      diggedag::node n1 (nd);
      diggedag::node n2 (nd);

      diggedag::edge e1 ("/tmp/s1", "/tmp/t2");

      d.add_node ("1", n1);
      d.add_node ("2", n2);

      d.add_edge (e1, n1, n2);

      d.fire ();

      std::cout << "dag    running..." << std::endl;
      // wait til the dag had a chance to finish
      while ( diggedag::Running == d.get_state () )
      {
        ::sleep (1);
        std::cout << "dag    waiting..." << std::endl;
      }
    }

    if ( 1 )
    {
      diggedag::dag d1;

      diggedag::node n1   ("/tmp/node_job.sh node_1"
                           " -i /tmp/in_1"
                           " -o /tmp/out_1_a"
                           " -o /tmp/out_1_b");

      diggedag::node n2   ("/tmp/node_job.sh node_2"
                           " -i /tmp/in_2" 
                           " -o /tmp/out_2_a"
                           " -o /tmp/out_2_b"
                           " -o /tmp/out_2_c");

      diggedag::node n3   ("/tmp/node_job.sh node_3"
                           " -i /tmp/in_3_a" 
                           " -i /tmp/in_3_b" 
                           " -i /tmp/in_3_c" 
                           " -o /tmp/out_3_a"
                           " -o /tmp/out_3_b"
                           " -o /tmp/out_3_c");

      diggedag::node n4   ("/tmp/node_job.sh node_4"
                           " -i /tmp/in_4_a" 
                           " -i /tmp/in_4_b" 
                           " -i /tmp/in_4_c" 
                           " -o /tmp/out_4");

      diggedag::node n5   ("/tmp/node_job.sh node_5"
                           " -i /tmp/in_5_a" 
                           " -i /tmp/in_5_b" 
                           " -o /tmp/out_5");

      diggedag::node n6   ("/tmp/node_job.sh node_6"
                           " -i /tmp/in_6" 
                           " -o /tmp/out_6_a");

      diggedag::node n7   ("/tmp/node_job.sh node_7"
                           " -i /tmp/in_7_a" 
                           " -o /tmp/out_7");

      diggedag::edge e1 ("/tmp/out_1_a", "/tmp/in_4_a");
      diggedag::edge e2 ("/tmp/out_1_b", "/tmp/in_3_a");
      diggedag::edge e3 ("/tmp/out_2_a", "/tmp/in_3_b");
      diggedag::edge e4 ("/tmp/out_2_b", "/tmp/in_3_c");
      diggedag::edge e5 ("/tmp/out_2_c", "/tmp/in_5_b");
      diggedag::edge e6 ("/tmp/out_3_a", "/tmp/in_4_b");
      diggedag::edge e7 ("/tmp/out_3_b", "/tmp/in_4_c");
      diggedag::edge e8 ("/tmp/out_3_c", "/tmp/in_5_a");
      diggedag::edge e9 ("/tmp/out_6_a", "/tmp/in_7_a");

      d1.add_node ("node_1", n1);
      d1.add_node ("node_2", n2);
      d1.add_node ("node_3", n3);
      d1.add_node ("node_4", n4);
      d1.add_node ("node_5", n5);
      d1.add_node ("node_6", n6);

      d1.add_edge (e1, n1, n4);
      d1.add_edge (e2, n1, n3);
      d1.add_edge (e3, n2, n3);
      d1.add_edge (e4, n2, n3);
      d1.add_edge (e5, n2, n5);
      d1.add_edge (e6, n3, n4);
      d1.add_edge (e7, n3, n4);
      d1.add_edge (e8, n3, n5);
      d1.add_edge (e9, n6, n7);

      diggedag::dag d2 = d1;

      d2.fire ();

      // wait til the dag had a chance to finish
      while ( diggedag::Running == d2.get_state () )
      {
        ::sleep (1);
        std::cout << "dag    waiting..." << std::endl;
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

