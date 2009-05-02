
#include <iostream>

#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  bool dryrun = false;

  try
  {
    if ( argc != 3 )
    {
      std::cerr << "\n\tusage: " << argv[0] << " <dag.xml> <scheduler.txt>\n\n";
      return -1;
    }

    diggedag::dax::parser p (argv[1]);

    diggedag::dag * d = p.get_dag ();

    d->dump ();
    d->set_scheduler (argv[2]);

    // allow for pre-run scheduling
    d->schedule (); 

    // run the dag.  This also performs scheduling on-the-fly
    if ( dryrun )
    {
      d->dryrun ();
    }
    else
    {
      d->fire ();

      std::cout << "dag    running..." << std::endl;

      // wait til the dag had a chance to finish
      d->wait ();
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

