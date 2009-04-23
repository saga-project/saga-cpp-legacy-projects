
#include "ticpp.hpp"

#include <iostream>

int main (int argc, char** argv)
{
  if ( argc < 2 )
  {
    std::cerr << "\n\tUsage: " << argv[0] << " <file.xml>\n" << std::endl;
    return (1);
  }

  std::string file (argv[1]);

  try
  {
    ticpp::Document doc (file);
    doc.LoadFile ();

    // get the ToDO child
    ticpp::Element * top = doc.FirstChildElement ("ToDo");

    std::cout << (*top) << std::endl;


    ticpp::Iterator <ticpp::Element> child ("Item"); 

    for ( child = child.begin (top); child != child.end (); child++ )
    {
      std::cout << (*child) << std::endl;
    }
    
  }
  catch ( const ticpp::Exception & e )
  {
    std::cout << e.what ();
  }

  return 0;

}

