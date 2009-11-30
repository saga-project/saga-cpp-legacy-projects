 
#include "parser_pegasus.hpp"

int main (int argc, char** argv)
{
  try
  {
    digedag::pegasus::parser p ("../src/parser_pegasus_example/black-diamond-0.dag");

    digedag::dag * d = p.get_dag ();

    d->fire ();
   
    std::cout << "dag    running..." << std::endl;

    // wait til the dag had a chance to finish
    while ( digedag::Running == d->get_state () )
    {
      ::sleep (1);
      std::cout << "dag    waiting..." << std::endl;
    }
  }
  catch ( const char * s )
  {
    std::cerr << "char* exception: " << s << std::endl;
  }
  catch ( const std::string & s )
  {
    std::cerr << "string exception: " << s << std::endl;
  }

  return 0;
}

