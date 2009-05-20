
#include <iostream>

#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  try
  {
    digedag::node_description nd;
    nd.set_attribute ("Executable", "/bin/date");

    std::vector <digedag::node *> nodes;
  
    for ( int i = 0; i < 10000; i++ )
    {
      nodes[i] = new digedag::node (nd);
    }

    for ( int i = 0; i < 10000; i++ )
    {
      nodes[i]->fire ();
    }
  }
  catch ( ... )
  {
    std::cerr << "oops exception: " << std::endl;
  }

  return 0;
}

