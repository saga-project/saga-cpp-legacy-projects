
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

    digedag::dax::parser p (argv[1], argv[2]);

    boost::shared_ptr <digedag::dag> d = p.get_dag ();

    // allow for pre-run scheduling
    d->schedule (); 
    // d->dump ();

    // run the dag.  This also performs scheduling on-the-fly
    if ( dryrun )
    {
      d->dryrun ();
    }
    else
    {
      std::cout << " === 1 " << std::endl;
      d->fire ();
      std::cout << " === 2 " << std::endl;

      // wait til the dag had a chance to finish
      d->wait ();
      std::cout << " === 3 " << std::endl;
    }
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

