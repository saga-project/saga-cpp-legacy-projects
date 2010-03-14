#include <iostream>

#include "../src/dag.hpp"
#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  try
  {
    if ( argc != 3 )
    { 
      std::cerr << "\n\tusage: " << argv[0] << " <dag.xml> <scheduler.txt>\n\n";
      return -1;
    }

    digedag::dax::parser p (argv[1], argv[2]);

    boost::shared_ptr <digedag::dag> d = p.get_dag ();

    std::cout << "DAG: node count: " << d->get_nodes_count()
              << " / edge count: " << d->get_edges_count()
              << std::endl;
        exit(0);

  }
  catch ( char const * s )
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
  catch ( ... )
  {
    std::cerr << "oops exception " << std::endl;
  }

  return 0;
}
