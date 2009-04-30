
#include <iostream>

#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  try
  {
    diggedag::dax::parser p ("../src/parser_dax_example/Montage_25.xml");

    diggedag::dag * d = p.get_dag ();

    // allow for pre-run scheduling
    d->schedule (); 

    // run the dag.  This also performs scheduling on-the-fly
    d->fire ();
   
    std::cout << "dag    running..." << std::endl;

    // wait til the dag had a chance to finish
    // TODO: implement d->wait ();
    while ( diggedag::Running == d->get_state () )
    {
      ::sleep (1);
      std::cout << "dag    waiting..." << std::endl;
    }

    delete d;
  }
  catch ( const char * s )
  {
    std::cerr << "char* exception: " << s << std::endl;
  }
  catch ( const std::string & s )
  {
    std::cerr << "string exception: " << s << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "oops exception: " << std::endl;
  }

  return 0;
}

