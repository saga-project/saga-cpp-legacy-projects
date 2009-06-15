
#include "digedag.hpp"
#include "util/thread.hpp"

int main (int argc, char** argv)
{
  try
  {
    if ( 1 )
    {
      digedag::dag * d = new digedag::dag;

      digedag::node_description nd; // inherits saga::job::description

      digedag::node * n1 = new digedag::node (nd);
      digedag::node * n2 = new digedag::node (nd);

      digedag::edge * e1 = new digedag::edge ("/tmp/s1", "/tmp/t2");

      d->add_node ("1", n1);
      d->add_node ("2", n2);

      d->add_edge (e1, n1, n2);

      d->fire ();

      std::cout << "dag    running..." << std::endl;

      // wait til the dag had a chance to finish
      while ( digedag::Running == d->get_state () )
      {
        ::sleep (1);
        std::cout << "dag    waiting..." << std::endl;
      }

      // this is the same:
      s.wait ();

      delete d;
    }

    if ( 1 )
    {
      digedag::dag  * d  = new digedag::dag;

      digedag::node * n1 = new digedag::node ("/tmp/node_job.sh node_1"
                                                " -i /tmp/in_1"
                                                " -o /tmp/out_1_a"
                                                " -o /tmp/out_1_b");

      digedag::node * n2 = new digedag::node ("/tmp/node_job.sh node_2"
                                                " -i /tmp/in_2" 
                                                " -o /tmp/out_2_a"
                                                " -o /tmp/out_2_b"
                                                " -o /tmp/out_2_c");

      digedag::node * n3 = new digedag::node ("/tmp/node_job.sh node_3"
                                                " -i /tmp/in_3_a" 
                                                " -i /tmp/in_3_b" 
                                                " -i /tmp/in_3_c" 
                                                " -o /tmp/out_3_a"
                                                " -o /tmp/out_3_b"
                                                " -o /tmp/out_3_c");

      digedag::node * n4 = new digedag::node ("/tmp/node_job.sh node_4"
                                                " -i /tmp/in_4_a" 
                                                " -i /tmp/in_4_b" 
                                                " -i /tmp/in_4_c" 
                                                " -o /tmp/out_4");

      digedag::node * n5 = new digedag::node ("/tmp/node_job.sh node_5"
                                                " -i /tmp/in_5_a" 
                                                " -i /tmp/in_5_b" 
                                                " -o /tmp/out_5");

      digedag::node * n6 = new digedag::node ("/tmp/node_job.sh node_6"
                                                " -i /tmp/in_6" 
                                                " -o /tmp/out_6_a");

      digedag::node * n7 = new digedag::node ("/tmp/node_job.sh node_7"
                                                " -i /tmp/in_7_a" 
                                                " -o /tmp/out_7");

      digedag::edge * e1 = new digedag::edge ("/tmp/out_1_a", "/tmp/in_4_a");
      digedag::edge * e2 = new digedag::edge ("/tmp/out_1_b", "/tmp/in_3_a");
      digedag::edge * e3 = new digedag::edge ("/tmp/out_2_a", "/tmp/in_3_b");
      digedag::edge * e4 = new digedag::edge ("/tmp/out_2_b", "/tmp/in_3_c");
      digedag::edge * e5 = new digedag::edge ("/tmp/out_2_c", "/tmp/in_5_b");
      digedag::edge * e6 = new digedag::edge ("/tmp/out_3_a", "/tmp/in_4_b");
      digedag::edge * e7 = new digedag::edge ("/tmp/out_3_b", "/tmp/in_4_c");
      digedag::edge * e8 = new digedag::edge ("/tmp/out_3_c", "/tmp/in_5_a");
      digedag::edge * e9 = new digedag::edge ("/tmp/out_6_a", "/tmp/in_7_a");

      d->add_node ("node_1", n1);
      d->add_node ("node_2", n2);
      d->add_node ("node_3", n3);
      d->add_node ("node_4", n4);
      d->add_node ("node_5", n5);
      d->add_node ("node_6", n6);

      d->add_edge (e1, n1, n4);
      d->add_edge (e2, n1, n3);
      d->add_edge (e3, n2, n3);
      d->add_edge (e4, n2, n3);
      d->add_edge (e5, n2, n5);
      d->add_edge (e6, n3, n4);
      d->add_edge (e7, n3, n4);
      d->add_edge (e8, n3, n5);
      d->add_edge (e9, n6, n7);

      d->fire ();

      d->wait ();

      delete d;
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
  catch ( const saga::exception & e )
  {
    std::cerr << "saga exception: " << e.what () << std::endl;
  }
}

