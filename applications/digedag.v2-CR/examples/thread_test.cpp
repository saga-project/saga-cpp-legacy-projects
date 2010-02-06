
#include <iostream>

#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  try
  {
    digedag::node_description nd;
    nd.set_attribute ("Executable", "/bin/date");

    boost::shared_ptr <digedag::dag>  d  (new digedag::dag (""));

    std::vector <boost::shared_ptr <digedag::node> > nodes;
  
    for ( int i = 0; i < 10000; i++ )
    {
      nodes[i] = d->create_node (nd);
    }

    boost::shared_ptr <digedag::edge> dummy = d->create_edge ();

    for ( int i = 0; i < 10000; i++ )
    {
      nodes[i]->fire (dummy);
    }
  }
  catch ( ... )
  {
    std::cerr << "oops exception: " << std::endl;
  }

  return 0;
}

