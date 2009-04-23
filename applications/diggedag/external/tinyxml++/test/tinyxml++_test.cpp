
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

    // get the top adag element
    ticpp::Element * top = doc.FirstChildElement ("adag");


    // list nodes
    ticpp::Iterator <ticpp::Element> job ("job"); 

    for ( job = job.begin (top); job != job.end (); job++ )
    {
      std::cout << (*job) << std::endl;
    }
    

    // list edges
    ticpp::Iterator <ticpp::Element> child ("child"); 

    for ( child = child.begin (top); child != child.end (); child++ )
    {
      std::cout << "job" << std::endl;

      ticpp::Iterator <ticpp::Attribute> a;

      for ( a = a.begin (element) ; a != a.end (); a++ )
      {
        std::cout << " \t " << (*a) << std::endl;
      }
      
      std::cout << std::endl;
    }
    
  }
  catch ( const ticpp::Exception & e )
  {
    std::cout << e.what () << std::endl;
  }

  return 0;

}

