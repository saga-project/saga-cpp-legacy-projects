
#include <iostream>

#include "parser_dax.hpp"

int main (int argc, char** argv)
{
  bool dryrun = false;

  try
  {
    if ( argc != 2 )
    {
      std::cerr << "\n\tusage: " << argv[0] << " <dag.xml>\n\n";
      return -1;
    }

    diggedag::dax::parser p (argv[1]);

    diggedag::dag * d = p.get_dag ();

    d->dump ();

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
      // TODO: implement d->wait ();
      while ( diggedag::Running == d->get_state () )
      {
        ::sleep (1);
        std::cout << "dag    waiting..." << std::endl;
      }
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

