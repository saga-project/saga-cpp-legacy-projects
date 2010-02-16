
#include "digedag.hpp"
#include "util/thread.hpp"

int main (int argc, char** argv)
{
  try
  {
    boost::shared_ptr <digedag::dag>  d  (new digedag::dag);

    boost::shared_ptr <digedag::node> in  = d->create_node ();

    // boost::shared_ptr <digedag::node> n1  = d->create_node ("/usr/local/bin/wget"
    //                                                         " -O /tmp/a_1"
    //                                                         " -q"
    //                                                         " http://saga.cct.lsu.edu/software");

    // boost::shared_ptr <digedag::node> n2  = d->create_node ("/usr/local/bin/wget"
    //                                                         " -O /tmp/a_2"
    //                                                         " -q"
    //                                                         " http://saga.cct.lsu.edu/documentation");

    boost::shared_ptr <digedag::node> n1  = d->create_node ("/bin/cp"
                                                            " /tmp/x_1"
                                                            " /tmp/a_1");

    boost::shared_ptr <digedag::node> n2  = d->create_node ("/bin/cp"
                                                            " /tmp/x_2"
                                                            " /tmp/a_2");

    boost::shared_ptr <digedag::node> n3  = d->create_node ("/bin/cp"
                                                            " /tmp/b_1"
                                                            " /tmp/c_1");

    boost::shared_ptr <digedag::node> n4  = d->create_node ("/bin/mv"
                                                            " /tmp/d_1" 
                                                            " /tmp/e_1");

    boost::shared_ptr <digedag::node> out = d->create_node ();

    boost::shared_ptr <digedag::edge> e1  = d->create_edge ("/tmp/a_1", "/tmp/b_1");
    boost::shared_ptr <digedag::edge> e2  = d->create_edge ("/tmp/a_2", "/tmp/b_2");
    boost::shared_ptr <digedag::edge> e3  = d->create_edge ("/tmp/c_1", "/tmp/d_1");
    boost::shared_ptr <digedag::edge> e4  = d->create_edge ("/tmp/e_1", "/tmp/out");

    d->add_node ("INPUT",  in );
    d->add_node ("node_1", n1 );
    d->add_node ("node_2", n2 );
    d->add_node ("node_3", n3 );
    d->add_node ("node_4", n4 );
    d->add_node ("OUTPUT", out);

    d->add_edge (e1, n1, n3);
    d->add_edge (e2, n1, n3);
    d->add_edge (e3, n3, n4);
    d->add_edge (e4, n4, out);

    d->fire ();

    d->wait ();
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

  return 0;
}

